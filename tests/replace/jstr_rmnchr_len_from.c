/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../rigor.h"
#include "replace.h"

#define ALL ((size_t)-1)

#if !(defined(JSTR_TEST_SLOW) && JSTR_TEST_SLOW)
static const size_t fast_lens[] = { 0, 1, 2, 3, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
#endif

/* Reference: remove up to LIM occurrences of C at/after START, binary-safe. */
static size_t
ref_rmnchr_len_from(unsigned char *dst, size_t *out_sz, const unsigned char *src, size_t n, size_t start, int c, size_t lim)
{
	size_t w = 0, removed = 0;
	for (size_t i = 0; i < start && i < n; ++i)
		dst[w++] = src[i];
	for (size_t i = start; i < n; ++i) {
		if ((unsigned char)c == src[i] && removed < lim) {
			++removed;
			continue;
		}
		dst[w++] = src[i];
	}
	dst[w] = 0;
	*out_sz = w;
	return removed;
}

static void
check(const char *input, size_t start, int c, size_t lim, const char *expect, size_t expect_ret)
{
	char b[512];
	size_t len = strlen(input);
	size_t sz = len;
	RCHECK(len + 1 <= sizeof b);
	memcpy(b, input, len + 1);
	size_t r = jstr_rmnchr_len_from(b, &sz, start, c, lim);
	RCHECK_EQ(r, expect_ret);
	RCHECK_EQ(sz, strlen(expect));
	RCHECK_MEM(b, expect, sz + 1);
	RCHECK(b[sz] == '\0');
}

int
main(void)
{
	/* No-op cases. */
	check("", 0, 'a', 1, "", 0);
	check("", 0, 'a', ALL, "", 0);
	check("abc", 0, 'z', 1, "abc", 0);
	check("abc", 0, 'z', ALL, "abc", 0);
	check("abc", 0, 'a', 0, "abc", 0);
	check("abc", 3, 'a', ALL, "abc", 0);
	check("abc", 3, 'a', 1, "abc", 0);

	/* Prefix before start_idx must be untouched. */
	check("aXbXc", 1, 'a', ALL, "aXbXc", 0);
	check("aXaXa", 2, 'a', ALL, "aXX", 2);
	check("aXaXa", 4, 'a', ALL, "aXaX", 1);
	check("aXaXa", 2, 'X', ALL, "aXaa", 1);
	check("aXaXa", 2, 'X', 1, "aXaa", 1);
	check("aXaXa", 3, 'X', 1, "aXaa", 1);

	/* n limits at/after START. */
	check("aaaa", 0, 'a', 1, "aaa", 1);
	check("aaaa", 0, 'a', 2, "aa", 2);
	check("aaaa", 0, 'a', 3, "a", 3);
	check("aaaa", 0, 'a', 4, "", 4);
	check("aaaa", 0, 'a', ALL, "", 4);
	check("aaaa", 1, 'a', 1, "aaa", 1);
	check("aaaa", 1, 'a', 2, "aa", 2);
	check("aaaa", 1, 'a', ALL, "a", 3);
	check("baaab", 0, 'a', 2, "bab", 2);
	check("baaab", 2, 'a', 2, "bab", 2);
	check("baaab", 3, 'a', 2, "baab", 1);

	/* Exact-length prefixes (start == sz is a no-op). */
	check("abcabc", 6, 'a', ALL, "abcabc", 0);
	check("abcabc", 0, 'a', ALL, "bcbc", 2);

	/* Binary data: embedded NULs are preserved, removals do not stop at them. */
	{
		char b[8] = { 'a', 0, 'a', 'b', 0, 'a', 0, 0 };
		size_t sz = 6;
		size_t r = jstr_rmnchr_len_from(b, &sz, 0, 'a', ALL);
		RCHECK_EQ(r, 3);
		RCHECK_EQ(sz, 3);
		RCHECK_MEM(b, "\0b\0", 4);
		RCHECK(b[sz] == '\0');

		memcpy(b, "\0a\0a\0a", 7);
		sz = 6;
		r = jstr_rmnchr_len_from(b, &sz, 3, 'a', ALL);
		RCHECK_EQ(r, 2);
		RCHECK_EQ(sz, 4);
		RCHECK_MEM(b, "\0a\0\0", 5);
	}

	/* Differential sweep against the reference implementation. */
	{
		unsigned char src[1026], expect[1026], got[1026];
		const size_t *lens;
		size_t nlens;
#if defined(JSTR_TEST_SLOW) && JSTR_TEST_SLOW
		lens = rigor_lens;
		nlens = RIGOR_NLENS;
		const size_t trials = 64;
#else
		lens = fast_lens;
		nlens = sizeof fast_lens / sizeof fast_lens[0];
		const size_t trials = 8;
#endif
		rigor_srand(0x5eed1234u);
		for (size_t li = 0; li < nlens; ++li) {
			const size_t n = lens[li];
			for (size_t trial = 0; trial < trials; ++trial) {
				rigor_fill(src, n, 1);
				int c;
				switch (trial % 5) {
				case 0: c = 0; break;
				case 1: c = 'a'; break;
				case 2: c = 0xff; break;
				case 3: c = 'z'; break;
				default: c = src[rigor_randn(n ? n : 1)]; break;
				}
				size_t start = n ? rigor_randn(n + 1) : 0;
				if (start < n && src[start] == 0)
					src[start] = (unsigned char)('A' + (trial % 26));
				size_t lim = (trial % 4 == 0) ? 0 : (trial % 4 == 1)   ? 1
				                                    : (trial % 4 == 2) ? 2
				                                                       : ALL;
				size_t expect_sz = 0;
				size_t expect_ret = ref_rmnchr_len_from(expect, &expect_sz, src, n, start, c, lim);
				memcpy(got, src, n + 1);
				size_t sz = n;
				size_t r = jstr_rmnchr_len_from((char *)got, &sz, start, c, lim);
				RCHECK_EQ(r, expect_ret);
				RCHECK_EQ(sz, expect_sz);
				RCHECK_MEM(got, expect, sz + 1);
				RCHECK(got[sz] == 0);
			}
		}
	}

	return 0;
}
