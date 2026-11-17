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
ref_memrcspn(const void *s, const char *reject, size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	if (!reject[0])
		return n;
	size_t i = n;
	for (; i > 0 && !ref_in(p[i - 1], reject); --i) {}
	return i;
}

int
main(void)
{
	static const char *const sets[] = { "", "l", "lo", "/ ", "\x01\x7f\x80\xff" };
	unsigned char buf[1200];
	char bigset[256];

	for (int b = 1; b < 256; ++b)
		bigset[b - 1] = (char)b;
	bigset[255] = 0;

	RCHECK(jstr_strrcspn_len("/usr/local", "/", 10) == 5);
	RCHECK(jstr_strrcspn_len("hello", "/", 5) == 0);
	RCHECK(jstr_strrcspn_len("hello", "", 5) == 5);
	RCHECK(jstr_strrcspn_len("hello", "o", 5) == 5);
	RCHECK(jstr_strrcspn_len("hello", "l", 5) == 4);
	RCHECK(jstr_strrcspn_len("hello", "l", 4) == 4);
	RCHECK(jstr_strrcspn_len("hello", "h", 5) == 1);
	RCHECK(jstr_strrcspn_len("", "x", 0) == 0);
	RCHECK(jstr_strrcspn_len("abc   ", " ", 4) == 4);
	RCHECK(jstr_strrcspn_len("abc   ", " ", 6) == 6);
	{
		const char bin[5] = { 'a', 0, 'b', 0, 'c' };
		RCHECK(jstr_strrcspn_len(bin, "b", 5) == 3);
		RCHECK(jstr_strrcspn_len(bin, "a", 5) == 1);
	}

	rigor_srand(0xabcdef01u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill(buf, len + 2, 1);
		for (size_t n = 0; n <= len + 2; ++n) {
			for (size_t si = 0; si < sizeof(sets) / sizeof(sets[0]); ++si)
				RCHECK(jstr_strrcspn_len((const char *)buf, sets[si], n) == ref_memrcspn(buf, sets[si], n));
			RCHECK(jstr_strrcspn_len((const char *)buf, bigset, n) == ref_memrcspn(buf, bigset, n));
		}
	}

	return 0;
}
