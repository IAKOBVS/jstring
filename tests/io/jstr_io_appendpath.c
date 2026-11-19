/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"

#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
main(void)
{

	/* Basic append and exact end pointer. */
	{
		char buf[64];
		memcpy(buf, jstr_literal("dir"));
		char *p = jstr_io_appendpath_p(buf, 3, "file");
		assert(strcmp(buf, "dir/file") == 0);
		assert(p == buf + 8);
		assert(*p == '\0');
	}

	/* Leading slash in the component is preserved (no deduplication). */
	{
		char buf[64];
		memcpy(buf, jstr_literal("dir"));
		char *p = jstr_io_appendpath_p(buf, 3, "/file");
		assert(strcmp(buf, "dir//file") == 0);
		assert(p == buf + 9);
	}

	/* Trailing slash plus empty component yields a double separator. */
	{
		char buf[64];
		memcpy(buf, jstr_literal("dir/"));
		char *p = jstr_io_appendpath_p(buf, 4, "");
		assert(strcmp(buf, "dir//") == 0);
		assert(p == buf + 5);
		assert(*p == '\0');
	}

	/* Empty base and dot-dot components. */
	{
		char buf[64];
		buf[0] = 'X';
		char *p = jstr_io_appendpath_p(buf, 0, "f");
		assert(strcmp(buf, "/f") == 0);
		assert(p == buf + 2);

		memcpy(buf, jstr_literal("dir/sub"));
		p = jstr_io_appendpath_p(buf, 7, "..");
		assert(strcmp(buf, "dir/sub/..") == 0);
		assert(p == buf + 10);
	}

	/* Managed appends, including empty components, with growth. */
	{
		static const char *const pieces[] = { "", "a", "b/c", "..", "leaf" };
		char expect[4096];
		char *s = NULL;
		size_t sz = 0, cap = 0, esz = 0;
		expect[0] = '\0';

		for (int i = 0; i < 200; ++i) {
			const char *piece = pieces[i % 5];
			const size_t n = strlen(piece);
			assert(esz + n + 2 < sizeof expect);
			expect[esz++] = '/';
			memcpy(expect + esz, piece, n);
			esz += n;
			expect[esz] = '\0';
			assert(jstr_io_appendpath_len(&s, &sz, &cap, piece, n) == JSTR_RET_SUCC);
			assert(sz == esz);
			assert(s[sz] == '\0');
			assert(cap == 0 || cap > sz);
			assert(memcmp(s, expect, esz + 1) == 0);
		}
		assert(cap > sz);

		/* Repeated reserve preserves an append path. */
		{
			const size_t saved_sz = sz;
			assert(jstr_reserve(&s, &sz, &cap, sz + 2048) == JSTR_RET_SUCC);
			assert(sz == saved_sz);
			assert(memcmp(s, expect, esz + 1) == 0);
		}

		jstr_free(&s, &sz, &cap);
	}

	/* A path near PATH_MAX still appends and terminates correctly. */
	{
		char name[5001];
		char *s = NULL;
		size_t sz = 0, cap = 0;
		memset(name, 'a', sizeof name - 1);
		name[sizeof name - 1] = '\0';

		assert(jstr_io_appendpath_len(&s, &sz, &cap, "root", 4) == JSTR_RET_SUCC);
		assert(sz == 5);
		assert(strcmp(s, "/root") == 0);
		assert(jstr_io_appendpath_len(&s, &sz, &cap, name, 5000) == JSTR_RET_SUCC);
		assert(sz == 5006);
		assert(s[0] == '/');
		assert(s[1] == 'r' && s[4] == 't');
		assert(s[5] == '/');
		assert(s[6] == 'a');
		assert(s[5005] == 'a');
		assert(s[5006] == '\0');
		assert(cap > sz);
		assert(jstr_io_appendpath_len(&s, &sz, &cap, "", 0) == JSTR_RET_SUCC);
		assert(sz == 5007);
		assert(s[5006] == '/');
		assert(s[5007] == '\0');
		jstr_free(&s, &sz, &cap);
	}

	/* Filesystem round-trips through a path built by appendpath. */
	{
		char tmpdir[] = "/tmp/jstr_harden_appendpath_XXXXXX";
		char file[4096];
		const size_t dirlen = strlen(tmpdir);
		char *s = NULL;
		size_t sz = 0, cap = 0;

		assert(mkdtemp(tmpdir) != NULL);
		memcpy(file, tmpdir, dirlen + 1);
		{
			char *end = jstr_io_appendpath_p(file, dirlen, "bin.dat");
			assert(end == file + strlen(file));
			assert(strcmp(end - strlen("bin.dat"), "bin.dat") == 0);
		}

		/* Empty file. */
		assert(jstr_io_writefile_len("", 0, file, O_CREAT | O_TRUNC, 0644) == JSTR_RET_SUCC);
		assert(jstr_io_readfile_len(&s, &sz, &cap, file, O_RDONLY, 0) == JSTR_RET_SUCC);
		assert(sz == 0);
		assert(s != NULL);
		assert(s[0] == '\0');
		free(s);
		s = NULL;
		sz = 0;
		cap = 0;

		/* One byte: writefile adds a newline, so two bytes are stored. */
		assert(jstr_io_writefile_len("A", 1, file, O_CREAT | O_TRUNC, 0644) == JSTR_RET_SUCC);
		assert(jstr_io_readfile_len(&s, &sz, &cap, file, O_RDONLY, 2) == JSTR_RET_SUCC);
		assert(sz == 2);
		assert(memcmp(s, "A\n", 2) == 0);
		assert(s[2] == '\0');
		free(s);
		s = NULL;
		sz = 0;
		cap = 0;

		/* Binary with embedded NUL and 0xFF, ending in newline. */
		{
			const char bin[] = { 'a', '\0', 'b', (char)0xFF, '\n' };
			assert(jstr_io_writefile_len(bin, sizeof bin, file, O_CREAT | O_TRUNC, 0644) == JSTR_RET_SUCC);
			assert(jstr_io_readfile_len(&s, &sz, &cap, file, O_RDONLY, sizeof bin) == JSTR_RET_SUCC);
			assert(sz == sizeof bin);
			assert(memcmp(s, bin, sizeof bin) == 0);
			assert(s[sizeof bin] == '\0');
			free(s);
			s = NULL;
			sz = 0;
			cap = 0;
		}

		/* Multi-KiB binary-safe round-trip. */
		{
			char big[5000];
			for (size_t i = 0; i < sizeof big; ++i)
				big[i] = (char)(i % 251);
			big[sizeof big - 1] = '\n';
			assert(jstr_io_writefile_len(big, sizeof big, file, O_CREAT | O_TRUNC, 0644) == JSTR_RET_SUCC);
			assert(jstr_io_readfile_len(&s, &sz, &cap, file, O_RDONLY, sizeof big) == JSTR_RET_SUCC);
			assert(sz == sizeof big);
			assert(memcmp(s, big, sizeof big) == 0);
			assert(s[sizeof big] == '\0');
			free(s);
		}

		assert(unlink(file) == 0);
		assert(rmdir(tmpdir) == 0);
	}

	return 0;
}
