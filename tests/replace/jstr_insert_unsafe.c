/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stdlib.h>
#include <string.h>

#include "../rigor.h"
#include "replace.h"

static void
ref_insert(char *out, const char *in, size_t sz, size_t at, const char *src, size_t src_len)
{
	memcpy(out, in, at);
	memcpy(out + at, src, src_len);
	memcpy(out + at + src_len, in + at, sz - at);
	out[sz + src_len] = '\0';
}

static void
one(const char *in, size_t sz, size_t at, const char *src, size_t src_len)
{
	const size_t body = sz + src_len;
	const size_t cap = body + 32;
	char *const raw = (char *)malloc(cap);
	char *const s = raw + 8;
	char exp[512];
	size_t i;

	RCHECK(raw != NULL);
	memset(raw, 0xa5, cap);
	memcpy(s, in, sz);
	s[sz] = '\0';
	ref_insert(exp, in, sz, at, src, src_len);
	jstr_insert_unsafe(s, sz, at, src, src_len);
	RCHECK_MEM(s, exp, body + 1);
	for (i = 0; i < 8; ++i)
		RCHECK_EQ((unsigned char)raw[i], 0xa5);
	for (i = body + 1; i < body + 16; ++i)
		RCHECK_EQ((unsigned char)s[i], 0xa5);
	free(raw);
}

int
main(void)
{
	static const size_t lens[] = { 0, 1, 2, 3, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
	char in[128];
	char src[128];
	size_t k;

	/* Explicit start / middle / end insertions. */
	one("hello", 5, 0, "XY", 2);
	one("hello", 5, 2, "XY", 2);
	one("hello", 5, 5, "XY", 2);
	one("", 0, 0, "abc", 3);
	one("abc", 3, 0, "", 0);
	one("abc", 3, 1, "", 0);
	one("abc", 3, 3, "", 0);

	/* src may contain embedded NUL bytes: no byte may be skipped. */
	{
		const char bin[] = { 'a', '\0', 'b', (char)0xff };
		one("1234", 4, 2, bin, sizeof(bin));
	}

	/* Every boundary length, inserting a short block at each position. */
	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		size_t at;
		for (at = 0; at <= n; ++at) {
			for (size_t i = 0; i < n; ++i)
				in[i] = (char)('a' + (int)((i * 7 + k) % 26));
			src[0] = 'X';
			src[1] = 'Y';
			src[2] = 'Z';
			one(in, n, at, src, 3);
		}
		/* Longer source exercises multi-vector memmove/memcpy. */
		for (size_t i = 0; i < 40; ++i)
			src[i] = (char)('A' + (int)(i % 26));
		one(in, n, n / 2, src, 40);
	}
	return 0;
}
