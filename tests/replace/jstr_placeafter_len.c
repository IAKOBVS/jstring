/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Regression coverage: jstr_placeafter_len() must place SRC at
 * (match_index + find_len), i.e. after the matched needle. Previously it
 * used (match_index - find_len), overwriting before the match and wrapping
 * to a huge offset (OOB write) whenever match_index < find_len. */

#include <stdlib.h>
#include <string.h>

#include "../rigor.h"
#include "replace.h"

static char *
xnew(size_t cap)
{
	char *p = (char *)malloc(cap ? cap : 1);
	RCHECK(p != NULL);
	memset(p, 0xa5, cap ? cap : 1);
	return p;
}

static size_t
ref_first(const char *in, size_t sz, const char *ne, size_t ne_len)
{
	if (ne_len == 0 || sz < ne_len)
		return sz;
	for (size_t i = 0; i + ne_len <= sz; ++i)
		if (memcmp(in + i, ne, ne_len) == 0)
			return i;
	return sz;
}

static size_t
ref_place(char *out, const char *in, size_t old_sz, size_t at, const char *src, size_t src_len)
{
	size_t new_sz = old_sz;
	if (at + src_len > old_sz) {
		new_sz = at + src_len;
		memcpy(out, in, old_sz);
		if (at > old_sz)
			memset(out + old_sz, 0, at - old_sz);
		out[new_sz] = '\0';
	} else {
		memcpy(out, in, old_sz + 1);
	}
	memcpy(out + at, src, src_len);
	return new_sz;
}

static void
check_inv(const char *s, size_t sz, size_t cap)
{
	RCHECK(cap == 0 || cap > sz);
	if (cap != 0)
		RCHECK_EQ(s[sz], 0);
}

static void
one(const char *in, size_t old_sz, const char *ne, size_t ne_len, const char *src, size_t src_len)
{
	size_t p;
	size_t at;
	char *s;
	char exp[2048];
	size_t sz = old_sz;
	size_t cap;
	size_t want;

	if (ne_len == 0) {
		p = old_sz;
		at = old_sz;
	} else {
		p = ref_first(in, old_sz, ne, ne_len);
		at = p == old_sz ? old_sz : p + ne_len;
	}
	cap = (at + src_len > old_sz ? at + src_len : old_sz) + 64;
	s = xnew(cap);
	memcpy(s, in, old_sz);
	s[old_sz] = '\0';
	if (p == old_sz) {
		memcpy(exp, in, old_sz + 1);
		want = old_sz;
	} else {
		want = ref_place(exp, in, old_sz, at, src, src_len);
	}
	RCHECK_EQ(jstr_placeafter_len(&s, &sz, &cap, ne, ne_len, src, src_len), JSTR_RET_SUCC);
	RCHECK_EQ(sz, want);
	check_inv(s, sz, cap);
	RCHECK_MEM(s, exp, sz + 1);
	free(s);
}

int
main(void)
{
	static const size_t lens[] = { 0, 1, 2, 3, 4, 5, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
	char in[160];
	char src[160];
	size_t k;

	/* Empty find / empty input / absent needle: unchanged. */
	one("abc", 3, "", 0, "XY", 2);
	one("", 0, "ab", 2, "XY", 2);
	one("abc", 3, "zz", 2, "XY", 2);
	one("abc", 3, "zz", 2, "", 0);

	/* Placement after the end of the first needle. */
	/* "zzab", first "ab" at index 2 -> at 4 -> "zzabXY". */
	one("zzab", 4, "ab", 2, "XY", 2);
	/* "zzzcde", first "cd" at index 3 -> at 5 -> "zzzcdeXY". */
	one("zzzcde", 6, "cd", 2, "XY", 2);
	/* "banana", first "na" at index 2 -> at 4 -> "bananaXY". */
	one("banana", 6, "na", 2, "XY", 2);
	/* Match at the start: at == find_len (no underflow). */
	one("abzz", 4, "ab", 2, "XY", 2);
	/* Match at the very end: at == old size, so the string grows. */
	one("zzab", 4, "ab", 2, "XYZ", 3);
	/* Empty source leaves the string untouched. */
	one("zzzcde", 6, "cd", 2, "", 0);

	/* find_len == 1 delegates to jstr_placeafterchr_len. */
	one("zabc", 4, "b", 1, "XY", 2);
	one("zzza", 4, "a", 1, "XY", 2);

	/* Growth: at = 1, at + 40 = 41 > old size 6. */
	one("zzzcde", 6, "cd", 2, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcd", 40);

	/* Growth from a truncated capacity (sums avoid 24*2^k boundaries). */
	{
		char *s = xnew(5);
		size_t sz = 4;
		size_t cap = 0;
		memcpy(s, "zzab", 5);
		/* first "ab" at 2, at 4; 4 + 5 > old size 4, so it grows. */
		RCHECK_EQ(jstr_placeafter_len(&s, &sz, &cap, "ab", 2, "12345", 5), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 9);
		check_inv(s, sz, cap);
		RCHECK_MEM(s, "zzab12345", 10);
		free(s);
	}
	{
		char *s = xnew(7);
		size_t sz = 6;
		size_t cap = 0;
		memcpy(s, "zzzcde", 7);
		/* first "cd" at 3, at 5; 5 + 10 > old size 6, so it grows. */
		RCHECK_EQ(jstr_placeafter_len(&s, &sz, &cap, "cd", 2, "1234567890", 10), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 15);
		check_inv(s, sz, cap);
		RCHECK_MEM(s, "zzzcd1234567890", 16);
		free(s);
	}

	/* Every boundary length >= 4: first needle at index 2, find_len 2. */
	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = 'b';
		if (n >= 4) {
			in[2] = 'a';
			in[3] = 'b';
		}
		in[n] = '\0';
		for (size_t i = 0; i < 40; ++i)
			src[i] = (char)('A' + (int)(i % 26));
		one(in, n, "ab", 2, src, 3);
		one(in, n, "ab", 2, src, 40);
		one(in, n, "zz", 2, src, 3);
	}
	return 0;
}
