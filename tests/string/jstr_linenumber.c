/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static size_t
ref_linenumber(const char *start, const char *end)
{
	size_t n = 1;
	for (const char *p = start; p < end; ++p)
		if (*p == '\n')
			++n;
	return n;
}

int
main(void)
{
	const char *s = "a\nb\nc";
	char buf[1100];

	RCHECK(jstr_linenumber(s, s + 5) == 3);
	RCHECK(jstr_linenumber(s, s) == 1);
	RCHECK(jstr_linenumber(s, s + 1) == 1);
	RCHECK(jstr_linenumber(s, s + 2) == 2);
	RCHECK(jstr_linenumber("", "") == 1);
	RCHECK(jstr_linenumber("\n", "\n" + 1) == 2);
	RCHECK(jstr_linenumber("\n\n\n", "\n\n\n" + 3) == 4);
	RCHECK(jstr_linenumber("abc", "abc" + 3) == 1);
	RCHECK(jstr_linenumber("abc\n", "abc\n" + 4) == 2);
	RCHECK(jstr_linenumber("a\r\nb", "a\r\nb" + 4) == 2);
	RCHECK(jstr_linenumber("a\nb", "a\nb" + 1) == 1);
	RCHECK(jstr_linenumber("a\nb" + 2, "a\nb" + 3) == 1);

	rigor_srand(0x2c3d4e5fu);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill_printable(buf, len);
		for (size_t i = 0; i < len; ++i)
			if ((rigor_rand() & 5u) == 0)
				buf[i] = '\n';
		for (size_t a = 0; a <= len; ++a)
			for (size_t b = a; b <= len; ++b)
				RCHECK(jstr_linenumber(buf + a, buf + b) ==
				       ref_linenumber(buf + a, buf + b));
	}

	return 0;
}
