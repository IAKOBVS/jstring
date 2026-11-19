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
check(size_t sz, size_t n, int c)
{
	unsigned char hay[SZ];
	const size_t eff = n < sz ? n : sz;

	rigor_srand((uint64_t)(sz * 131u + n * 17u + (unsigned)c));
	rigor_fill(hay, sz, 1);
	if (sz && eff) {
		/* Plant the target so hits at various boundaries are exercised. */
		size_t pos = rigor_randn(eff);
		hay[pos] = (unsigned char)c;
	}

	void *expected = memchr(hay, c, eff);
	char *got = jstr_strnchr_len((const char *)hay, c, sz, n);
	assert(got == (char *)expected);
	if (expected != NULL)
		assert((size_t)(got - (char *)hay) == (size_t)((unsigned char *)expected - hay));
}

int
main(void)
{

	static const int needles[] = { 0, 'a', 'A', 0x7f, 0x80, 0xff, 0x100, 0x141, -1, -128 };

	for (size_t sz = 0; sz <= 128; ++sz) {
		for (size_t n = 0; n <= 128; ++n) {
			for (size_t k = 0; k < sizeof needles / sizeof needles[0]; ++k)
				check(sz, n, needles[k]);
		}
	}
#if JSTR_TEST_SLOW
	for (size_t i = 0; i < RIGOR_NLENS; ++i) {
		const size_t sz = rigor_lens[i];
		const size_t ns[] = { 0,
		                      1,
		                      sz ? sz - 1 : 0,
		                      sz,
		                      sz + 1,
		                      sz + 16,
		                      sz + 129 };
		for (size_t j = 0; j < sizeof ns / sizeof ns[0]; ++j) {
			for (size_t k = 0; k < sizeof needles / sizeof needles[0]; ++k)
				check(sz, ns[j], needles[k]);
		}
	}
#endif

	/* Truncation semantics: n < sz, n == sz, n > sz. */
	{
		const char hay[] = { 'x', 'y', 'z', 'w' };
		assert(jstr_strnchr_len(hay, 'z', 4, 0) == NULL);
		assert(jstr_strnchr_len(hay, 'z', 4, 2) == NULL);
		assert(jstr_strnchr_len(hay, 'z', 4, 3) == hay + 2);
		assert(jstr_strnchr_len(hay, 'z', 2, 4) == NULL); /* sz wins */
		assert(jstr_strnchr_len(hay, 'w', 2, 4) == NULL);
		assert(jstr_strnchr_len(hay, 'w', 4, 4) == hay + 3);
		assert(jstr_strnchr_len(hay, 'x', 4, 100) == hay);
	}

	/* Byte values outside [0, 255] are truncated like memchr. */
	{
		const char hay[] = { 0, (char)0xff };
		assert(jstr_strnchr_len(hay, 0x100, 2, 2) == hay);
		assert(jstr_strnchr_len(hay, -1, 2, 2) == hay + 1);
		assert(jstr_strnchr_len(hay, 0x1ff, 2, 2) == hay + 1);
	}

	return 0;
}
