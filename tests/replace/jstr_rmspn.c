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

static int
in_set(int ch, const char *reject)
{
	return ch != '\0' && strchr(reject, ch) != NULL;
}

/* Remove the first maximal run of reject-set characters (only one run). */
static size_t
ref_rmspn(char *dst, const char *src, const char *reject)
{
	size_t n = strlen(src), w = 0, i = 0, removed = 0;
	while (i < n && !in_set((unsigned char)src[i], reject))
		dst[w++] = src[i++];
	while (i < n && in_set((unsigned char)src[i], reject)) {
		++i;
		++removed;
	}
	while (i < n)
		dst[w++] = src[i++];
	dst[w] = 0;
	return removed;
}

static void
check(const char *input, const char *reject, const char *expect, size_t expect_ret)
{
	char b[512];
	size_t len = strlen(input);
	size_t sz = len;
	RCHECK(len + 1 <= sizeof b);
	memcpy(b, input, len + 1);
	size_t r = jstr_rmspn(b, &sz, reject);
	RCHECK_EQ(r, expect_ret);
	RCHECK_EQ(sz, strlen(expect));
	RCHECK_MEM(b, expect, sz + 1);
	RCHECK(b[sz] == '\0');
}

int
main(void)
{
	/* No match / empty set / empty input. */
	check("", "a", "", 0);
	check("", "", "", 0);
	check("abc", "", "abc", 0);
	check("abc", "z", "abc", 0);
	check("abc", "xyz", "abc", 0);

	/* Single run in the middle, at the start and at the end. */
	check("aabbbc", "b", "aac", 3);
	check("aabbbc", "ab", "c", 5);
	check("aabbbc", "abc", "", 6);
	check("abcabc", "b", "acabc", 1);
	check("abcabc", "c", "ababc", 1);
	check("abcabc", "a", "bcabc", 1);
	check("abcabc", "ac", "bcabc", 1);
	check("aXXbXc", "X", "abXc", 2);
	check("XXaXXbXX", "X", "aXXbXX", 2);
	check("XXaXXbXX", "Xa", "bXX", 5);
	check("xxYxx", "xy", "Yxx", 2);

	/* Only a single run is removed, even if the set appears again later. */
	check("aXbXc", "X", "abXc", 1);
	check("XaXbX", "X", "aXbX", 1);
	check("aXXbXXc", "X", "abXXc", 2);

	/* Repeated characters in the set must not change behavior. */
	check("aabbbc", "bbbb", "aac", 3);
	check("aabbbc", "ba", "c", 5);

	/* Differential sweep. */
	{
		char src[1026], expect[1026], got[1026], reject[8];
		const size_t *lens;
		size_t nlens;
#if defined(JSTR_TEST_SLOW) && JSTR_TEST_SLOW
		lens = rigor_lens;
		nlens = RIGOR_NLENS;
		const size_t trials = 48;
#else
		lens = fast_lens;
		nlens = sizeof fast_lens / sizeof fast_lens[0];
		const size_t trials = 8;
#endif
		rigor_srand(0x0badf00du);
		for (size_t li = 0; li < nlens; ++li) {
			const size_t n = lens[li];
			for (size_t trial = 0; trial < trials; ++trial) {
				rigor_fill_printable(src, n);
				size_t rl = 1 + rigor_randn(sizeof reject - 1);
				for (size_t k = 0; k < rl; ++k)
					reject[k] = (char)(' ' + rigor_randn(95));
				reject[rl] = 0;
				size_t expect_ret = ref_rmspn(expect, src, reject);
				memcpy(got, src, n + 1);
				size_t sz = n;
				size_t r = jstr_rmspn(got, &sz, reject);
				RCHECK_EQ(r, expect_ret);
				RCHECK_EQ(sz, strlen(expect));
				RCHECK_MEM(got, expect, sz + 1);
				RCHECK(got[sz] == 0);
			}
		}
	}

	return 0;
}
