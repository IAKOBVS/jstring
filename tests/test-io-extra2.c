#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "jstr.h"
#include "io.h"

int main(void) {
	char tmpdir[] = "/tmp/jstr_io2_XXXXXX";
	assert(mkdtemp(tmpdir));
	char f1[256], f2[256], f3[256];
	snprintf(f1, sizeof f1, "%s/rw.txt", tmpdir);
	snprintf(f2, sizeof f2, "%s/fw.txt", tmpdir);
	snprintf(f3, sizeof f3, "%s/fd.txt", tmpdir);

	/* jstr_io_isbinary_maybe: empty buffer */
	{
		int r = jstr_io_isbinary_maybe("", 0);
		assert(r == 0);
	}
	/* jstr_io_isbinary_maybe: binary data (NUL byte) */
	{
		const char buf[] = "ab\0cd";
		int r = jstr_io_isbinary_maybe(buf, 5);
		assert(r != 0);
	}
	/* jstr_io_isbinary: short text */
	{
		int r = jstr_io_isbinary("hello\n", 6);
		assert(r == 0);
	}
	/* Write a file and read it back via various read methods */
	{
		/* Use data ending with \n to avoid extra newline appending */
		int r = jstr_io_writefile_len("hello\n", 6, f1, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		assert(r == 0);
		/* Read via jstr_io_readfile */
		char *s = NULL;
		size_t sz = 0, cap = 0;
		r = jstr_io_readfile(&s, &sz, &cap, f1, O_RDONLY);
		assert(r == 0);
		assert(sz == 6);
		assert(!strcmp(s, "hello\n"));
		free(s);
		/* Read via jstr_io_readfile_len */
		s = NULL; sz = 0; cap = 0;
		r = jstr_io_readfile_len(&s, &sz, &cap, f1, O_RDONLY, 5);
		assert(r == 0);
		assert(sz == 5);
		assert(!strcmp(s, "hello"));
		free(s);
		/* Read via jstr_io_freadfile */
		s = NULL; sz = 0; cap = 0;
		{
			struct stat st;
			r = jstr_io_freadfile(&s, &sz, &cap, f1, "r", &st);
			assert(r == 0);
			assert(sz == 6);
			assert(!strcmp(s, "hello\n"));
		}
		free(s);
		/* Read via jstr_io_freadfile_len */
		s = NULL; sz = 0; cap = 0;
		r = jstr_io_freadfile_len(&s, &sz, &cap, f1, "r", 5);
		assert(r == 0);
		assert(sz == 5);
		assert(!strcmp(s, "hello"));
		free(s);
		unlink(f1);
	}
	/* Write via jstr_io_fwritefile_len */
	{
		int r = jstr_io_fwritefile_len("world\n", 6, f2, "w");
		assert(r == 0);
		char *s = NULL;
		size_t sz = 0, cap = 0;
		r = jstr_io_readfile(&s, &sz, &cap, f2, O_RDONLY);
		assert(r == 0);
		assert(sz == 6);
		assert(!strcmp(s, "world\n"));
		free(s);
		unlink(f2);
	}
	/* jstr_io_appendpath_len */
	{
		char *path = NULL;
		size_t sz = 0, cap = 0;
		assert(!jstr_chk(jstr_assign_len(&path, &sz, &cap, "/tmp", 4)));
		int r = jstr_io_appendpath_len(&path, &sz, &cap, "sub", 3);
		assert(r == 0);
		assert(!strcmp(path, "/tmp/sub"));
		free(path);
	}
	/* jstr_io_expandtildefirst with tilde (in-place) */
	{
		const char *home = getenv("HOME");
		if (home) {
			char buf[512];
			strcpy(buf, "~/x");
			char *r = jstr_io_expandtildefirst_len_unsafe_p(buf, sizeof buf);
			assert(r > buf);
			assert(buf[0] == '/');
		}
	}
	/* jstr_io_writefilefd_len */
	{
		int fd = open(f3, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		assert(fd >= 0);
		int r = jstr_io_writefilefd_len("testdata", 8, fd);
		assert(r == 0);
		close(fd);
		unlink(f3);
	}

	/* Cleanup */
	char cmd[300];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	return 0;
}
