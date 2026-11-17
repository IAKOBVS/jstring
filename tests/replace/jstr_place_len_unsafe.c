/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stdlib.h>
#include <string.h>

#include "../rigor.h"
#include "replace.h"

static void
one(const char *in, size_t in_len, size_t at, const char *src, size_t src_len)
{
	size_t body = in_len + 1;
	size_t cap;
	char *raw;
	char *s;
	char exp[512];
	size_t i;

	if (at + src_len > body)
		body = at + src_len;
	cap = body + 40;
	raw = (char *)malloc(cap);
	s = raw + 8;
	RCHECK(raw != NULL);
	memset(raw, 0xa5, cap);
	memcpy(s, in, in_len);
	s[in_len] = '\0';
	memset(exp, 0xa5, sizeof(exp));
	memcpy(exp, s, in_len + 1);
	memcpy(exp + at, src, src_len);
	jstr_place_len_unsafe(s, at, src, src_len);
	RCHECK_MEM(s, exp, body);
	for (i = 0; i < 8; ++i)
		RCHECK_EQ((unsigned char)raw[i], 0xa5);
	for (i = body; i < body + 16; ++i)
		RCHECK_EQ((unsigned char)s[i], 0xa5);
	free(raw);
}

int
main(void)
{
	static const size_t lens[] = { 0, 1, 2, 3, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
	char in[160];
	char src[160];
	size_t k;

	/* Explicit start / middle / end placements. */
	one("hello", 5, 0, "XYZ", 3);
	one("hello", 5, 1, "XYZ", 3);
	one("hello", 5, 2, "XYZ", 3);
	one("hello", 5, 5, "XYZ", 3);
	one("hello", 5, 1, "", 0);
	one("", 0, 0, "XYZ", 3);

	/* Binary source bytes (including embedded NUL) are copied verbatim. */
	{
		const char bin[] = { 'a', '\0', 'b', (char)0xff };
		one("0123456789", 10, 3, bin, sizeof(bin));
	}

	/* Every boundary length with a short source at every position. */
	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (char)('a' + (int)((i + k) % 26));
		in[n] = '\0';
		for (size_t at = 0; at <= n; ++at) {
			src[0] = 'X';
			src[1] = 'Y';
			src[2] = 'Z';
			one(in, n, at, src, 3);
		}
		for (size_t i = 0; i < 40; ++i)
			src[i] = (char)('A' + (int)(i % 26));
		one(in, n, n / 2, src, 40);
	}
	return 0;
}
