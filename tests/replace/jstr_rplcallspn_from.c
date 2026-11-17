/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stdlib.h>
#include <string.h>

#include "../rigor.h"
#include "replace.h"

static void
one(const char *in, size_t start, const char *reject, char rplc)
{
	const size_t len = strlen(in);
	const size_t cap = len + 32;
	char *const raw = (char *)malloc(cap);
	char *const s = raw + 8;
	char exp[256];
	size_t sz = len;
	size_t want = 0;
	size_t i;

	RCHECK(raw != NULL);
	memset(raw, 0xa5, cap);
	memcpy(s, in, len + 1);
	memcpy(exp, in, len + 1);
	if (start < len) {
		i = start;
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
	}
	RCHECK_EQ(jstr_rplcallspn_from(s, &sz, start, reject, rplc), want);
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
	static const size_t starts[] = { 0, 1, 2, 3, 7, 100 };
	char in[160];
	size_t k;

	/* start == 0 is the whole string. */
	one("..aa..bb", 0, "ab", 'X');
	one("abcabc", 0, "ab", 'X');
	one("abcabc", 0, "", 'X');
	one("", 0, "a", 'X');

	/* Starting in the middle: bytes before start must be untouched. */
	one("..aa..bb", 2, "ab", 'X');
	one("..aa..bb", 3, "ab", 'X');
	one("..aa..bb", 4, "ab", 'X');
	one("abcabc", 2, "ab", 'X');
	one("abcabc", 3, "abc", 'X');
	one("abcabc", 5, "c", 'X');
	one("abcabc", 2, "z", 'X');

	/* start at / beyond the end is a no-op. */
	one("abcabc", 6, "ab", 'X');
	one("abcabc", 7, "ab", 'X');
	one("abcabc", 100, "ab", 'X');

	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (char)('a' + (int)(i % 4));
		in[n] = '\0';
		for (size_t j = 0; j < sizeof(starts) / sizeof(starts[0]); ++j) {
			const size_t start = starts[j] <= n + 1 ? starts[j] : n + 1;
			one(in, start, "a", 'X');
			one(in, start, "ab", 'X');
			one(in, start, "abc", 'X');
			one(in, start, "", 'X');
			one(in, start, "z", 'X');
		}
	}
	return 0;
}
