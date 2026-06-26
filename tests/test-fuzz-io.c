/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Fuzz test for I/O helper functions: binary detection,
 * path manipulation, tilde expansion. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/io.h"
#include "fuzz-shared.h"

JSTR_ATTR_MAYBE_UNUSED
static void
fuzz_io(size_t iter)
{
	char h[FSTR];
	for (size_t i = 0; i < iter; ++i) {
		enum gen_mode hmode = (enum gen_mode)(rand() % 4);
		size_t hl = gen_str(h, FSTR - 1, hmode);

		/* --- jstr_isbinary / jstr_io_isbinary_maybe --- */
		/* jstr_isbinary: first N bytes checked for control chars */
		for (size_t n = 0; n <= hl + 2; ++n) {
			int r = jstr_isbinary(h, hl, n);
			if (n == 0 || hl == 0) {
				assert(r == 0);
			} else {
				size_t check = JSTR_MIN(n, hl);
		int found = 0;
		for (size_t j = 0; j < check; ++j) {
			if (((unsigned char)h[j] < ' ' && h[j] != '\t' && h[j] != '\n') || (unsigned char)h[j] == 127)
				found = 1;
		}
				assert(r == found);
			}
		}

		/* --- jstr_io_isbinary_maybe --- */
		int mb = jstr_io_isbinary_maybe(h, hl);
		if (hl == 0)
			assert(mb == 0);
		else if (hl >= 4 && (unsigned char)h[0] == 0x7 && h[1] == 'E' && h[2] == 'L' && h[3] == 'F')
			assert(mb == 0);
		else if (hl >= 3 && (unsigned char)h[0] == 0xEF && (unsigned char)h[1] == 0xBB && (unsigned char)h[2] == 0xBF)
			assert(mb == 1);
		/* otherwise, result depends on first 64 bytes for control chars */

		/* --- jstr_io_isbinary --- */
		/* Checks signature + NUL detection (strlen != sz) */
		int ib = jstr_io_isbinary(h, hl);
		if (hl >= 4 && (unsigned char)h[0] == 0x7 && h[1] == 'E' && h[2] == 'L' && h[3] == 'F')
			assert(ib == 0);
		else if (hl >= 3 && (unsigned char)h[0] == 0xEF && (unsigned char)h[1] == 0xBB && (unsigned char)h[2] == 0xBF)
			assert(ib == 1);
		else {
			/* embedded NUL -> binary */
			int has_nul = 0;
			for (size_t j = 0; j < hl; ++j)
				if (h[j] == '\0')
					has_nul = 1;
			assert(ib == has_nul);
		}

		/* --- jstr_io_appendpath_len_p / jstr_io_appendpath_p --- */
		if (hl > 0) {
			char buf[FSTR + JSTR_IO_NAME_MAX + 2];
			memcpy(buf, h, hl);
			buf[hl] = '\0';
			/* These functions require path to NOT end with '/' */
			size_t path_end = hl;
			while (path_end > 0 && buf[path_end - 1] == '/')
				--path_end;
			if (path_end > 0 && hl < FSTR - 2) {
				/* _len variant */
				const char *fname = "testfile";
				size_t flen = 8;
				char *end = jstr_io_appendpath_len_p(buf, path_end, fname, flen);
				assert(end >= buf);
				assert(*end == '\0');
				assert(buf[path_end] == '/');
				assert(memcmp(buf + path_end + 1, fname, flen) == 0);
				/* Restore for _p variant */
				memcpy(buf, h, hl);
				buf[hl] = '\0';
				/* _p variant */
				end = jstr_io_appendpath_p(buf, path_end, fname);
				assert(end >= buf);
				assert(*end == '\0');
				assert(buf[path_end] == '/');
				assert(strcmp(buf + path_end + 1, fname) == 0);
			}
		}

		/* --- jstr_io_appendpath_len (safe) --- */
		char *s = NULL;
		size_t sz = 0, cap = 0;
		if (hl > 0 && hl < FSTR - 2) {
			/* set up */
			int ret = jstr_assign_len(&s, &sz, &cap, h, hl);
			assert(ret == JSTR_RET_SUCC);
			/* append path */
			const char *fname = "subdir";
			size_t flen = 6;
			ret = jstr_io_appendpath_len(&s, &sz, &cap, fname, flen);
			assert(ret == JSTR_RET_SUCC);
			assert(sz == hl + 1 + flen);
			assert(s[hl] == '/');
			assert(memcmp(s + hl + 1, fname, flen) == 0);
			assert(s[sz] == '\0');
		}
		jstr_free(&s, &sz, &cap);
	}
}

#ifndef FUZZ_LIBFUZZER

int
main(int argc, char **argv)
{
	START();
	unsigned seed = 0;
	if (argc > 1)
		seed = (unsigned)atoi(argv[1]);
	size_t iter = FITER;
#ifdef JSTR_TEST_SLOW
	iter *= 2;
#else
	if (iter > 50)
		iter = 50;
#endif
	if (iter < 1)
		iter = 1;
#ifdef FUZZ_AFL
	while (__AFL_LOOP(10000))
#endif
	{
		srand(seed);
		TESTING(fuzz_io);
		fuzz_io(iter);
	}
	SUCCESS();
	return EXIT_SUCCESS;
}

#else

int
LLVMFuzzerTestOneInput(const unsigned char *data, size_t size)
{
	if (size < 4)
		return 0;
	unsigned seed = (unsigned)data[0] | ((unsigned)data[1] << 8) |
	                ((unsigned)data[2] << 16) | ((unsigned)data[3] << 24);
	srand(seed);
	data += 4;
	size -= 4;
	size_t iter = size > 0 ? size : 10;
	if (iter > 100)
		iter = 100;
	fuzz_io(iter);
	return 0;
}

#endif
