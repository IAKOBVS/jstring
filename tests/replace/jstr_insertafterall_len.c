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
ref_insertafterall(char *out, const char *in, size_t sz, const char *ne, size_t ne_len, const char *src, size_t src_len)
{
	size_t o = 0;
	size_t i = 0;
	if (ne_len == 0) {
		memcpy(out, in, sz + 1);
		return sz;
	}
	while (i < sz) {
		if (i + ne_len <= sz && memcmp(in + i, ne, ne_len) == 0) {
			memcpy(out + o, in + i, ne_len);
			o += ne_len;
			memcpy(out + o, src, src_len);
			o += src_len;
			i += ne_len;
		} else {
			out[o++] = in[i++];
		}
	}
	out[o] = '\0';
	return o;
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
	char exp[4096];
	size_t cap = sz + 1;
	const size_t want = ref_insertafterall(exp, in, sz, ne, ne_len, src, src_len);
	memcpy(s, in, sz + 1);
	RCHECK_EQ(jstr_insertafterall_len(&s, &sz, &cap, ne, ne_len, src, src_len), JSTR_RET_SUCC);
	RCHECK_EQ(sz, want);
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

	/* Explicit single / repeated / absent matches. */
	one("abcabc", 6, "abc", 3, "X", 1);
	one("abcabc", 6, "bc", 2, "XY", 2);
	one("abcabc", 6, "ab", 2, "XY", 2);
	one("abcabc", 6, "zz", 2, "XY", 2);
	one("", 0, "ab", 2, "XY", 2);
	one("abc", 3, "abc", 3, "XY", 2);
	one("abc", 3, "abcd", 4, "XY", 2);
	one("abcabc", 6, "", 0, "XY", 2);
	one("abcabc", 6, "abc", 3, "", 0);
	one("abcabc", 6, "zz", 2, "", 0);

	/* find_len == 1 delegates to the character implementation. */
	one("banana", 6, "a", 1, "-", 1);
	one("banana", 6, "a", 1, "", 0);

	/* Non-overlapping matches: advancing by find_len, not one byte. */
	one("aaaa", 4, "aa", 2, "X", 1);
	one("aaa", 3, "aa", 2, "X", 1);
	one("abcabc", 6, "abc", 3, "X", 1);
	one("ababab", 6, "aba", 3, "X", 1);

	/* Tight capacity forces exact growth. */
	{
		char *s = xnew(4);
		size_t sz = 3;
		size_t cap = 4;
		memcpy(s, "abc", 4);
		RCHECK_EQ(jstr_insertafterall_len(&s, &sz, &cap, "b", 1, "123456", 6), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 9);
		RCHECK_MEM(s, "ab123456c", 10);
		check_inv(s, sz, cap);
		free(s);
	}

	/* All boundary lengths; several needle and source sizes. */
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
