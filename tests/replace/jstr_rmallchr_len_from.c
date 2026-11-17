/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Note: despite the "_from" suffix, this function has no start_idx parameter
 * (see replace.h); it removes all C from offset 0. Tests below cover that API. */

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
ref_rmallchr_len(unsigned char *dst, size_t *out_sz, const unsigned char *src, size_t n, int c)
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
	size_t r = jstr_rmallchr_len_from(b, &sz, c);
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
	check("aXaXa", 'X', "aaa", 2);
	check("aXaXa", 'a', "XX", 3);
	check("abcabc", 'a', "bcbc", 2);
	check("abcabc", 'b', "acac", 2);
	check("abcabc", 'c', "abab", 2);
	check("mississippi", 's', "miiippi", 4);
	check("mississippi", 'i', "msssspp", 4);

	/* Embedded NULs are preserved. */
	{
		char b[8] = { 'a', 0, 'a', 'b', 0, 'a', 0, 0 };
		size_t sz = 6;
		size_t r = jstr_rmallchr_len_from(b, &sz, 'a');
		RCHECK_EQ(r, 3);
		RCHECK_EQ(sz, 3);
		RCHECK_MEM(b, "\0b\0", 4);
		RCHECK(b[sz] == '\0');
	}

	/* Must agree with the plain (start-0) variant on every input. */
	{
		char a[128], c[128];
		size_t asz, csz, ar, cr;
		for (size_t n = 0; n < 100; ++n) {
			for (size_t i = 0; i < n; ++i)
				a[i] = (char)(rigor_randbyte() | 1);
			a[n] = 0;
			memcpy(c, a, n + 1);
			asz = csz = n;
			ar = jstr_rmallchr_len_from(a, &asz, 'a');
			cr = jstr_rmallchr_len(c, &csz, 'a');
			RCHECK_EQ(ar, cr);
			RCHECK_EQ(asz, csz);
			RCHECK_MEM(a, c, csz + 1);
		}
	}

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
		rigor_srand(0x31415926u);
		for (size_t li = 0; li < nlens; ++li) {
			const size_t n = lens[li];
			for (size_t trial = 0; trial < trials; ++trial) {
				rigor_fill(src, n, 1);
				if (n && src[0] == 0)
					src[0] = (unsigned char)('A' + (trial % 26));
				int c;
				switch (trial % 4) {
				case 0: c = 0; break;
				case 1: c = 'a'; break;
				case 2: c = 0xff; break;
				default: c = src[rigor_randn(n ? n : 1)]; break;
				}
				size_t expect_sz = 0;
				size_t expect_ret = ref_rmallchr_len(expect, &expect_sz, src, n, c);
				memcpy(got, src, n + 1);
				size_t sz = n;
				size_t r = jstr_rmallchr_len_from((char *)got, &sz, c);
				RCHECK_EQ(r, expect_ret);
				RCHECK_EQ(sz, expect_sz);
				RCHECK_MEM(got, expect, sz + 1);
				RCHECK(got[sz] == 0);
			}
		}
	}

	return 0;
}
