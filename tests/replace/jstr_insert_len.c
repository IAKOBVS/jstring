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

static void
ref_insert(char *out, const char *in, size_t sz, size_t at, const char *src, size_t src_len)
{
	memcpy(out, in, at);
	memcpy(out + at, src, src_len);
	memcpy(out + at + src_len, in + at, sz - at);
	out[sz + src_len] = '\0';
}

static void
check_inv(const char *s, size_t sz, size_t cap)
{
	RCHECK(cap == 0 || cap > sz);
	if (cap != 0)
		RCHECK_EQ(s[sz], 0);
}

int
main(void)
{
	static const size_t lens[] = { 0, 1, 2, 3, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
	const char src[] = "XYZ";
	const size_t src_len = sizeof(src) - 1;
	char in[128];
	char exp[256];
	size_t k;

	/* Empty input: allocation from scratch. */
	{
		char *s = xnew(1);
		size_t sz = 0;
		size_t cap = 0;
		s[0] = '\0';
		RCHECK_EQ(jstr_insert_len(&s, &sz, &cap, 0, src, src_len), JSTR_RET_SUCC);
		RCHECK_EQ(sz, src_len);
		check_inv(s, sz, cap);
		RCHECK_MEM(s, "XYZ", src_len + 1);
		free(s);
	}

	/* No-op insertion of an empty source still leaves the string intact. */
	{
		char *s = xnew(1);
		size_t sz = 0;
		size_t cap = 0;
		s[0] = '\0';
		RCHECK_EQ(jstr_insert_len(&s, &sz, &cap, 0, "", 0), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 0);
		check_inv(s, sz, cap);
		free(s);
	}

	/* Prepend, middle, append with a tight capacity (forces growth). */
	for (size_t at = 0; at <= 5; ++at) {
		char *s = xnew(6);
		size_t sz = 5;
		size_t cap = 6;
		memcpy(s, "hello", 6);
		RCHECK_EQ(jstr_insert_len(&s, &sz, &cap, at, src, src_len), JSTR_RET_SUCC);
		ref_insert(exp, "hello", 5, at, src, src_len);
		RCHECK_EQ(sz, 5 + src_len);
		check_inv(s, sz, cap);
		RCHECK_MEM(s, exp, sz + 1);
		free(s);
	}

	/* Enough capacity: the pointer must not move. */
	{
		char *s = xnew(64);
		char *const old = s;
		size_t sz = 3;
		size_t cap = 64;
		memcpy(s, "abc", 4);
		RCHECK_EQ(jstr_insert_len(&s, &sz, &cap, 1, "Q", 1), JSTR_RET_SUCC);
		RCHECK(s == old);
		RCHECK_EQ(cap, 64);
		RCHECK_EQ(sz, 4);
		RCHECK_MEM(s, "aQbc", 5);
		free(s);
	}

	/* Embedded NUL bytes in the inserted block. */
	{
		const char bin[] = { 'a', '\0', 'b' };
		char *s = xnew(16);
		size_t sz = 2;
		size_t cap = 16;
		memcpy(s, "hi", 3);
		RCHECK_EQ(jstr_insert_len(&s, &sz, &cap, 1, bin, sizeof(bin)), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 2 + sizeof(bin));
		RCHECK_MEM(s, "ha\0bi", 6);
		RCHECK_EQ(s[sz], 0);
		check_inv(s, sz, cap);
		free(s);
	}

	/* All boundary lengths, every insertion position, tight capacity. */
	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (char)('a' + (int)((i + k) % 26));
		in[n] = '\0';
		for (size_t at = 0; at <= n; ++at) {
			char *s;
			size_t sz;
			size_t cap;
			if (n == 0) {
				s = xnew(1);
				s[0] = '\0';
				sz = 0;
				cap = 0;
			} else {
				s = xnew(n + 1);
				sz = n;
				cap = n + 1;
				memcpy(s, in, n + 1);
			}
			RCHECK_EQ(jstr_insert_len(&s, &sz, &cap, at, src, src_len), JSTR_RET_SUCC);
			ref_insert(exp, in, n, at, src, src_len);
			RCHECK_EQ(sz, n + src_len);
			check_inv(s, sz, cap);
			RCHECK_MEM(s, exp, sz + 1);
			free(s);
		}
	}

	/* Growth across several realloc boundaries. */
	{
		char payload[200];
		char *s = xnew(1);
		size_t sz = 0;
		size_t cap = 0;
		size_t total = 0;
		s[0] = '\0';
		for (size_t i = 0; i < sizeof(payload); ++i)
			payload[i] = (char)('A' + (int)(i % 26));
		for (int i = 0; i < 20; ++i) {
			RCHECK_EQ(jstr_insert_len(&s, &sz, &cap, sz, payload, sizeof(payload)), JSTR_RET_SUCC);
			total += sizeof(payload);
			RCHECK_EQ(sz, total);
			check_inv(s, sz, cap);
			RCHECK_MEM(s + sz - sizeof(payload), payload, sizeof(payload));
		}
		free(s);
	}
	return 0;
}
