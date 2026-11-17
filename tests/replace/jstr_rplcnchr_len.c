/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stdlib.h>
#include <string.h>

#include "../rigor.h"
#include "replace.h"

static void
one(const char *in, size_t sz, char find, char rplc, size_t n)
{
	const size_t cap = sz + 40;
	char *const raw = (char *)malloc(cap);
	char *const s = raw + 8;
	char exp[512];
	size_t want = 0;
	size_t i;

	RCHECK(raw != NULL);
	memset(raw, 0xa5, cap);
	memcpy(s, in, sz);
	memcpy(exp, in, sz);
	for (i = 0; i < sz && want < n; ++i) {
		if (exp[i] == find) {
			exp[i] = rplc;
			++want;
		}
	}
	RCHECK_EQ(jstr_rplcnchr_len(s, sz, find, rplc, n), want);
	RCHECK_MEM(s, exp, sz);
	for (i = 0; i < 8; ++i)
		RCHECK_EQ((unsigned char)raw[i], 0xa5);
	for (i = sz; i < sz + 20; ++i)
		RCHECK_EQ((unsigned char)s[i], 0xa5);
	free(raw);
}

int
main(void)
{
	static const size_t lens[] = { 0, 1, 2, 3, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
	static const size_t ns[] = { 0, 1, 2, 3, 100, (size_t)-1 };
	char in[160];
	size_t k;

	one("abcabc", 6, 'a', 'X', 0);
	one("abcabc", 6, 'a', 'X', 1);
	one("abcabc", 6, 'a', 'X', 2);
	one("abcabc", 6, 'a', 'X', 100);
	one("abcabc", 6, 'a', 'X', (size_t)-1);
	one("abcabc", 6, 'z', 'X', 100);
	one("", 0, 'a', 'X', 100);
	one("aaaa", 4, 'a', 'a', 2);

	/* Embedded NULs (including replacing an embedded NUL) with explicit len. */
	{
		const char bin[] = { 'a', '\0', 'a', '\0', 'a' };
		for (size_t j = 0; j < sizeof(ns) / sizeof(ns[0]); ++j) {
			one(bin, sizeof(bin), 'a', 'X', ns[j]);
			one(bin, sizeof(bin), '\0', 'Y', ns[j]);
			one(bin, sizeof(bin), 'b', 'X', ns[j]);
		}
	}

	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (i % 5 == 2) ? '\0' : (char)('a' + (int)(i % 3));
		for (size_t j = 0; j < sizeof(ns) / sizeof(ns[0]); ++j) {
			one(in, n, 'a', 'X', ns[j]);
			one(in, n, 'b', 'X', ns[j]);
			one(in, n, '\0', 'Z', ns[j]);
			one(in, n, 'z', 'X', ns[j]);
		}
	}
	return 0;
}
