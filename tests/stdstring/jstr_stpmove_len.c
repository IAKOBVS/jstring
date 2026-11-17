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

	rigor_srand(0x1111u + n);
	rigor_fill(src, n, 1);
	for (size_t i = 0; i < TOTAL; ++i)
		dst[i] = ref[i] = 0x77;

	char *r = jstr_stpmove_len((char *)dst, (const char *)src, n);
	memcpy(ref, src, n);
	ref[n] = 0;

	assert(r == (char *)dst + n);
	assert(memcmp(dst, ref, TOTAL) == 0);
}

static void
check_overlap(size_t len, size_t dist, int forward)
{
	unsigned char buf[TOTAL];
	unsigned char ref[TOTAL];

	rigor_srand(0x2222u + len * 131u + dist * 7u + (unsigned)forward);
	rigor_fill(buf, len + dist + 8, 1);
	for (size_t i = len + dist + 8; i < TOTAL; ++i)
		buf[i] = 0x33; /* canary tail */
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

	char *r = jstr_stpmove_len(dst, src, len);
	assert(r == dst + len);
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

	/* Empty string: only the terminator is written. */
	{
		char buf[4] = { 'x', 'y', 'z', 'w' };
		char *r = jstr_stpmove_len(buf, "", 0);
		assert(r == buf);
		assert(buf[0] == 0);
		assert(buf[1] == 'y' && buf[2] == 'z' && buf[3] == 'w');
	}

	return 0;
}
