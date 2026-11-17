/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stdlib.h>
#include <string.h>

#include "../rigor.h"
#include "replace.h"

static void
one(const char *in, char find, char rplc, size_t n)
{
	const size_t len = strlen(in);
	const size_t cap = len + 32;
	char *const raw = (char *)malloc(cap);
	char *const s = raw + 8;
	char exp[256];
	size_t want = 0;
	size_t i;

	RCHECK(raw != NULL);
	memset(raw, 0xa5, cap);
	memcpy(s, in, len + 1);
	memcpy(exp, in, len + 1);
	for (i = 0; i < len && want < n; ++i) {
		if (exp[i] == find) {
			exp[i] = rplc;
			++want;
		}
	}
	RCHECK_EQ(jstr_rplcnchr(s, find, rplc, n), want);
	RCHECK_EQ(strlen(s), len);
	RCHECK_MEM(s, exp, len + 1);
	for (i = 0; i < 8; ++i)
		RCHECK_EQ((unsigned char)raw[i], 0xa5);
	for (i = len + 1; i < len + 20; ++i)
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

	one("abcabc", 'a', 'X', 0);
	one("abcabc", 'a', 'X', 1);
	one("abcabc", 'a', 'X', 2);
	one("abcabc", 'a', 'X', 3);
	one("abcabc", 'a', 'X', 100);
	one("abcabc", 'a', 'X', (size_t)-1);
	one("abcabc", 'z', 'X', 100);
	one("", 'a', 'X', 100);
	one("aaaa", 'a', 'a', 2);
	one("banana", 'a', 'o', 2);

	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (i % 3 == 0) ? 'a' : ((i % 3 == 1) ? 'b' : 'c');
		in[n] = '\0';
		for (size_t j = 0; j < sizeof(ns) / sizeof(ns[0]); ++j) {
			one(in, 'a', 'X', ns[j]);
			one(in, 'b', 'X', ns[j]);
			one(in, 'z', 'X', ns[j]);
		}
	}
	return 0;
}
