/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Regression coverage: the single-byte accept-set path must honour n.
 * Previously it scanned until the first non-accept byte and ignored the
 * byte count (e.g. jstr_memspn("aaaaX", "a", 2) returned 4). */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static int
ref_in(unsigned char c, const char *set)
{
	for (; *set; ++set)
		if ((unsigned char)*set == c)
			return 1;
	return 0;
}

static size_t
ref_memspn(const void *s, const char *accept, size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	if (!accept[0])
		return 0;
	size_t i = 0;
	for (; i < n && ref_in(p[i], accept); ++i) {}
	return i;
}

int
main(void)
{
	static const char *const sets[] = { "", "a", "ab", "abcXYZ", "\x01\x7f\x80\xff" };
	unsigned char buf[1200];
	char bigset[256];

	for (int b = 1; b < 256; ++b)
		bigset[b - 1] = (char)b;
	bigset[255] = 0;

	RCHECK(jstr_memspn("hello", "hel", 5) == 4);
	RCHECK(jstr_memspn("hello", "hel", 3) == 3);
	RCHECK(jstr_memspn("hello", "", 5) == 0);
	RCHECK(jstr_memspn("hello", "xyz", 5) == 0);
	RCHECK(jstr_memspn("aaaaX", "a", 4) == 4);
	RCHECK(jstr_memspn("aaaaX", "a", 5) == 4);
	RCHECK(jstr_memspn("aaaaX", "a", 3) == 3);
	RCHECK(jstr_memspn("aaaaX", "a", 2) == 2);
	RCHECK(jstr_memspn("aaaaX", "a", 1) == 1);
	RCHECK(jstr_memspn("aaaaX", "a", 0) == 0);
	RCHECK(jstr_memspn("Xaaa", "a", 0) == 0);
	RCHECK(jstr_memspn("Xaaa", "a", 3) == 0);
	{
		const char bin[5] = { 0, 'a', 'a', 'a', 0 };
		RCHECK(jstr_memspn(bin, "a", 5) == 0);
		RCHECK(jstr_memspn(bin + 1, "a", 3) == 3);
	}

	rigor_srand(0x55667788u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill(buf, len + 2, 1);
		for (size_t n = 0; n <= len + 2; ++n) {
			for (size_t si = 0; si < sizeof(sets) / sizeof(sets[0]); ++si) {
				const char *set = sets[si];
				RCHECK(jstr_memspn(buf, set, n) == ref_memspn(buf, set, n));
			}
			RCHECK(jstr_memspn(buf, bigset, n) == ref_memspn(buf, bigset, n));
		}
	}

	return 0;
}
