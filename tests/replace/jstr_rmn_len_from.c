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

/* Remove up to LIMIT non-overlapping FIND occurrences at/after START. */
static size_t
ref_rmn_len_from(unsigned char *dst, size_t *out_sz, const unsigned char *src, size_t n, size_t start, const unsigned char *find, size_t fl, size_t limit)
{
	size_t w = 0, i = start, removed = 0;
	for (size_t k = 0; k < start && k < n; ++k)
		dst[w++] = src[k];
	if (fl == 0 || limit == 0) {
		while (i < n)
			dst[w++] = src[i++];
		dst[w] = 0;
		*out_sz = w;
		return 0;
	}
	while (removed < limit && i + fl <= n) {
		size_t pos = 0;
		int found = 0;
		for (size_t j = i; j + fl <= n; ++j) {
			if (memcmp(src + j, find, fl) == 0) {
				pos = j;
				found = 1;
				break;
			}
		}
		if (!found)
			break;
		while (i < pos)
			dst[w++] = src[i++];
		i = pos + fl;
		++removed;
	}
	while (i < n)
		dst[w++] = src[i++];
	dst[w] = 0;
	*out_sz = w;
	return removed;
}

static void
check(const char *input, size_t start, const char *find, size_t fl, size_t limit, const char *expect, size_t expect_ret)
{
	char b[512];
	size_t len = strlen(input);
	size_t sz = len;
	RCHECK(len + 1 <= sizeof b);
	memcpy(b, input, len + 1);
	size_t r = jstr_rmn_len_from(b, &sz, start, find, fl, limit);
	RCHECK_EQ(r, expect_ret);
	RCHECK_EQ(sz, strlen(expect));
	RCHECK_MEM(b, expect, sz + 1);
	RCHECK(b[sz] == '\0');
}

int
main(void)
{
	check("abcabc", 0, "bc", 2, 0, "abcabc", 0);
	check("abcabc", 3, "bc", 2, 0, "abcabc", 0);
	check("abc", 0, "", 0, 1, "abc", 0);
	check("abc", 0, "z", 1, ALL, "abc", 0);
	check("abc", 0, "abcd", 4, ALL, "abc", 0);
	check("", 0, "a", 1, ALL, "", 0);

	check("aaaa", 0, "a", 1, 1, "aaa", 1);
	check("aaaa", 0, "a", 1, 2, "aa", 2);
	check("aaaa", 0, "a", 1, ALL, "", 4);
	check("aaaa", 1, "a", 1, 1, "aaa", 1);
	check("aaaa", 1, "a", 1, ALL, "a", 3);
	check("aaaa", 0, "aa", 2, 2, "", 2);
	check("aaa", 0, "aa", 2, ALL, "a", 1);
	check("abcabc", 0, "abc", 3, 2, "", 2);
	check("abcabc", 1, "abc", 3, 2, "abc", 1);
	check("abcabc", 3, "abc", 3, 1, "abc", 1);
	check("abcabc", 4, "abc", 3, ALL, "abcabc", 0);
	check("abcabc", 1, "bc", 2, ALL, "aa", 2);
	check("abcabc", 2, "bc", 2, 1, "abca", 1);
	check("abcabc", 5, "bc", 2, ALL, "abcabc", 0);
	check("abcabc", 6, "abc", 3, ALL, "abcabc", 0);
	check("abcabc", 2, "c", 1, 2, "abab", 2);
	check("aXbXcX", 2, "X", 1, ALL, "aXbc", 2);
	check("aXbXcX", 0, "X", 1, ALL, "abc", 3);
	check("aXbXcX", 5, "X", 1, ALL, "aXbXc", 1);
	check("aXbXcX", 4, "X", 1, ALL, "aXbXc", 1);

	/* Binary data, including embedded NULs (start byte is non-NUL). */
	{
		char b[8] = { 'a', 0, 'b', 0, 'c', 0, 0, 0 };
		size_t sz = 5;
		size_t r = jstr_rmn_len_from(b, &sz, 0, "\0b", 2, ALL);
		RCHECK_EQ(r, 1);
		RCHECK_EQ(sz, 3);
		RCHECK_MEM(b, "a\0c", 4);
		RCHECK(b[sz] == '\0');

		memcpy(b, "\0a\0b\0", 6);
		sz = 5;
		r = jstr_rmn_len_from(b, &sz, 3, "\0", 1, ALL);
		RCHECK_EQ(r, 1);
		RCHECK_EQ(sz, 4);
		RCHECK_MEM(b, "\0a\0b", 5);
	}

	/* Differential sweep. */
	{
		unsigned char src[1026], find[1100], expect[1026], got[1026];
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
		rigor_srand(0x30405060u);
		for (size_t li = 0; li < nlens; ++li) {
			const size_t n = lens[li];
			for (size_t trial = 0; trial < trials; ++trial) {
				rigor_fill(src, n, 1);
				size_t start = n ? rigor_randn(n + 1) : 0;
				if (start < n && src[start] == 0)
					src[start] = (unsigned char)('A' + (trial % 26));
				size_t fl;
				switch (trial % 5) {
				case 0:
					fl = 0;
					break;
				case 1:
					fl = n + 1 + rigor_randn(8);
					if (fl > sizeof find)
						fl = sizeof find;
					rigor_fill(find, fl, 1);
					break;
				case 2:
					fl = n ? 1 + rigor_randn(n < 8 ? n : 8) : 1;
					rigor_fill(find, fl, 1);
					break;
				default: {
					fl = n ? 1 + rigor_randn(n < 16 ? n : 16) : 1;
					size_t pos = n > fl ? rigor_randn(n - fl + 1) : 0;
					if (n >= fl)
						memcpy(find, src + pos, fl);
					else
						rigor_fill(find, fl, 1);
					break;
				}
				}
				size_t limit = (trial % 4 == 0) ? 0 : (trial % 4 == 1)   ? 1
				                                      : (trial % 4 == 2) ? 2
				                                                         : ALL;
				size_t expect_sz = 0;
				size_t expect_ret = ref_rmn_len_from(expect, &expect_sz, src, n, start, find, fl, limit);
				memcpy(got, src, n + 1);
				size_t sz = n;
				size_t r = jstr_rmn_len_from((char *)got, &sz, start, (const char *)find, fl, limit);
				RCHECK_EQ(r, expect_ret);
				RCHECK_EQ(sz, expect_sz);
				RCHECK_MEM(got, expect, sz + 1);
				RCHECK(got[sz] == 0);
			}
		}
	}

	return 0;
}
