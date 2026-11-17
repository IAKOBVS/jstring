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

/* Remove FIND_LEN bytes at AT; the caller guarantees AT + FIND_LEN <= SZ. */
static size_t
ref_rmat(unsigned char *dst, const unsigned char *src, size_t n, size_t at, size_t find_len)
{
	size_t w = 0;
	for (size_t i = 0; i < at; ++i)
		dst[w++] = src[i];
	for (size_t i = at + find_len; i < n; ++i)
		dst[w++] = src[i];
	dst[w] = 0;
	return w;
}

static void
check(const char *input, size_t at, size_t find_len, const char *expect)
{
	char b[512];
	size_t len = strlen(input);
	RCHECK(len + 1 <= sizeof b);
	RCHECK(at + find_len <= len);
	memcpy(b, input, len + 1);
	char *p = jstr_rmat_len_p(b, len, at, find_len);
	RCHECK(p != NULL);
	RCHECK_EQ(p - b, (long)(len - find_len));
	RCHECK_EQ(p, b + len - find_len);
	RCHECK(*p == '\0');
	RCHECK_EQ(strlen(b), strlen(expect));
	RCHECK_STR(b, expect);
}

int
main(void)
{
	check("", 0, 0, "");
	check("abcdef", 0, 0, "abcdef");
	check("abcdef", 6, 0, "abcdef");
	check("abcdef", 0, 1, "bcdef");
	check("abcdef", 0, 3, "def");
	check("abcdef", 0, 6, "");
	check("abcdef", 3, 0, "abcdef");
	check("abcdef", 3, 3, "abc");
	check("abcdef", 6, 0, "abcdef");
	check("abcdef", 2, 2, "abef");
	check("abcdef", 1, 4, "af");
	check("abcdef", 5, 1, "abcde");
	check("abcdef", 4, 2, "abcd");
	check("aXbXcX", 1, 2, "aXcX");
	check("aXbXcX", 3, 2, "aXbX");
	check("aXbXcX", 5, 1, "aXbXc");

	/* Binary data, including embedded NULs. */
	{
		char b[8] = { 'a', 0, 'b', 'c', 0, 'd', 0, 0 };
		char *p = jstr_rmat_len_p(b, 6, 1, 2);
		RCHECK_EQ(p - b, 4);
		RCHECK(*p == '\0');
		RCHECK_MEM(b, "ac\0d", 5);
		RCHECK(b[4] == '\0');
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
		const size_t trials = 16;
#endif
		rigor_srand(0x0a1b2c3du);
		for (size_t li = 0; li < nlens; ++li) {
			const size_t n = lens[li];
			for (size_t trial = 0; trial < trials; ++trial) {
				rigor_fill(src, n, 1);
				size_t at = n ? rigor_randn(n + 1) : 0;
				size_t fl = n - at ? rigor_randn(n - at + 1) : 0;
				size_t expect_sz = ref_rmat(expect, src, n, at, fl);
				memcpy(got, src, n + 1);
				char *p = jstr_rmat_len_p((char *)got, n, at, fl);
				RCHECK_EQ(p - (char *)got, (long)expect_sz);
				RCHECK(*p == '\0');
				RCHECK_MEM(got, expect, expect_sz + 1);
			}
		}
	}

	return 0;
}
