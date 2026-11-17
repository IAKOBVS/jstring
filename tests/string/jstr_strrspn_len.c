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
ref_memrspn(const void *s, const char *accept, size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	if (!accept[0])
		return 0;
	size_t i = n;
	for (; i > 0 && ref_in(p[i - 1], accept); --i) {}
	return i;
}

int
main(void)
{
	static const char *const sets[] = { "", "l", "hel", " ", "\x01\x7f\x80\xff" };
	unsigned char buf[1200];
	char bigset[256];

	for (int b = 1; b < 256; ++b)
		bigset[b - 1] = (char)b;
	bigset[255] = 0;

	RCHECK(jstr_strrspn_len("hello", "hel", 5) == 5);
	RCHECK(jstr_strrspn_len("ohell", "hel", 5) == 1);
	RCHECK(jstr_strrspn_len("abc", "abc", 3) == 0);
	RCHECK(jstr_strrspn_len("abc", "", 3) == 0);
	RCHECK(jstr_strrspn_len("abc", "xyz", 3) == 3);
	RCHECK(jstr_strrspn_len("abc   ", " ", 6) == 3);
	RCHECK(jstr_strrspn_len("abc   ", " ", 4) == 3);
	RCHECK(jstr_strrspn_len("abc   ", " ", 3) == 3);
	RCHECK(jstr_strrspn_len("", "x", 0) == 0);
	RCHECK(jstr_strrspn_len("aaaa", "a", 3) == 0);
	{
		const char bin[4] = { 'a', 'b', 0, 'c' };
		RCHECK(jstr_strrspn_len(bin, "c", 4) == 3);
		RCHECK(jstr_strrspn_len(bin, "a", 4) == 4);
		RCHECK(jstr_strrspn_len(bin, "bc", 4) == 3);
		RCHECK(jstr_strrspn_len(bin, "c", 3) == 3);
	}

	rigor_srand(0x76543210u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill(buf, len + 2, 1);
		for (size_t n = 0; n <= len + 2; ++n) {
			for (size_t si = 0; si < sizeof(sets) / sizeof(sets[0]); ++si)
				RCHECK(jstr_strrspn_len((const char *)buf, sets[si], n) == ref_memrspn(buf, sets[si], n));
			RCHECK(jstr_strrspn_len((const char *)buf, bigset, n) == ref_memrspn(buf, bigset, n));
		}
	}

	return 0;
}
