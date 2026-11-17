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
ref_rplcat(char *out, const char *in, size_t old_sz, size_t at, const char *rplc, size_t rplc_len, size_t find_len)
{
	size_t o = 0;
	memcpy(out + o, in, at);
	o += at;
	memcpy(out + o, rplc, rplc_len);
	o += rplc_len;
	memcpy(out + o, in + at + find_len, old_sz - at - find_len);
	o += old_sz - at - find_len;
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
one(const char *in, size_t old_sz, size_t at, const char *rplc, size_t rplc_len, size_t find_len)
{
	const size_t capwant = old_sz + rplc_len + 64;
	char *s = xnew(capwant);
	char exp[2048];
	size_t sz = old_sz;
	size_t cap = capwant;
	size_t want;
	char *ret;

	memcpy(s, in, old_sz);
	s[old_sz] = '\0';
	want = ref_rplcat(exp, in, old_sz, at, rplc, rplc_len, find_len);
	ret = jstr_rplcat_len(&s, &sz, &cap, at, rplc, rplc_len, find_len);
	RCHECK(ret != NULL);
	RCHECK_EQ((size_t)(ret - s), at + rplc_len);
	RCHECK_EQ(sz, want);
	check_inv(s, sz, cap);
	RCHECK_MEM(s, exp, sz + 1);
	free(s);
}

int
main(void)
{
	static const size_t lens[] = { 0, 1, 2, 3, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
	static const size_t fls[] = { 0, 1, 2, 3 };
	static const size_t rls[] = { 0, 1, 2, 3, 5 };
	char in[160];
	char rplc[160];
	size_t k;

	/* Equal, shorter, longer and empty replacements. */
	one("abcabc", 6, 0, "ZZ", 2, 2);
	one("abcabc", 6, 0, "X", 1, 2);
	one("abcabc", 6, 0, "XYZ", 3, 2);
	one("abcabc", 6, 0, "", 0, 2);
	one("abcabc", 6, 2, "XYZW", 4, 1);
	one("abcabc", 6, 5, "Q", 1, 1);
	one("abcabc", 6, 6, "Q", 1, 0);
	one("abcabc", 6, 0, "Q", 1, 0);
	one("", 0, 0, "abc", 3, 0);
	one("abc", 3, 3, "", 0, 0);

	/* Embedded NUL bytes in the replacement are copied verbatim. */
	{
		const char bin[] = { 'a', '\0', 'b' };
		one("012345", 6, 2, bin, sizeof(bin), 1);
	}

	/* Growth from a truncated capacity (reserve is *sz + rplc - find + 1). */
	{
		char *s = xnew(4);
		size_t sz = 3;
		size_t cap = 0;
		char *ret;
		memcpy(s, "abc", 4);
		ret = jstr_rplcat_len(&s, &sz, &cap, 3, "XYZ", 3, 0);
		RCHECK(ret != NULL);
		RCHECK_EQ((size_t)(ret - s), 6);
		RCHECK_EQ(sz, 6);
		check_inv(s, sz, cap);
		RCHECK_MEM(s, "abcXYZ", 7);
		free(s);
	}

	/* Boundary lengths and all three replacement-size relations. */
	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (char)('a' + (int)((i + k) % 26));
		in[n] = '\0';
		for (size_t fi = 0; fi < sizeof(fls) / sizeof(fls[0]); ++fi) {
			const size_t fl = fls[fi];
			if (fl > n)
				continue;
			for (size_t ri = 0; ri < sizeof(rls) / sizeof(rls[0]); ++ri) {
				const size_t rl = rls[ri];
				for (size_t i = 0; i < rl; ++i)
					rplc[i] = (char)('A' + (int)(i % 26));
				rplc[rl] = '\0';
				one(in, n, 0, rplc, rl, fl);
				one(in, n, (n - fl) / 2, rplc, rl, fl);
				one(in, n, n - fl, rplc, rl, fl);
			}
		}
	}
	return 0;
}
