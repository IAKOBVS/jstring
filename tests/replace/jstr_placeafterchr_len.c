/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Regression coverage: jstr_placeafterchr_len() must place SRC at
 * (match_index + 1), i.e. after the matched character. Previously it used
 * (match_index - 1), overwriting before the match and wrapping to SIZE_MAX
 * (OOB write) when the match was at index 0. */

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
one(const char *in, size_t old_sz, int c, const char *src, size_t src_len)
{
	const unsigned char uc = (unsigned char)c;
	size_t p = old_sz;
	size_t at;
	char *s;
	char exp[2048];
	size_t sz = old_sz;
	size_t cap;
	size_t want;

	for (size_t i = 0; i < old_sz; ++i) {
		if ((unsigned char)in[i] == uc) {
			p = i;
			break;
		}
	}
	at = p == old_sz ? old_sz : p + 1;
	cap = (at + src_len > old_sz ? at + src_len : old_sz) + 64;
	s = xnew(cap);
	memcpy(s, in, old_sz);
	s[old_sz] = '\0';
	want = p == old_sz ? old_sz : ref_place(exp, in, old_sz, at, src, src_len);
	if (p == old_sz)
		memcpy(exp, in, old_sz + 1);
	RCHECK_EQ(jstr_placeafterchr_len(&s, &sz, &cap, c, src, src_len), JSTR_RET_SUCC);
	RCHECK_EQ(sz, want);
	check_inv(s, sz, cap);
	RCHECK_MEM(s, exp, sz + 1);
	free(s);
}

int
main(void)
{
	static const size_t lens[] = { 2, 3, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
	char in[160];
	char src[160];
	size_t k;

	/* No match / empty input: untouched. */
	one("", 0, 'a', "XY", 2);
	one("abc", 3, 'z', "XY", 2);
	one("abc", 3, 'z', "", 0);

	/* Placement after the matched character. */
	/* "a,b", match ',' at 1 -> at 2 -> "a,XYb". */
	one("a,b", 3, ',', "XY", 2);
	/* "zabc", match 'b' at 2 -> at 3 -> "zabXYc". */
	one("zabc", 4, 'b', "XY", 2);
	/* "abcd", match 'd' at 3 -> at 4 -> "abcdXY". */
	one("abcd", 4, 'd', "XY", 2);
	/* Match at the end, source longer than the tail: the string grows. */
	one("abcd", 4, 'd', "XYZ", 3);
	/* Match at index 0 -> at 1 (previously wrapped to SIZE_MAX). */
	one("abcd", 4, 'a', "XY", 2);
	/* Empty source leaves everything unchanged. */
	one("zabc", 4, 'a', "", 0);

	/* Only the first match is used: "zaaa" -> at 1 -> "zXYaaa". */
	one("zaaa", 4, 'a', "XY", 2);

	/* Growth from a truncated capacity (sums avoid 24*2^k boundaries). */
	{
		char *s = xnew(4);
		size_t sz = 3;
		size_t cap = 0;
		memcpy(s, "zab", 4);
		/* match 'a' at 1 -> at 2 -> "zaXYZ". */
		RCHECK_EQ(jstr_placeafterchr_len(&s, &sz, &cap, 'a', "XYZ", 3), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 5);
		check_inv(s, sz, cap);
		RCHECK_MEM(s, "zaXYZ", 6);
		free(s);
	}
	{
		char *s = xnew(5);
		size_t sz = 4;
		size_t cap = 0;
		memcpy(s, "zzza", 5);
		/* match 'a' at 3 -> at 4; 4 + 10 > old size 4, so it grows. */
		RCHECK_EQ(jstr_placeafterchr_len(&s, &sz, &cap, 'a', "1234567890", 10), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 14);
		check_inv(s, sz, cap);
		RCHECK_MEM(s, "zzza1234567890", 15);
		free(s);
	}

	/* Every boundary length >= 2: first match at the last byte (index >= 1). */
	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = 'b';
		in[n - 1] = 'a';
		in[n] = '\0';
		for (size_t i = 0; i < 40; ++i)
			src[i] = (char)('A' + (int)(i % 26));
		one(in, n, 'a', src, 3);
		one(in, n, 'a', src, 40);
		one(in, n, 'z', src, 3);
	}
	return 0;
}
