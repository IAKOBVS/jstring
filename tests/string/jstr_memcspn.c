/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

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
ref_memcspn(const void *s, const char *reject, size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	if (!reject[0])
		return n;
	size_t i = 0;
	for (; i < n && !ref_in(p[i], reject); ++i) {}
	return i;
}

int
main(void)
{
	/* Regression coverage: an empty reject set returns n, including over
	 * binary data with embedded NULs (previously it stopped at the first
	 * NUL because it used memchr(p, 0, n)). */
	static const char *const sets[] = { "", "a", "ab", "abcXYZ", "\x01\x7f\x80\xff" };
	unsigned char buf[1200];
	char bigset[256];

	for (int b = 1; b < 256; ++b)
		bigset[b - 1] = (char)b;
	bigset[255] = 0;

	RCHECK(jstr_memcspn("hello", "ol", 5) == 2);
	RCHECK(jstr_memcspn("hello", "", 5) == 5);
	RCHECK(jstr_memcspn("hello", "helo", 5) == 0);
	RCHECK(jstr_memcspn("hello", "xyz", 5) == 5);
	RCHECK(jstr_memcspn("hello", "o", 5) == 4);
	RCHECK(jstr_memcspn("hello", "o", 4) == 4);
	RCHECK(jstr_memcspn("hello", "o", 0) == 0);
	{
		const char bin[5] = { 'a', 'b', 0, 'c', 'd' };
		RCHECK(jstr_memcspn(bin, "d", 5) == 4);
		RCHECK(jstr_memcspn(bin, "c", 5) == 3);
		RCHECK(jstr_memcspn(bin, "", 5) == 5);
		RCHECK(jstr_memcspn(bin, "", 0) == 0);
		RCHECK(jstr_memcspn(bin, "b", 0) == 0);
	}

	rigor_srand(0x11223344u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill(buf, len + 2, 1);
		for (size_t n = 0; n <= len + 2; ++n) {
			for (size_t si = 0; si < sizeof(sets) / sizeof(sets[0]); ++si)
				RCHECK(jstr_memcspn(buf, sets[si], n) == ref_memcspn(buf, sets[si], n));
			RCHECK(jstr_memcspn(buf, bigset, n) == ref_memcspn(buf, bigset, n));
		}
	}

	return 0;
}
