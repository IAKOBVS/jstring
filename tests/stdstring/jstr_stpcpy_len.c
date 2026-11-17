/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "jstr.h"
#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

#define SZ 1400

static void
check_len(size_t n)
{
	unsigned char src[SZ];
	unsigned char dst[SZ];
	unsigned char ref[SZ];

	rigor_srand(0x5678u + n);
	rigor_fill(src, n, 1);
	for (size_t i = 0; i < SZ; ++i)
		dst[i] = ref[i] = 0xcc;

	char *r = jstr_stpcpy_len((char *)dst, (const char *)src, n);
	memcpy(ref, src, n);
	ref[n] = 0;

	assert(r == (char *)dst + n);
	assert(memcmp(dst, ref, SZ) == 0);
}

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	for (size_t n = 0; n <= 128; ++n)
		check_len(n);
#if JSTR_TEST_SLOW
	for (size_t i = 0; i < RIGOR_NLENS; ++i)
		check_len(rigor_lens[i]);
#endif

	/* n <, ==, > strlen with an embedded NUL in the source.  The source has
	 * 8 readable bytes so n may legitimately extend past the first NUL. */
	{
		const unsigned char src[8] = { 'a', 'b', 0, 'c', 'd', 0, 0, 0 };
		unsigned char out[16];

		for (size_t n = 0; n <= 6; ++n) {
			memset(out, 0xee, sizeof out);
			char *r = jstr_stpcpy_len((char *)out, (const char *)src, n);
			assert(r == (char *)out + n);
			for (size_t i = 0; i < n; ++i)
				assert(out[i] == src[i]);
			assert(out[n] == 0);
			for (size_t i = n + 1; i < sizeof out; ++i)
				assert(out[i] == 0xee);
		}
	}

	/* Empty source and n == 0. */
	{
		char out[4] = { 'x', 'y', 'z', 'w' };
		char *r = jstr_stpcpy_len(out, "", 0);
		assert(r == out);
		assert(out[0] == 0);
		assert(out[1] == 'y' && out[2] == 'z' && out[3] == 'w');
	}

	return 0;
}
