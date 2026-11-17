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

/* Same semantics as jstr_rmn_len, via a precompiled twoway matcher. */
static size_t
ref_rmn_len(unsigned char *dst, size_t *out_sz, const unsigned char *src, size_t n, const unsigned char *find, size_t fl, size_t limit)
{
	size_t w = 0, i = 0, removed = 0;
	if (fl == 0 || limit == 0) {
		memcpy(dst, src, n);
		dst[n] = 0;
		*out_sz = n;
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
check(const char *input, const char *find, size_t fl, size_t limit, const char *expect, size_t expect_ret)
{
	char b[512];
	size_t len = strlen(input);
	size_t sz = len;
	RCHECK(len + 1 <= sizeof b);
	memcpy(b, input, len + 1);
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, fl);
	size_t r = jstr_rmn_len_exec(&t, b, &sz, find, fl, limit);
	RCHECK_EQ(r, expect_ret);
	RCHECK_EQ(sz, strlen(expect));
	RCHECK_MEM(b, expect, sz + 1);
	RCHECK(b[sz] == '\0');
}

int
main(void)
{
	/* n == 0 and empty/oversized needles are no-ops. */
	check("abcabc", "bc", 2, 0, "abcabc", 0);
	check("abc", "", 0, 1, "abc", 0);
	check("abc", "", 0, ALL, "abc", 0);
	check("abc", "z", 1, ALL, "abc", 0);
	check("abc", "abcd", 4, ALL, "abc", 0);
	check("", "a", 1, ALL, "", 0);

	/* Limits and all-occurrence behavior. */
	check("aaaa", "a", 1, 1, "aaa", 1);
	check("aaaa", "a", 1, 2, "aa", 2);
	check("aaaa", "a", 1, 3, "a", 3);
	check("aaaa", "a", 1, 4, "", 4);
	check("aaaa", "a", 1, 5, "", 4);
	check("aaaa", "a", 1, ALL, "", 4);
	check("aaaa", "aa", 2, 1, "aa", 1);
	check("aaaa", "aa", 2, 2, "", 2);
	check("aaaa", "aa", 2, ALL, "", 2);
	check("aaa", "aa", 2, 1, "a", 1);
	check("aaa", "aa", 2, ALL, "a", 1);
	check("abcabc", "bc", 2, 1, "aabc", 1);
	check("abcabc", "bc", 2, 2, "aa", 2);
	check("abcabc", "bc", 2, ALL, "aa", 2);
	check("abcabc", "ab", 2, 2, "cc", 2);
	check("abcabc", "abc", 3, 1, "abc", 1);
	check("abcabc", "abc", 3, 2, "", 2);
	check("banana", "na", 2, 2, "ba", 2);
	check("banana", "ana", 3, 2, "bna", 1);
	check("mississippi", "ss", 2, ALL, "miiippi", 2);
	check("mississippi", "i", 1, 2, "mssssippi", 2);

	/* Binary data, including embedded NULs. */
	{
		char b[8] = { 'a', 0, 'b', 0, 'c', 0, 0, 0 };
		size_t sz = 5;
		jstr_twoway_ty t;
		jstr_memmem_comp(&t, "\0b", 2);
		size_t r = jstr_rmn_len_exec(&t, b, &sz, "\0b", 2, ALL);
		RCHECK_EQ(r, 1);
		RCHECK_EQ(sz, 3);
		RCHECK_MEM(b, "a\0c", 4);
		RCHECK(b[sz] == '\0');

		memcpy(b, "a\0b\0c\0", 7);
		sz = 5;
		jstr_memmem_comp(&t, "\0", 1);
		r = jstr_rmn_len_exec(&t, b, &sz, "\0", 1, 2);
		RCHECK_EQ(r, 2);
		RCHECK_EQ(sz, 3);
		RCHECK_MEM(b, "abc\0", 4);
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
		rigor_srand(0x20304050u);
		for (size_t li = 0; li < nlens; ++li) {
			const size_t n = lens[li];
			for (size_t trial = 0; trial < trials; ++trial) {
				rigor_fill(src, n, 1);
				if (n && src[0] == 0)
					src[0] = (unsigned char)('A' + (trial % 26));
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
				size_t expect_ret = ref_rmn_len(expect, &expect_sz, src, n, find, fl, limit);
				memcpy(got, src, n + 1);
				size_t sz = n;
				jstr_twoway_ty t;
				jstr_memmem_comp(&t, find, fl);
				size_t r = jstr_rmn_len_exec(&t, (char *)got, &sz, (const char *)find, fl, limit);
				RCHECK_EQ(r, expect_ret);
				RCHECK_EQ(sz, expect_sz);
				RCHECK_MEM(got, expect, sz + 1);
				RCHECK(got[sz] == 0);
			}
		}
	}

	return 0;
}
