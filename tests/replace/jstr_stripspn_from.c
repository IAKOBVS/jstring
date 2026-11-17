/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* stripspn removes every character belonging to REJECT; the _from variant
 * preserves the prefix before start_idx and strips all reject chars after it. */

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
ref_stripspn_from(char *dst, const char *src, size_t start, const char *reject)
{
	size_t n = strlen(src), w = 0, removed = 0;
	for (size_t i = 0; i < start && i < n; ++i)
		dst[w++] = src[i];
	for (size_t i = start; i < n; ++i) {
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
check(const char *input, size_t start, const char *reject, const char *expect, size_t expect_ret)
{
	char b[512];
	size_t len = strlen(input);
	size_t sz = len;
	RCHECK(len + 1 <= sizeof b);
	memcpy(b, input, len + 1);
	size_t r = jstr_stripspn_from(b, &sz, start, reject);
	RCHECK_EQ(r, expect_ret);
	RCHECK_EQ(sz, strlen(expect));
	RCHECK_MEM(b, expect, sz + 1);
	RCHECK(b[sz] == '\0');
}

int
main(void)
{
	check("", 0, "a", "", 0);
	check("abc", 0, "", "abc", 0);
	check("abc", 0, "z", "abc", 0);
	check("abc", 3, "a", "abc", 0);
	check("aabbbc", 0, "b", "aac", 3);
	check("abc", 0, "abc", "", 3);

	/* Prefix before START is preserved. */
	check("aXbXc", 1, "X", "abc", 2);
	check("aXbXc", 2, "X", "aXbc", 1);
	check("aXbXc", 4, "X", "aXbXc", 0);
	check("aXbXc", 5, "X", "aXbXc", 0);
	check("XXaXX", 2, "X", "XXa", 2);
	check("XaXbX", 1, "X", "Xab", 2);
	check("XaXbX", 4, "X", "XaXb", 1);
	check("aabbbc", 3, "b", "aabc", 2);
	check("aabbbc", 3, "a", "aabbbc", 0);
	check("aXXbXXc", 3, "X", "aXXbc", 2);
	check("aXXbXXc", 2, "X", "aXbc", 3);
	check("aXXbXXc", 6, "X", "aXXbXXc", 0);
	check("aabbcc", 2, "abc", "aa", 4);

	/* Must agree with rmallspn_from for every input. */
	{
		char a[1026], b[1026], reject[8];
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
		char expect[1026];
		rigor_srand(0x57a1c0dfu);
		for (size_t li = 0; li < nlens; ++li) {
			const size_t n = lens[li];
			for (size_t trial = 0; trial < trials; ++trial) {
				rigor_fill_printable(a, n);
				size_t rl = 1 + rigor_randn(sizeof reject - 1);
				for (size_t k = 0; k < rl; ++k)
					reject[k] = (char)(' ' + rigor_randn(95));
				reject[rl] = 0;
				size_t start = n ? rigor_randn(n + 1) : 0;
				size_t ref_ret = ref_stripspn_from(expect, a, start, reject);
				memcpy(b, a, n + 1);
				size_t asz = n, bsz = n;
				size_t ar = jstr_stripspn_from(a, &asz, start, reject);
				size_t br = jstr_rmallspn_from(b, &bsz, start, reject);
				RCHECK_EQ(ar, br);
				RCHECK_EQ(asz, bsz);
				RCHECK_MEM(a, b, bsz + 1);
				RCHECK_EQ(ar, ref_ret);
				RCHECK_EQ(bsz, strlen(expect));
				RCHECK_MEM(b, expect, bsz + 1);
				RCHECK(b[bsz] == 0);
			}
		}
	}

	return 0;
}
