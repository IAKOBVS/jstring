/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "../rigor.h"
#include "io.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

	/* Basic copy plus NUL. */
	{
		char dst[64];
		memset(dst, 0xA5, sizeof dst);
		jstr_strmove_len(dst, jstr_literal("hello"));
		assert(strcmp(dst, "hello") == 0);
		assert(dst[5] == '\0');
		for (size_t i = 6; i < 16; ++i)
			assert((unsigned char)dst[i] == 0xA5);
	}

	/* n == 0 writes only the NUL at dst[0]. */
	{
		char dst[8];
		memset(dst, 0xA5, sizeof dst);
		jstr_strmove_len(dst, "xyz", 0);
		assert(dst[0] == '\0');
		for (size_t i = 1; i < sizeof dst; ++i)
			assert((unsigned char)dst[i] == 0xA5);
	}

	/* Binary data with embedded NULs is copied for exactly n bytes. */
	{
		const char src[] = { 'a', '\0', 'b', (char)0xFF, 'c' };
		char dst[16];
		memset(dst, 0x5A, sizeof dst);
		jstr_strmove_len(dst, src, sizeof src);
		assert(memcmp(dst, src, sizeof src) == 0);
		assert(dst[sizeof src] == '\0');
		assert((unsigned char)dst[sizeof src + 1] == 0x5A);
	}

	/* Overlapping copy with dst < src (memmove semantics). */
	{
		char buf[64];
		memcpy(buf, jstr_literal("0123456789"));
		jstr_strmove_len(buf, buf + 4, 6);
		assert(memcmp(buf, jstr_literal("456789\0")) == 0);
		assert(buf[6] == '\0');
		assert(buf[7] == '7');
	}

	/* Overlapping copy with dst > src. */
	{
		char buf[64];
		memcpy(buf, jstr_literal("abcdef"));
		jstr_strmove_len(buf + 2, buf, 4);
		assert(memcmp(buf, jstr_literal("ababcd\0")) == 0);
		assert(buf[6] == '\0');
	}

	/* Self-copy is a no-op apart from rewriting the existing NUL. */
	{
		char buf[16];
		memcpy(buf, jstr_literal("self"));
		jstr_strmove_len(buf, buf, 4);
		assert(strcmp(buf, "self") == 0);
		assert(buf[4] == '\0');
	}

	/* Differential sweep over scalar/SIMD boundary lengths. */
	{
		char src[2064];
		char dst[2064];
		for (size_t li = 0; li < RIGOR_NLENS; ++li) {
			const size_t n = rigor_lens[li];
			if (n > 2048)
				continue;
			rigor_srand(0x123456789abcdefULL ^ n);
			rigor_fill(src, n, 1);
			memset(dst, 0xA5, sizeof dst);
			jstr_strmove_len(dst, src, n);
			assert(memcmp(dst, src, n) == 0);
			assert(dst[n] == '\0');
			for (size_t i = n + 1; i < n + 17 && i < sizeof dst; ++i)
				assert((unsigned char)dst[i] == 0xA5);
		}
	}

	/* Unaligned source/destination combinations. */
	{
		char src[300];
		char dst[300];
		rigor_srand(0xfeedfacecafebeefULL);
		rigor_fill(src + 1, 255, 1);
		for (size_t off = 0; off < 64; ++off) {
			memset(dst, 0x33, sizeof dst);
			jstr_strmove_len(dst + off, src + (off % 7), 128);
			assert(memcmp(dst + off, src + (off % 7), 128) == 0);
			assert(dst[off + 128] == '\0');
			if (off > 0)
				assert((unsigned char)dst[off - 1] == 0x33);
		}
	}

	SUCCESS();
	return 0;
}
