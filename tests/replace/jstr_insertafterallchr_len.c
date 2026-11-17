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
ref_insertafterallchr(char *out, const char *in, size_t sz, unsigned char c, const char *src, size_t src_len)
{
	size_t o = 0;
	if (src_len == 0 || sz == 0) {
		memcpy(out, in, sz + 1);
		return sz;
	}
	for (size_t i = 0; i < sz; ++i) {
		out[o++] = in[i];
		if ((unsigned char)in[i] == c) {
			memcpy(out + o, src, src_len);
			o += src_len;
		}
	}
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
one(const char *in, size_t sz, int c, const char *src, size_t src_len)
{
	char *s = xnew(sz + 1);
	char exp[2048];
	const size_t cap0 = sz + 1;
	size_t cap = cap0;
	const size_t want = ref_insertafterallchr(exp, in, sz, (unsigned char)c, src, src_len);
	memcpy(s, in, sz + 1);
	RCHECK_EQ(jstr_insertafterallchr_len(&s, &sz, &cap, c, src, src_len), JSTR_RET_SUCC);
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
	char src[128];
	size_t k;

	/* Explicit start / middle / end / repeated matches. */
	one("abcabc", 6, 'a', "XY", 2);
	one("abcabc", 6, 'b', "XY", 2);
	one("abcabc", 6, 'c', "XY", 2);
	one("abcabc", 6, 'z', "XY", 2);
	one("", 0, 'a', "XY", 2);
	one("aaa", 3, 'a', "XY", 2);
	one("a", 1, 'a', "XY", 2);
	one("abc", 3, 'a', "", 0);
	one("abc", 3, 'b', "", 0);

	/* The inserted bytes are not rescanned: src may contain the match char. */
	one("a-a-a", 5, 'a', "a", 1);

	/* Embedded NUL bytes are matched and copied byte-for-byte. */
	{
		const char v[] = { 'a', '\0', 'b', '\0' };
		one(v, 3, '\0', "X", 1);
	}

	/* Everything matches: the result is the repeated pair. */
	{
		char *s = xnew(8);
		size_t sz = 4;
		size_t cap = 8;
		memcpy(s, "aaaa", 5);
		RCHECK_EQ(jstr_insertafterallchr_len(&s, &sz, &cap, 'a', "XY", 2), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 12);
		RCHECK_MEM(s, "aXYaXYaXYaXY", 13);
		check_inv(s, sz, cap);
		free(s);
	}

	/* All boundary lengths, tight capacity (forces one exact growth). */
	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (i % 3 == 0) ? 'a' : ((i % 3 == 1) ? 'b' : 'c');
		in[n] = '\0';
		for (size_t i = 0; i < 40; ++i)
			src[i] = (char)('A' + (int)(i % 26));
		one(in, n, 'a', src, 3);
		one(in, n, 'b', src, 3);
		one(in, n, 'z', src, 3);
		one(in, n, 'a', src, 40);
	}
	return 0;
}
