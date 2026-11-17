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

/* Remove the first occurrence of FIND (binary-safe); 1 if removed, else 0. */
static size_t
ref_rm_len(unsigned char *dst, size_t *out_sz, const unsigned char *src, size_t n, const unsigned char *find, size_t fl)
{
	size_t w = 0, pos = 0;
	int found = 0;
	if (fl != 0 && fl <= n) {
		for (size_t i = 0; i + fl <= n; ++i) {
			if (memcmp(src + i, find, fl) == 0) {
				pos = i;
				found = 1;
				break;
			}
		}
	}
	if (!found) {
		memcpy(dst, src, n);
		dst[n] = 0;
		*out_sz = n;
		return 0;
	}
	for (size_t i = 0; i < pos; ++i)
		dst[w++] = src[i];
	for (size_t i = pos + fl; i < n; ++i)
		dst[w++] = src[i];
	dst[w] = 0;
	*out_sz = w;
	return 1;
}

static void
check(const char *input, const char *find, size_t fl, const char *expect, size_t expect_ret)
{
	char b[512];
	size_t len = strlen(input);
	size_t sz = len;
	RCHECK(len + 1 <= sizeof b);
	memcpy(b, input, len + 1);
	size_t r = jstr_rm_len(b, &sz, find, fl);
	RCHECK_EQ(r, expect_ret);
	RCHECK_EQ(sz, strlen(expect));
	RCHECK_MEM(b, expect, sz + 1);
	RCHECK(b[sz] == '\0');
}

int
main(void)
{
	/* No-op cases. */
	check("", "a", 1, "", 0);
	check("", "", 0, "", 0);
	check("abc", "", 0, "abc", 0);
	check("abc", "z", 1, "abc", 0);
	check("abc", "abcd", 4, "abc", 0);
	check("abc", "abcabc", 6, "abc", 0);

	/* Single occurrence, first of several, whole string. */
	check("abc", "a", 1, "bc", 1);
	check("abc", "b", 1, "ac", 1);
	check("abc", "c", 1, "ab", 1);
	check("abc", "abc", 3, "", 1);
	check("abcabc", "abc", 3, "abc", 1);
	check("abcabc", "bc", 2, "aabc", 1);
	check("abcabc", "ab", 2, "cabc", 1);
	check("aaaa", "aa", 2, "aa", 1);
	check("aaa", "aa", 2, "a", 1);
	check("banana", "ana", 3, "bna", 1);
	check("banana", "na", 2, "bana", 1);
	check("hello world", "o", 1, "hell world", 1);
	check("hello world", "world", 5, "hello ", 1);

	/* Binary data, including embedded NULs. */
	{
		char b[8] = { 'a', 0, 'b', 0, 'c', 0, 0, 0 };
		size_t sz = 5;
		size_t r = jstr_rm_len(b, &sz, "\0b", 2);
		RCHECK_EQ(r, 1);
		RCHECK_EQ(sz, 3);
		RCHECK_MEM(b, "a\0c", 4);
		RCHECK(b[sz] == '\0');

		memcpy(b, "a\0b\0c\0", 7);
		sz = 5;
		r = jstr_rm_len(b, &sz, "\0", 1);
		RCHECK_EQ(r, 1);
		RCHECK_EQ(sz, 4);
		RCHECK_MEM(b, "ab\0c", 5);
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
		rigor_srand(0x10203040u);
		for (size_t li = 0; li < nlens; ++li) {
			const size_t n = lens[li];
			for (size_t trial = 0; trial < trials; ++trial) {
				rigor_fill(src, n, 1);
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
				size_t expect_sz = 0;
				size_t expect_ret = ref_rm_len(expect, &expect_sz, src, n, find, fl);
				memcpy(got, src, n + 1);
				size_t sz = n;
				size_t r = jstr_rm_len((char *)got, &sz, (const char *)find, fl);
				RCHECK_EQ(r, expect_ret);
				RCHECK_EQ(sz, expect_sz);
				RCHECK_MEM(got, expect, sz + 1);
				RCHECK(got[sz] == 0);
			}
		}
	}

	return 0;
}
