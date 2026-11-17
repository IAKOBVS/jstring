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

#define TOTAL 2200

static void
check_len(size_t n)
{
	unsigned char src[TOTAL];
	unsigned char dst[TOTAL];
	unsigned char ref[TOTAL];

	rigor_srand(0x3333u + n);
	rigor_fill(src, n, 1);
	for (size_t i = 0; i < TOTAL; ++i)
		dst[i] = ref[i] = 0x88;

	jstr_strmove_len((char *)dst, (const char *)src, n);
	memcpy(ref, src, n);
	ref[n] = 0;

	assert(memcmp(dst, ref, TOTAL) == 0);
}

static void
check_overlap(size_t len, size_t dist, int forward)
{
	unsigned char buf[TOTAL];
	unsigned char ref[TOTAL];

	rigor_srand(0x4444u + len * 149u + dist * 11u + (unsigned)forward);
	rigor_fill(buf, len + dist + 8, 1);
	for (size_t i = len + dist + 8; i < TOTAL; ++i)
		buf[i] = 0x55;
	memcpy(ref, buf, TOTAL);

	char *dst;
	const char *src;
	if (forward) {
		dst = (char *)buf;
		src = (const char *)buf + dist;
		memmove(ref, ref + dist, len);
		ref[len] = 0;
	} else {
		dst = (char *)buf + dist;
		src = (const char *)buf;
		memmove(ref + dist, ref, len);
		ref[dist + len] = 0;
	}

	jstr_strmove_len(dst, src, len);
	assert(memcmp(buf, ref, TOTAL) == 0);
}

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	for (size_t n = 0; n <= 128; ++n) {
		check_len(n);
		for (size_t d = 1; d <= n; ++d) {
			check_overlap(n, d, 1);
			check_overlap(n, d, 0);
		}
	}
#if JSTR_TEST_SLOW
	for (size_t i = 0; i < RIGOR_NLENS; ++i) {
		const size_t n = rigor_lens[i];
		check_len(n);
		for (size_t d = 1; d <= n; ++d) {
			check_overlap(n, d, 1);
			check_overlap(n, d, 0);
		}
	}
#endif

	/* n <, ==, > strlen with an embedded NUL in the source.  The source has
	 * 8 readable bytes so n may legitimately extend past the first NUL. */
	{
		const unsigned char src[8] = { 'a', 'b', 0, 'c', 'd', 0, 0, 0 };
		unsigned char out[16];

		for (size_t n = 0; n <= 6; ++n) {
			memset(out, 0xee, sizeof out);
			jstr_strmove_len((char *)out, (const char *)src, n);
			for (size_t i = 0; i < n; ++i)
				assert(out[i] == src[i]);
			assert(out[n] == 0);
			for (size_t i = n + 1; i < sizeof out; ++i)
				assert(out[i] == 0xee);
		}
	}

	return 0;
}
