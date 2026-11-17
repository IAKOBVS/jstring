/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stdlib.h>
#include <string.h>

#include "../rigor.h"
#include "replace.h"

static void
one(const char *in, char find, char rplc)
{
	const size_t len = strlen(in);
	const size_t cap = len + 32;
	char *const raw = (char *)malloc(cap);
	char *const s = raw + 8;
	char exp[256];
	size_t i;
	int want = 0;

	RCHECK(raw != NULL);
	memset(raw, 0xa5, cap);
	memcpy(s, in, len + 1);
	memcpy(exp, in, len + 1);
	for (i = 0; i < len; ++i) {
		if (exp[i] == find) {
			exp[i] = rplc;
			want = 1;
			break;
		}
	}
	RCHECK_EQ(jstr_rplcchr(s, find, rplc), want);
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

	one("abcabc", 'a', 'X');
	one("abcabc", 'b', 'X');
	one("abcabc", 'c', 'X');
	one("abcabc", 'z', 'X');
	one("", 'a', 'X');
	one("a", 'a', 'X');
	one("abc", 'a', 'a');

	/* Only the first of several matches is changed. */
	one("aaaaa", 'a', 'X');

	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (i % 3 == 0) ? 'a' : ((i % 3 == 1) ? 'b' : 'c');
		in[n] = '\0';
		one(in, 'a', 'X');
		one(in, 'b', 'X');
		one(in, 'c', 'a');
		one(in, 'z', 'X');
	}
	return 0;
}
