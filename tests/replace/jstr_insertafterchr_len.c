/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

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
ref_first(const char *in, size_t sz, unsigned char c)
{
	for (size_t i = 0; i < sz; ++i)
		if ((unsigned char)in[i] == c)
			return i;
	return sz;
}

static void
ref_insertafterchr(char *out, const char *in, size_t sz, unsigned char c, const char *src, size_t src_len)
{
	const size_t p = ref_first(in, sz, c);
	if (p == sz) {
		memcpy(out, in, sz + 1);
		return;
	}
	memcpy(out, in, p + 1);
	memcpy(out + p + 1, src, src_len);
	memcpy(out + p + 1 + src_len, in + p + 1, sz - p - 1);
	out[sz + src_len] = '\0';
}

static void
check_inv(const char *s, size_t sz, size_t cap)
{
	RCHECK(cap == 0 || cap > sz);
	if (cap != 0)
		RCHECK_EQ(s[sz], 0);
}

static void
one(const char *in, size_t sz, int c, const char *src, size_t src_len)
{
	char *s = xnew(sz + 1);
	char exp[512];
	const size_t old_sz = sz;
	const int matched = ref_first(in, old_sz, (unsigned char)c) != old_sz;
	size_t cap = sz + 1;
	memcpy(s, in, sz + 1);
	RCHECK_EQ(jstr_insertafterchr_len(&s, &sz, &cap, c, src, src_len), JSTR_RET_SUCC);
	ref_insertafterchr(exp, in, old_sz, (unsigned char)c, src, src_len);
	RCHECK_EQ(sz, old_sz + (matched ? src_len : 0));
	check_inv(s, sz, cap);
	RCHECK_MEM(s, exp, sz + 1);
	free(s);
}

int
main(void)
{
	static const size_t lens[] = { 0, 1, 2, 3, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
	char in[160];
	char src[128];
	size_t k;

	/* Explicit positions. */
	{
		const char *v = "abcabc";
		one(v, 6, 'a', "XY", 2);
		one(v, 6, 'b', "XY", 2);
		one(v, 6, 'c', "XY", 2);
		one(v, 6, 'z', "XY", 2);
		one("", 0, 'a', "XY", 2);
		one("a", 1, 'a', "XY", 2);
		one("a", 1, 'a', "", 0);
		one("abc", 3, 'b', "", 0);
		one("abc", 3, 'z', "", 0);
	}

	/* Embedded NUL is a valid byte when a length is supplied. */
	{
		const char v[] = { 'a', '\0', 'b', '\0' };
		one(v, sizeof(v) - 1, '\0', "X", 1);
	}

	/* First occurrence only: only one insertion even with many matches. */
	{
		char exp[] = "aXaaaa";
		char *s = xnew(7);
		size_t sz = 5;
		size_t cap = 7;
		memcpy(s, "aaaaa", 6);
		RCHECK_EQ(jstr_insertafterchr_len(&s, &sz, &cap, 'a', "X", 1), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 6);
		RCHECK_MEM(s, exp, 7);
		check_inv(s, sz, cap);
		free(s);
	}

	/* Tight capacity forces a reallocation for the appended block. */
	{
		char *s = xnew(4);
		size_t sz = 3;
		size_t cap = 4;
		memcpy(s, "abc", 4);
		RCHECK_EQ(jstr_insertafterchr_len(&s, &sz, &cap, 'c', "123456", 6), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 9);
		RCHECK_MEM(s, "abc123456", 10);
		check_inv(s, sz, cap);
		free(s);
	}

	/* All boundary lengths, with and without a match. */
	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (i % 5 == 3) ? 'b' : 'a';
		in[n] = '\0';
		for (size_t i = 0; i < 40; ++i)
			src[i] = (char)('A' + (int)(i % 26));
		one(in, n, 'a', src, 3);
		one(in, n, 'b', src, 3);
		one(in, n, 'z', src, 3);
		one(in, n, 'a', src, 40);
	}
	return 0;
}
