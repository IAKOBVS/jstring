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
find_at(const char *in, size_t sz, const char *ne, size_t ne_len, size_t from)
{
	if (ne_len == 0 || from >= sz || sz - from < ne_len)
		return sz;
	for (size_t i = from; i + ne_len <= sz; ++i)
		if (memcmp(in + i, ne, ne_len) == 0)
			return i;
	return sz;
}

static void
check_inv(const char *s, size_t sz, size_t cap)
{
	RCHECK(cap == 0 || cap > sz);
	if (cap != 0)
		RCHECK_EQ(s[sz], 0);
}

static void
one(const char *in, size_t sz, size_t start, const char *find, size_t find_len, const char *rplc, size_t rplc_len)
{
	const size_t capwant = sz + rplc_len + 64;
	char *s = xnew(capwant);
	char exp[2048];
	size_t old = sz;
	size_t cap = capwant;
	size_t p = find_at(in, sz, find, find_len, start);
	size_t want = sz;
	int ret;

	memcpy(s, in, sz);
	s[sz] = '\0';
	if (start >= sz || find_len == 0 || p == sz) {
		memcpy(exp, in, sz + 1);
	} else {
		size_t o = 0;
		memcpy(exp + o, in, p);
		o += p;
		memcpy(exp + o, rplc, rplc_len);
		o += rplc_len;
		memcpy(exp + o, in + p + find_len, sz - p - find_len);
		o += sz - p - find_len;
		exp[o] = '\0';
		want = o;
	}
	ret = jstr_rplc_len_from(&s, &old, &cap, start, find, find_len, rplc, rplc_len);
	RCHECK_EQ(ret, (start < sz && find_len != 0 && p != sz) ? 1 : 0);
	RCHECK_EQ(old, want);
	check_inv(s, old, cap);
	RCHECK_MEM(s, exp, old + 1);
	free(s);
}

int
main(void)
{
	static const size_t lens[] = { 0, 1, 2, 3, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
	static const size_t starts[] = { 0, 1, 2, 3, 7, 100 };
	static const size_t rls[] = { 0, 1, 2, 5 };
	char in[160];
	char rplc[160];
	size_t k;

	/* start at / beyond the end, empty find, absent find. */
	one("abcabc", 6, 6, "ab", 2, "X", 1);
	one("abcabc", 6, 7, "ab", 2, "X", 1);
	one("abcabc", 6, 100, "ab", 2, "X", 1);
	one("abcabc", 6, 0, "", 0, "X", 1);
	one("abcabc", 6, 2, "zz", 2, "X", 1);
	one("", 0, 0, "a", 1, "X", 1);

	/* First match at/after start, leaving the prefix untouched. */
	one("abcabc", 6, 0, "ab", 2, "X", 1);
	one("abcabc", 6, 1, "ab", 2, "X", 1);
	one("abcabc", 6, 2, "ab", 2, "X", 1);
	one("abcabc", 6, 3, "ab", 2, "X", 1);
	one("abcabc", 6, 4, "bc", 2, "X", 1);
	one("abcabc", 6, 5, "bc", 2, "X", 1);

	/* Equal / shorter / longer / empty replacement. */
	one("abcabc", 6, 2, "ca", 2, "XY", 2);
	one("abcabc", 6, 2, "ca", 2, "Z", 1);
	one("abcabc", 6, 2, "ca", 2, "WXYZ", 4);
	one("abcabc", 6, 2, "ca", 2, "", 0);

	/* Growth from a truncated capacity. */
	{
		char *s = xnew(7);
		size_t sz = 6;
		size_t cap = 0;
		char big[100];
		memcpy(s, "abcabc", 7);
		for (size_t i = 0; i < sizeof(big); ++i)
			big[i] = (char)('A' + (int)(i % 26));
		RCHECK_EQ(jstr_rplc_len_from(&s, &sz, &cap, 3, "bc", 2, big, sizeof(big)), 1);
		RCHECK_EQ(sz, 4 + sizeof(big));
		check_inv(s, sz, cap);
		RCHECK_MEM(s, "abca", 4);
		RCHECK_MEM(s + 4, big, sizeof(big));
		free(s);
	}

	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (char)('a' + (int)(i % 3));
		in[n] = '\0';
		for (size_t si = 0; si < sizeof(starts) / sizeof(starts[0]); ++si) {
			const size_t start = starts[si] <= n + 1 ? starts[si] : n + 1;
			for (size_t ri = 0; ri < sizeof(rls) / sizeof(rls[0]); ++ri) {
				const size_t rl = rls[ri];
				for (size_t i = 0; i < rl; ++i)
					rplc[i] = (char)('A' + (int)(i % 26));
				rplc[rl] = '\0';
				one(in, n, start, "a", 1, rplc, rl);
				one(in, n, start, "ab", 2, rplc, rl);
				one(in, n, start, "zz", 2, rplc, rl);
			}
		}
	}
	return 0;
}
