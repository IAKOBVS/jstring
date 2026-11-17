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

static size_t
ref_rmallspn(char *dst, const char *src, const char *reject)
{
	size_t n = strlen(src), w = 0, removed = 0;
	for (size_t i = 0; i < n; ++i) {
		if (in_set((unsigned char)src[i], reject)) {
			++removed;
			continue;
		}
		dst[w++] = src[i];
	}
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
	size_t r = jstr_rmallspn(b, &sz, reject);
	RCHECK_EQ(r, expect_ret);
	RCHECK_EQ(sz, strlen(expect));
	RCHECK_MEM(b, expect, sz + 1);
	RCHECK(b[sz] == '\0');
}

int
main(void)
{
	check("", "a", "", 0);
	check("", "", "", 0);
	check("abc", "", "abc", 0);
	check("abc", "z", "abc", 0);

	/* Every run is removed, not only the first. */
	check("aabbbc", "b", "aac", 3);
	check("aXbXXc", "X", "abc", 3);
	check("XXaXXbXX", "X", "ab", 6);
	check("aabbbc", "ab", "c", 5);
	check("aabbbc", "abc", "", 6);
	check("abcabc", "a", "bcbc", 2);
	check("abcabc", "ac", "bb", 4);
	check("aXbXc", "X", "abc", 2);
	check("XaXbX", "X", "ab", 3);
	check("mississippi", "s", "miiippi", 4);
	check("mississippi", "i", "msssspp", 4);
	check("aabbcc", "abc", "", 6);
	check("aabbcc", "b", "aacc", 2);

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
		rigor_srand(0x0ddc0ffeeu);
		for (size_t li = 0; li < nlens; ++li) {
			const size_t n = lens[li];
			for (size_t trial = 0; trial < trials; ++trial) {
				rigor_fill_printable(src, n);
				size_t rl = 1 + rigor_randn(sizeof reject - 1);
				for (size_t k = 0; k < rl; ++k)
					reject[k] = (char)(' ' + rigor_randn(95));
				reject[rl] = 0;
				size_t expect_ret = ref_rmallspn(expect, src, reject);
				memcpy(got, src, n + 1);
				size_t sz = n;
				size_t r = jstr_rmallspn(got, &sz, reject);
				RCHECK_EQ(r, expect_ret);
				RCHECK_EQ(sz, strlen(expect));
				RCHECK_MEM(got, expect, sz + 1);
				RCHECK(got[sz] == 0);
			}
		}
	}

	return 0;
}
