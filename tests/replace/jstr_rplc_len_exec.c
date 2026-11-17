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
find_at(const char *in, size_t sz, const char *ne, size_t ne_len)
{
	if (ne_len == 0 || sz < ne_len)
		return sz;
	for (size_t i = 0; i + ne_len <= sz; ++i)
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
one(const char *in, size_t sz, const char *find, size_t find_len, const char *rplc, size_t rplc_len)
{
	const size_t capwant = sz + rplc_len + 64;
	char *s = xnew(capwant);
	char exp[2048];
	jstr_twoway_ty t;
	size_t old = sz;
	size_t cap = capwant;
	size_t p = find_at(in, sz, find, find_len);
	size_t want = sz;
	int ret;

	memcpy(s, in, sz);
	s[sz] = '\0';
	if (find_len == 0 || p == sz) {
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
	jstr_memmem_comp(&t, find, find_len);
	ret = jstr_rplc_len_exec(&t, &s, &old, &cap, find, find_len, rplc, rplc_len);
	RCHECK_EQ(ret, (find_len != 0 && p != sz) ? 1 : 0);
	RCHECK_EQ(old, want);
	check_inv(s, old, cap);
	RCHECK_MEM(s, exp, old + 1);
	free(s);
}

int
main(void)
{
	static const size_t lens[] = { 0, 1, 2, 3, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
	static const size_t rls[] = { 0, 1, 2, 5 };
	char in[256];
	char rplc[160];
	size_t k;

	one("", 0, "ab", 2, "X", 1);
	one("abcabc", 6, "zz", 2, "X", 1);
	one("abcabc", 6, "", 0, "X", 1);
	one("abc", 3, "abcd", 4, "X", 1);
	one("abcabc", 6, "ab", 2, "X", 1);
	one("abcabc", 6, "bc", 2, "X", 1);
	one("abcabc", 6, "ca", 2, "X", 1);
	one("abcabc", 6, "bc", 2, "XY", 2);
	one("abcabc", 6, "bc", 2, "Z", 1);
	one("abcabc", 6, "bc", 2, "WXYZ", 4);
	one("abcabc", 6, "bc", 2, "", 0);

	/* Long needle (> JSTR_TWOWAY_MEMMEM_THRES) uses the twoway path. */
	{
		char ne[41];
		char hay[128];
		size_t o = 0;
		for (size_t i = 0; i < 40; ++i)
			ne[i] = (char)('A' + (int)(i % 26));
		ne[40] = '\0';
		memcpy(hay + o, "pre", 3);
		o += 3;
		memcpy(hay + o, ne, 40);
		o += 40;
		memcpy(hay + o, "mid", 3);
		o += 3;
		memcpy(hay + o, ne, 40);
		o += 40;
		memcpy(hay + o, "post", 4);
		o += 4;
		hay[o] = '\0';
		one(hay, o, ne, 40, "X", 1);
		one(hay, o, ne, 40, "LONGER", 6);
		one(hay, o, ne, 40, "", 0);
	}

	/* One compiled matcher can be reused for several strings. */
	{
		jstr_twoway_ty t;
		jstr_memmem_comp(&t, "bc", 2);
		char *a = xnew(16);
		char *b = xnew(16);
		size_t sza = 6;
		size_t szb = 5;
		size_t capa = 16;
		size_t capb = 16;
		memcpy(a, "abcabc", 7);
		memcpy(b, "zbcbc", 6);
		RCHECK_EQ(jstr_rplc_len_exec(&t, &a, &sza, &capa, "bc", 2, "Q", 1), 1);
		RCHECK_EQ(jstr_rplc_len_exec(&t, &b, &szb, &capb, "bc", 2, "Q", 1), 1);
		RCHECK_MEM(a, "aQabc", 5 + 1);
		RCHECK_MEM(b, "zQbc", 4 + 1);
		free(a);
		free(b);
	}

	/* Growth from a truncated capacity. */
	{
		char *s = xnew(7);
		size_t sz = 6;
		size_t cap = 0;
		char big[100];
		jstr_twoway_ty t;
		memcpy(s, "abcabc", 7);
		for (size_t i = 0; i < sizeof(big); ++i)
			big[i] = (char)('A' + (int)(i % 26));
		jstr_memmem_comp(&t, "bc", 2);
		RCHECK_EQ(jstr_rplc_len_exec(&t, &s, &sz, &cap, "bc", 2, big, sizeof(big)), 1);
		RCHECK_EQ(sz, 1 + sizeof(big) + 3);
		check_inv(s, sz, cap);
		RCHECK_EQ(s[0], 'a');
		RCHECK_MEM(s + 1, big, sizeof(big));
		RCHECK_MEM(s + 1 + sizeof(big), "abc", 3);
		free(s);
	}

	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (char)('a' + (int)(i % 3));
		in[n] = '\0';
		for (size_t ri = 0; ri < sizeof(rls) / sizeof(rls[0]); ++ri) {
			const size_t rl = rls[ri];
			for (size_t i = 0; i < rl; ++i)
				rplc[i] = (char)('A' + (int)(i % 26));
			rplc[rl] = '\0';
			one(in, n, "a", 1, rplc, rl);
			one(in, n, "ab", 2, rplc, rl);
			one(in, n, "zz", 2, rplc, rl);
		}
	}
	return 0;
}
