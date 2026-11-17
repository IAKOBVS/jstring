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
ref_first(const char *in, size_t sz, const char *ne, size_t ne_len)
{
	if (ne_len == 0 || sz < ne_len)
		return sz;
	for (size_t i = 0; i + ne_len <= sz; ++i)
		if (memcmp(in + i, ne, ne_len) == 0)
			return i;
	return sz;
}

static void
ref_insertafter(char *out, const char *in, size_t sz, const char *ne, size_t ne_len, const char *src, size_t src_len)
{
	const size_t p = ref_first(in, sz, ne, ne_len);
	if (p == sz) {
		memcpy(out, in, sz + 1);
		return;
	}
	memcpy(out, in, p + ne_len);
	memcpy(out + p + ne_len, src, src_len);
	memcpy(out + p + ne_len + src_len, in + p + ne_len, sz - p - ne_len);
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
one(const char *in, size_t sz, const char *ne, size_t ne_len, const char *src, size_t src_len)
{
	char *s = xnew(sz + 1);
	char exp[2048];
	const size_t old_sz = sz;
	const int matched = ref_first(in, old_sz, ne, ne_len) != old_sz;
	size_t cap = sz + 1;
	memcpy(s, in, sz + 1);
	RCHECK_EQ(jstr_insertafter_len(&s, &sz, &cap, ne, ne_len, src, src_len), JSTR_RET_SUCC);
	ref_insertafter(exp, in, old_sz, ne, ne_len, src, src_len);
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

	/* Explicit start / middle / end / absent needles. */
	one("abcabc", 6, "ab", 2, "XY", 2);
	one("abcabc", 6, "bc", 2, "XY", 2);
	one("abcabc", 6, "ca", 2, "XY", 2);
	one("abcabc", 6, "zz", 2, "XY", 2);
	one("", 0, "ab", 2, "XY", 2);
	one("abc", 3, "abc", 3, "XY", 2);
	one("abc", 3, "abcd", 4, "XY", 2);
	one("abcabc", 6, "", 0, "XY", 2);
	one("abc", 3, "abc", 3, "", 0);
	one("abc", 3, "zz", 2, "", 0);

	/* find_len == 1 uses the character shortcut. */
	one("banana", 6, "a", 1, "-", 1);
	one("banana", 6, "z", 1, "-", 1);

	/* Overlapping candidate occurrences: only the first is used. */
	one("aaaa", 4, "aa", 2, "X", 1);
	one("aaa", 3, "aa", 2, "X", 1);

	/* Tight capacity forces reallocation for the inserted block. */
	{
		char *s = xnew(4);
		size_t sz = 3;
		size_t cap = 4;
		memcpy(s, "abc", 4);
		RCHECK_EQ(jstr_insertafter_len(&s, &sz, &cap, "bc", 2, "123456", 6), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 9);
		RCHECK_MEM(s, "abc123456", 10);
		check_inv(s, sz, cap);
		free(s);
	}

	/* All boundary lengths; needles of several sizes, present and absent. */
	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (char)('a' + (int)(i % 3));
		in[n] = '\0';
		for (size_t i = 0; i < 40; ++i)
			src[i] = (char)('A' + (int)(i % 26));
		one(in, n, "a", 1, src, 3);
		one(in, n, "ab", 2, src, 3);
		one(in, n, "abc", 3, src, 3);
		one(in, n, "zz", 2, src, 3);
		one(in, n, "ab", 2, src, 40);
	}
	return 0;
}
