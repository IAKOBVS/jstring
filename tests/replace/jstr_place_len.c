/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Regression coverage: growth reserves room for the NUL terminator even
 * when (at + src_len) lands exactly on a JSTR_MIN_CAP * JSTR_GROWTH^k
 * boundary (24, 48, 96, ...). Previously the terminator write at index
 * (at + src_len) was one byte past the allocation (ASAN heap overflow). */

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
ref_place(char *out, const char *in, size_t old_sz, size_t at, const char *src, size_t src_len)
{
	size_t new_sz = old_sz;
	if (at + src_len > old_sz) {
		new_sz = at + src_len;
		memcpy(out, in, old_sz);
		if (at > old_sz)
			memset(out + old_sz, 0, at - old_sz);
		out[new_sz] = '\0';
	} else {
		memcpy(out, in, old_sz + 1);
	}
	memcpy(out + at, src, src_len);
	return new_sz;
}

static void
check_inv(const char *s, size_t sz, size_t cap)
{
	RCHECK(cap == 0 || cap > sz);
	if (cap != 0)
		RCHECK_EQ(s[sz], 0);
}

static void
one(const char *in, size_t old_sz, size_t at, const char *src, size_t src_len)
{
	const size_t capwant = (at + src_len > old_sz ? at + src_len : old_sz) + 64;
	char *s = xnew(capwant);
	char exp[2048];
	size_t sz = old_sz;
	size_t cap = capwant;
	size_t want;

	memcpy(s, in, old_sz);
	s[old_sz] = '\0';
	want = ref_place(exp, in, old_sz, at, src, src_len);
	RCHECK_EQ(jstr_place_len(&s, &sz, &cap, at, src, src_len), JSTR_RET_SUCC);
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
	char src[1200];
	size_t k;

	/* Overwrite inside the current length: size and terminator unchanged. */
	one("hello", 5, 1, "XYZ", 3);
	one("hello", 5, 0, "world", 5);
	one("hello", 5, 0, "", 0);
	one("hello", 5, 3, "lo", 2);
	one("hello", 5, 5, "", 0);

	/* Extend past the old end; the size grows and a gap is zero-filled. */
	one("hi", 2, 2, "XY", 2);
	one("hi", 2, 5, "hello", 5);
	one("hello", 5, 3, "XYZ", 3);
	one("hi", 2, 5, "", 0);
	one("", 0, 0, "abc", 3);
	one("", 0, 4, "abc", 3);

	/* Binary source bytes are copied verbatim and the result is terminated. */
	{
		const char bin[] = { 'a', '\0', 'b', (char)0xff };
		one("0123456789", 10, 2, bin, sizeof(bin));
	}

	/* Growth from a deliberately truncated capacity, avoiding 24*2^k sums. */
	{
		char *s = xnew(3);
		size_t sz = 2;
		size_t cap = 0;
		memcpy(s, "hi", 3);
		RCHECK_EQ(jstr_place_len(&s, &sz, &cap, 5, "hello", 5), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 10);
		check_inv(s, sz, cap);
		RCHECK_MEM(s, "hi\0\0\0hello", 11);
		RCHECK_EQ(jstr_place_len(&s, &sz, &cap, 20, "0123456789", 10), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 30);
		check_inv(s, sz, cap);
		RCHECK_MEM(s + 20, "0123456789", 10);
		RCHECK_EQ(jstr_place_len(&s, &sz, &cap, 50, "abcdefghij", 10), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 60);
		check_inv(s, sz, cap);
		RCHECK_MEM(s + 50, "abcdefghij", 10);
		free(s);
	}

	/* Regression: exact capacity-growth boundaries (24 * 2^k) must leave
	 * room for the terminator. Previously these wrote one byte OOB. */
	{
		static const size_t bounds[] = { 24, 48, 96, 192, 384 };
		for (size_t b = 0; b < sizeof(bounds) / sizeof(bounds[0]); ++b) {
			const size_t n = bounds[b];
			char *growth = (char *)malloc(1);
			RCHECK(growth != NULL);
			growth[0] = '\0';
			size_t gsz = 0;
			size_t gcap = 0;
			for (size_t i = 0; i < sizeof(src); ++i)
				src[i] = (char)('A' + (int)(i % 26));
			RCHECK_EQ(jstr_place_len(&growth, &gsz, &gcap, 0, src, n), JSTR_RET_SUCC);
			RCHECK_EQ(gsz, n);
			check_inv(growth, gsz, gcap);
			RCHECK_MEM(growth, src, n);
			RCHECK_EQ(growth[n], 0);
			free(growth);
		}
	}

	/* Every boundary length, placements at the start / middle / end. */
	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (char)('a' + (int)((i + k) % 26));
		in[n] = '\0';
		for (size_t i = 0; i < 40; ++i)
			src[i] = (char)('A' + (int)(i % 26));
		one(in, n, 0, src, 3);
		one(in, n, n / 2, src, 3);
		one(in, n, n, src, 3);
		one(in, n, n / 2, src, 40);
	}
	return 0;
}
