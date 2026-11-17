/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Regression coverage: a no-match must return NULL without inspecting s[n].
 * Previously jstr_mempbrk() tested *s[n] and returned s + n when that byte
 * was non-zero. */

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

static char *
ref_mempbrk(const void *s, const char *accept, size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	for (size_t i = 0; i < n; ++i)
		if (ref_in(p[i], accept))
			return (char *)(p + i);
	return NULL;
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

	RCHECK(jstr_mempbrk("hello", "ol", 5) == (char *)"hello" + 2);
	RCHECK(jstr_mempbrk("hello", "xo", 5) == (char *)"hello" + 4);
	RCHECK(jstr_mempbrk("hello", "h", 5) == (char *)"hello");
	RCHECK(jstr_mempbrk("hello", "xyz", 5) == NULL);
	RCHECK(jstr_mempbrk("hello", "", 5) == NULL);
	RCHECK(jstr_mempbrk("", "h", 0) == NULL);
	RCHECK(jstr_mempbrk("hello", "x", 5) == NULL);
	{
		const char bin[6] = { 'a', 'b', 'c', 0, 'd', 0 };
		RCHECK(jstr_mempbrk(bin, "d", 5) == bin + 4);
		RCHECK(jstr_mempbrk(bin, "d", 3) == NULL);
		RCHECK(jstr_mempbrk(bin, "a", 1) == bin);
		RCHECK(jstr_mempbrk(bin, "c", 3) == bin + 2);
	}
	{
		/* Matching byte inside n, garbage past n must be ignored. */
		const char g[6] = { 'q', 'q', 'b', 'q', 'Z', 0 };
		RCHECK(jstr_mempbrk(g, "b", 3) == g + 2);
		RCHECK(jstr_mempbrk(g, "q", 3) == g);
		RCHECK(jstr_mempbrk(g, "Z", 5) == g + 4);
		/* Regression: no match must be NULL even when s[n] != 0. */
		RCHECK(jstr_mempbrk(g, "z", 3) == NULL);
		RCHECK(jstr_mempbrk(g, "z", 0) == NULL);
		RCHECK(jstr_mempbrk(g, "", 5) == NULL);
	}

	rigor_srand(0x99aabbccu);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill(buf, len + 2, 1);
		for (size_t n = 0; n <= len + 2; ++n) {
			for (size_t si = 0; si < sizeof(sets) / sizeof(sets[0]); ++si)
				RCHECK(jstr_mempbrk(buf, sets[si], n) == ref_mempbrk(buf, sets[si], n));
			RCHECK(jstr_mempbrk(buf, bigset, n) == ref_mempbrk(buf, bigset, n));
		}
	}

	return 0;
}
