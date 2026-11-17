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
ref_last(const char *in, size_t sz, const char *find, size_t find_len)
{
	if (find_len == 0)
		return sz;
	if (sz < find_len)
		return sz;
	for (size_t i = sz - find_len + 1; i-- > 0;)
		if (memcmp(in + i, find, find_len) == 0)
			return i;
	return sz;
}

static size_t
ref_rplclast(char *out, const char *in, size_t sz, const char *find, size_t find_len, const char *rplc, size_t rplc_len)
{
	const size_t p = ref_last(in, sz, find, find_len);
	size_t o = 0;
	if (p == sz && find_len != 0) {
		memcpy(out, in, sz + 1);
		return sz;
	}
	memcpy(out + o, in, p);
	o += p;
	memcpy(out + o, rplc, rplc_len);
	o += rplc_len;
	memcpy(out + o, in + p + find_len, sz - p - find_len);
	o += sz - p - find_len;
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
one(const char *in, size_t old_sz, const char *find, size_t find_len, const char *rplc, size_t rplc_len)
{
	const size_t capwant = old_sz + rplc_len + 64;
	char *s = xnew(capwant);
	char exp[2048];
	size_t sz = old_sz;
	size_t cap = capwant;
	size_t want;
	int ret;
	const int matched = find_len == 0 || ref_last(in, old_sz, find, find_len) != old_sz;

	memcpy(s, in, old_sz);
	s[old_sz] = '\0';
	want = ref_rplclast(exp, in, old_sz, find, find_len, rplc, rplc_len);
	ret = jstr_rplclast_len(&s, &sz, &cap, find, find_len, rplc, rplc_len);
	RCHECK_EQ(ret, matched ? 1 : 0);
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
	char rplc[160];
	size_t k;

	/* No match / single match at start / middle / end. */
	one("abcabc", 6, "zz", 2, "X", 1);
	one("abcabc", 6, "ab", 2, "X", 1);
	one("abcabc", 6, "bc", 2, "X", 1);
	one("abcabc", 6, "ca", 2, "X", 1);
	one("abcabc", 6, "abc", 3, "X", 1);
	one("abc", 3, "abcd", 4, "X", 1);
	one("abc", 3, "a", 1, "X", 1);

	/* Multiple matches: only the last changes. */
	one("abcabc", 6, "a", 1, "X", 1);
	one("abcabc", 6, "b", 1, "X", 1);
	one("banana", 6, "an", 2, "X", 1);

	/* Overlapping last occurrence: "aaa"/"aa" -> last at index 1. */
	one("aaa", 3, "aa", 2, "X", 1);
	one("aaaa", 4, "aa", 2, "X", 1);

	/* Equal / shorter / longer / empty replacements. */
	one("abcabc", 6, "bc", 2, "XY", 2);
	one("abcabc", 6, "bc", 2, "Z", 1);
	one("abcabc", 6, "bc", 2, "WXYZ", 4);
	one("abcabc", 6, "bc", 2, "", 0);
	one("abcabc", 6, "abc", 3, "", 0);

	/* Empty find appends the replacement (current documented-by-code form). */
	one("abc", 3, "", 0, "X", 1);
	one("", 0, "", 0, "X", 1);

	/* Growth from a truncated capacity. */
	{
		char *s = xnew(4);
		size_t sz = 3;
		size_t cap = 0;
		memcpy(s, "abc", 4);
		RCHECK_EQ(jstr_rplclast_len(&s, &sz, &cap, "b", 1, "WXYZ", 4), 1);
		RCHECK_EQ(sz, 6);
		check_inv(s, sz, cap);
		RCHECK_MEM(s, "aWXYZc", 7);
		free(s);
	}

	/* Boundary lengths and multiple needles. */
	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (char)('a' + (int)(i % 3));
		in[n] = '\0';
		for (size_t rl = 0; rl <= 4; ++rl) {
			for (size_t i = 0; i < rl; ++i)
				rplc[i] = (char)('A' + (int)(i % 26));
			rplc[rl] = '\0';
			one(in, n, "a", 1, rplc, rl);
			one(in, n, "ab", 2, rplc, rl);
			one(in, n, "abc", 3, rplc, rl);
			one(in, n, "zz", 2, rplc, rl);
		}
	}
	return 0;
}
