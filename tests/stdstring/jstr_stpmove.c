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
	char src[TOTAL];
	char dst[TOTAL];
	char ref[TOTAL];

	rigor_srand(0x5555u + n);
	rigor_fill_printable(src, n);
	for (size_t i = 0; i < TOTAL; ++i)
		dst[i] = ref[i] = (char)0x66;

	char *r = jstr_stpmove(dst, src);
	memcpy(ref, src, n + 1); /* include NUL */
	assert(r == dst + n);
	assert(memcmp(dst, ref, TOTAL) == 0);
}

static void
check_overlap(size_t len, size_t dist, int forward)
{
	char buf[TOTAL];
	char ref[TOTAL];

	rigor_srand(0x6666u + len * 157u + dist * 13u + (unsigned)forward);
	rigor_fill(buf, TOTAL - 1, 0); /* all non-NUL except the final byte */
	memcpy(ref, buf, TOTAL);

	char *dst;
	const char *src;
	if (forward) {
		dst = buf;
		src = buf + dist;
		buf[dist + len] = 0; /* terminate the source string at len */
		ref[dist + len] = 0;
		memmove(ref, ref + dist, len);
		ref[len] = 0;
	} else {
		dst = buf + dist;
		src = buf;
		buf[len] = 0;
		ref[len] = 0;
		memmove(ref + dist, ref, len);
		ref[dist + len] = 0;
	}

	char *r = jstr_stpmove(dst, src);
	assert(r == dst + len);
	assert(memcmp(buf, ref, TOTAL) == 0);
}

int
main(void)
{

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
		char *r = jstr_stpmove(buf, "");
		assert(r == buf);
		assert(buf[0] == 0);
		assert(buf[1] == 'y' && buf[2] == 'z' && buf[3] == 'w');
	}

	/* all-0xff (non-NUL) string copied in full. */
	{
		char src[64], dst[65];
		memset(src, (char)0xff, sizeof src);
		src[sizeof src - 1] = 0;
		memset(dst, 0, sizeof dst);
		char *r = jstr_stpmove(dst, src);
		assert(r == dst + sizeof src - 1);
		assert(memcmp(dst, src, sizeof src) == 0);
	}

	return 0;
}
