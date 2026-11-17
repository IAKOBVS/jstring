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
ref_rplcn(char *out, const char *in, size_t sz, size_t start, const char *ne, size_t ne_len, const char *rplc, size_t rplc_len, size_t n, size_t *count)
{
	size_t o = 0;
	size_t c = 0;
	size_t i = start;
	memcpy(out, in, start);
	o = start;
	while (i + ne_len <= sz && c < n && ne_len > 0) {
		if (memcmp(in + i, ne, ne_len) == 0) {
			memcpy(out + o, rplc, rplc_len);
			o += rplc_len;
			i += ne_len;
			++c;
		} else {
			out[o++] = in[i++];
		}
	}
	memcpy(out + o, in + i, sz - i);
	o += sz - i;
	out[o] = '\0';
	*count = c;
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
one(const char *in, size_t sz, const char *find, size_t find_len, const char *rplc, size_t rplc_len, size_t n)
{
	const size_t capwant = sz + (sz + 1) * rplc_len + 64;
	char *s = xnew(capwant);
	char exp[8192];
	jstr_twoway_ty t;
	size_t old = sz;
	size_t cap = capwant;
	size_t count = 0;
	size_t want = ref_rplcn(exp, in, sz, 0, find, find_len, rplc, rplc_len, n, &count);
	size_t ret;

	memcpy(s, in, sz);
	s[sz] = '\0';
	jstr_memmem_comp(&t, find, find_len);
	ret = jstr_rplcn_len_exec(&t, &s, &old, &cap, find, find_len, rplc, rplc_len, n);
	RCHECK_EQ(ret, count);
	RCHECK_EQ(old, want);
	check_inv(s, old, cap);
	RCHECK_MEM(s, exp, old + 1);
	free(s);
}

int
main(void)
{
	static const size_t lens[] = { 0, 1, 2, 3, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
	static const size_t ns[] = { 0, 1, 2, 3, 100, (size_t)-1 };
	static const size_t rls[] = { 0, 1, 2, 5 };
	char in[160];
	char rplc[160];
	size_t k;

	one("abcabc", 6, "a", 1, "X", 1, 0);
	one("abcabc", 6, "", 0, "X", 1, (size_t)-1);
	one("abcabc", 6, "zz", 2, "X", 1, (size_t)-1);
	one("", 0, "a", 1, "X", 1, (size_t)-1);
	one("abcabc", 6, "a", 1, "X", 1, 1);
	one("abcabc", 6, "a", 1, "X", 1, 2);
	one("abcabc", 6, "a", 1, "X", 1, 100);
	one("abcabc", 6, "ab", 2, "X", 1, 1);
	one("abcabc", 6, "ab", 2, "X", 1, 100);
	one("abcabc", 6, "bc", 2, "XY", 2, 100);
	one("abcabc", 6, "bc", 2, "Z", 1, 100);
	one("abcabc", 6, "bc", 2, "WXYZ", 4, 100);
	one("abcabc", 6, "bc", 2, "", 0, 100);
	one("aaaa", 4, "aa", 2, "X", 1, 1);
	one("aaaa", 4, "aa", 2, "X", 1, 2);
	one("aaaa", 4, "aa", 2, "X", 1, (size_t)-1);
	one("aaa", 3, "aa", 2, "X", 1, (size_t)-1);

	/* Long needle (> threshold) exercises the twoway match path. */
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
		one(hay, o, ne, 40, "X", 1, 1);
		one(hay, o, ne, 40, "X", 1, 2);
		one(hay, o, ne, 40, "X", 1, (size_t)-1);
		one(hay, o, ne, 40, "LONGER", 6, 2);
		one(hay, o, ne, 40, "", 0, (size_t)-1);
	}

	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (char)('a' + (int)(i % 3));
		in[n] = '\0';
		for (size_t ni = 0; ni < sizeof(ns) / sizeof(ns[0]); ++ni) {
			for (size_t ri = 0; ri < sizeof(rls) / sizeof(rls[0]); ++ri) {
				const size_t rl = rls[ri];
				for (size_t i = 0; i < rl; ++i)
					rplc[i] = (char)('A' + (int)(i % 26));
				rplc[rl] = '\0';
				one(in, n, "a", 1, rplc, rl, ns[ni]);
				one(in, n, "ab", 2, rplc, rl, ns[ni]);
				one(in, n, "abc", 3, rplc, rl, ns[ni]);
				one(in, n, "zz", 2, rplc, rl, ns[ni]);
			}
		}
	}
	return 0;
}
