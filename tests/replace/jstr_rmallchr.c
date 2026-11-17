/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../rigor.h"
#include "replace.h"

#if !(defined(JSTR_TEST_SLOW) && JSTR_TEST_SLOW)
static const size_t fast_lens[] = { 0, 1, 2, 3, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
#endif

static size_t
ref_rmallchr(unsigned char *dst, size_t *out_sz, const unsigned char *src, size_t n, int c)
{
	size_t w = 0, removed = 0;
	for (size_t i = 0; i < n; ++i) {
		if ((unsigned char)c == src[i]) {
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
check(const char *input, int c, const char *expect, size_t expect_ret)
{
	char b[512];
	size_t len = strlen(input);
	size_t sz = len;
	RCHECK(len + 1 <= sizeof b);
	memcpy(b, input, len + 1);
	size_t r = jstr_rmallchr(b, &sz, c);
	RCHECK_EQ(r, expect_ret);
	RCHECK_EQ(sz, strlen(expect));
	RCHECK_MEM(b, expect, sz + 1);
	RCHECK(b[sz] == '\0');
}

int
main(void)
{
	check("", 'a', "", 0);
	check("abc", 'z', "abc", 0);
	check("aaaa", 'a', "", 4);
	check("banana", 'a', "bnn", 3);
	check("banana", 'n', "baaa", 2);
	check("banana", 'b', "anana", 1);
	check("aabbcc", 'a', "bbcc", 2);
	check("aabbcc", 'b', "aacc", 2);
	check("aabbcc", 'c', "aabb", 2);
	check("abcabc", 'a', "bcbc", 2);
	check("abcabc", 'b', "acac", 2);
	check("abcabc", 'c', "abab", 2);
	check("mississippi", 's', "miiippi", 4);
	check("mississippi", 'i', "msssspp", 4);

	/* Differential sweep. */
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
		rigor_srand(0x55667788u);
		for (size_t li = 0; li < nlens; ++li) {
			const size_t n = lens[li];
			for (size_t trial = 0; trial < trials; ++trial) {
				rigor_fill_printable(src, n);
				int c;
				switch (trial % 4) {
				case 0: c = 'a'; break;
				case 1: c = 'z'; break;
				case 2: c = '!'; break;
				default: c = src[rigor_randn(n ? n : 1)]; break;
				}
				size_t expect_sz = 0;
				size_t expect_ret = ref_rmallchr(expect, &expect_sz, src, n, c);
				memcpy(got, src, n + 1);
				size_t sz = n;
				size_t r = jstr_rmallchr((char *)got, &sz, c);
				RCHECK_EQ(r, expect_ret);
				RCHECK_EQ(sz, expect_sz);
				RCHECK_MEM(got, expect, sz + 1);
				RCHECK(got[sz] == 0);
			}
		}
	}

	return 0;
}
