/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stdlib.h>
#include <string.h>

#include "../rigor.h"
#include "replace.h"

static void
one(const char *in, const char *reject, char rplc)
{
	const size_t len = strlen(in);
	const size_t cap = len + 32;
	char *const raw = (char *)malloc(cap);
	char *const s = raw + 8;
	char exp[256];
	size_t sz = len;
	size_t want = 0;
	size_t i = 0;

	RCHECK(raw != NULL);
	memset(raw, 0xa5, cap);
	memcpy(s, in, len + 1);
	memcpy(exp, in, len + 1);
	while (i < len) {
		if (strchr(reject, in[i]) != NULL) {
			while (i < len && strchr(reject, in[i]) != NULL) {
				exp[i] = rplc;
				++want;
				++i;
			}
		} else {
			++i;
		}
	}
	RCHECK_EQ(jstr_rplcallspn(s, &sz, reject, rplc), want);
	RCHECK_EQ(sz, len);
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
	char in[160];
	size_t k;

	/* All runs, not just the first. */
	one("..aa..bb", "ab", 'X');
	one("aa..bb", "ab", 'X');
	one("..aa..bb..", "ab", 'X');
	one("abcabc", "ab", 'X');
	one("abcabc", "c", 'X');
	one("abcabc", "z", 'X');
	one("abcabc", "", 'X');
	one("abcabc", "abc", 'X');
	one("", "a", 'X');
	one("aaaa", "a", 'a');
	one("xxabcxabcxx", "abc", 'Z');

	/* A run may mix several reject characters. */
	one("xabcabcy", "abc", 'Z');
	one("aabbccdd", "abcd", '.');

	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (char)('a' + (int)(i % 4));
		in[n] = '\0';
		one(in, "a", 'X');
		one(in, "ab", 'X');
		one(in, "abc", 'X');
		one(in, "", 'X');
		one(in, "z", 'X');
	}
	return 0;
}
