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
	char exp[4096];
	size_t old = sz;
	size_t cap = capwant;
	size_t count = 0;
	size_t want = ref_rplcn(exp, in, sz, 0, find, find_len, rplc, rplc_len, n, &count);
	size_t ret;

	memcpy(s, in, sz);
	s[sz] = '\0';
	ret = jstr_rplcn_len(&s, &old, &cap, find, find_len, rplc, rplc_len, n);
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

	/* n == 0, empty find, absent find, empty input. */
	one("abcabc", 6, "a", 1, "X", 1, 0);
	one("abcabc", 6, "", 0, "X", 1, (size_t)-1);
	one("abcabc", 6, "zz", 2, "X", 1, (size_t)-1);
	one("", 0, "a", 1, "X", 1, (size_t)-1);

	/* Exactly n occurrences, including more than are present. */
	one("abcabc", 6, "a", 1, "X", 1, 1);
	one("abcabc", 6, "a", 1, "X", 1, 2);
	one("abcabc", 6, "a", 1, "X", 1, 3);
	one("abcabc", 6, "a", 1, "X", 1, 100);
	one("abcabc", 6, "ab", 2, "X", 1, 1);
	one("abcabc", 6, "ab", 2, "X", 1, 100);

	/* Equal / shorter / longer / empty replacement. */
	one("abcabc", 6, "bc", 2, "XY", 2, 100);
	one("abcabc", 6, "bc", 2, "Z", 1, 100);
	one("abcabc", 6, "bc", 2, "WXYZ", 4, 100);
	one("abcabc", 6, "bc", 2, "", 0, 100);
	one("abcabc", 6, "bc", 2, "", 0, 1);

	/* Matches do not overlap: advancing by find_len. */
	one("aaaa", 4, "aa", 2, "X", 1, 1);
	one("aaaa", 4, "aa", 2, "X", 1, 2);
	one("aaaa", 4, "aa", 2, "X", 1, (size_t)-1);
	one("aaa", 3, "aa", 2, "X", 1, (size_t)-1);

	/* Growth from a truncated capacity. */
	{
		char *s = xnew(7);
		size_t sz = 6;
		size_t cap = 0;
		char big[100];
		memcpy(s, "abcabc", 7);
		for (size_t i = 0; i < sizeof(big); ++i)
			big[i] = (char)('A' + (int)(i % 26));
		RCHECK_EQ(jstr_rplcn_len(&s, &sz, &cap, "bc", 2, big, sizeof(big), (size_t)-1), 2);
		RCHECK_EQ(sz, 1 + 2 * sizeof(big) + 1);
		check_inv(s, sz, cap);
		RCHECK_EQ(s[0], 'a');
		RCHECK_MEM(s + 1, big, sizeof(big));
		RCHECK_EQ(s[1 + sizeof(big)], 'a');
		RCHECK_MEM(s + 1 + sizeof(big) + 1, big, sizeof(big));
		free(s);
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
