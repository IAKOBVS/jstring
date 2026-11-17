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
ref_strrspn(const char *s, const char *accept)
{
	size_t n = strlen(s);
	if (!accept[0])
		return 0;
	size_t i = n;
	for (; i > 0 && ref_in((unsigned char)s[i - 1], accept); --i) {}
	return i;
}

int
main(void)
{
	static const char *const sets[] = { "", "l", "hel", " ", "xyz", "abc" };
	char buf[1200];

	RCHECK(jstr_strrspn("hello", "hel") == 5);
	RCHECK(jstr_strrspn("ohell", "hel") == 1);
	RCHECK(jstr_strrspn("abc", "abc") == 0);
	RCHECK(jstr_strrspn("abc", "") == 0);
	RCHECK(jstr_strrspn("abc", "xyz") == 3);
	RCHECK(jstr_strrspn("abc   ", " ") == 3);
	RCHECK(jstr_strrspn("   abc", " ") == 6);
	RCHECK(jstr_strrspn("", "x") == 0);
	RCHECK(jstr_strrspn("aaaa", "a") == 0);
	RCHECK(jstr_strrspn("xaaa", "a") == 1);

	rigor_srand(0x10203040u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill_printable(buf, len);
		for (size_t si = 0; si < sizeof(sets) / sizeof(sets[0]); ++si)
			RCHECK(jstr_strrspn(buf, sets[si]) == ref_strrspn(buf, sets[si]));
	}

	return 0;
}
