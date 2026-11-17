/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stdlib.h>
#include <string.h>

#include "../rigor.h"
#include "replace.h"

static void
one(const char *in, size_t sz, char find, char rplc)
{
	const size_t cap = sz + 40;
	char *const raw = (char *)malloc(cap);
	char *const s = raw + 8;
	char exp[512];
	size_t i;

	RCHECK(raw != NULL);
	memset(raw, 0xa5, cap);
	memcpy(s, in, sz);
	memcpy(exp, in, sz);
	for (i = 0; i < sz; ++i)
		if (exp[i] == find)
			exp[i] = rplc;
	jstr_rplcallchr_len(s, sz, find, rplc);
	RCHECK_MEM(s, exp, sz);
	for (i = 0; i < 8; ++i)
		RCHECK_EQ((unsigned char)raw[i], 0xa5);
	/* Bytes at and after the explicit length must not be touched. */
	for (i = sz; i < sz + 20; ++i)
		RCHECK_EQ((unsigned char)s[i], 0xa5);
	free(raw);
}

int
main(void)
{
	static const size_t lens[] = { 0, 1, 2, 3, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
	char in[160];
	size_t k;

	one("abcabc", 6, 'a', 'X');
	one("abcabc", 6, 'c', 'X');
	one("abcabc", 6, 'z', 'X');
	one("abcabc", 6, 'a', 'a');
	one("", 0, 'a', 'X');

	/* Embedded NULs are ordinary bytes, not a terminator. */
	{
		const char bin[] = { 'a', '\0', 'a', '\0', 'a' };
		one(bin, sizeof(bin), 'a', 'X');
		one(bin, sizeof(bin), '\0', 'Y');
		one(bin, sizeof(bin), 'a', '\0');
	}

	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (i % 5 == 2) ? '\0' : (char)('a' + (int)(i % 3));
		one(in, n, 'a', 'X');
		one(in, n, 'b', 'X');
		one(in, n, '\0', 'Z');
		one(in, n, 'z', 'X');
	}
	return 0;
}
