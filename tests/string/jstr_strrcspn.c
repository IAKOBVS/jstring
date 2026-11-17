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
ref_strrcspn(const char *s, const char *reject)
{
	size_t n = strlen(s);
	if (!reject[0])
		return n;
	size_t i = n;
	for (; i > 0 && !ref_in((unsigned char)s[i - 1], reject); --i) {}
	return i;
}

int
main(void)
{
	static const char *const decoys[] = { "", "/", "lo", " ", "xyz" };
	char buf[1200];

	RCHECK(jstr_strrcspn("/usr/local", "/") == 5);
	RCHECK(jstr_strrcspn("hello", "/") == 0);
	RCHECK(jstr_strrcspn("hello", "") == 5);
	RCHECK(jstr_strrcspn("hello", "lo") == 5);
	RCHECK(jstr_strrcspn("hello", "l") == 4);
	RCHECK(jstr_strrcspn("hello", "h") == 1);
	RCHECK(jstr_strrcspn("", "x") == 0);
	RCHECK(jstr_strrcspn("a/b/c", "/") == 4);
	RCHECK(jstr_strrcspn("a/b/c/", "/") == 6);

	rigor_srand(0x0f1e2d3cu);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill_printable(buf, len);
		for (size_t d = 0; d < sizeof(decoys) / sizeof(decoys[0]); ++d)
			RCHECK(jstr_strrcspn(buf, decoys[d]) == ref_strrcspn(buf, decoys[d]));
		/* Single delimiters that occur (or not) in the payload. */
		RCHECK(jstr_strrcspn(buf, "~") == ref_strrcspn(buf, "~"));
		RCHECK(jstr_strrcspn(buf, "z") == ref_strrcspn(buf, "z"));
	}

	return 0;
}
