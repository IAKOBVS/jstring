/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static char *
ref_linestart(const char *start, const char *end)
{
	const char *p = end;
	while (p > start)
		if (*--p == '\n')
			return (char *)(p + 1);
	return (char *)start;
}

int
main(void)
{
	const char s[] = "abc\ndef\nghi";
	char buf[1100];

	RCHECK(jstr_linestart(s, s + 11) == s + 8);
	RCHECK(jstr_linestart(s, s + 8) == s + 8);
	RCHECK(jstr_linestart(s, s + 7) == s + 4);
	RCHECK(jstr_linestart(s + 4, s + 7) == s + 4);
	RCHECK(jstr_linestart(s + 5, s + 11) == s + 8);
	RCHECK(jstr_linestart(s + 4, s + 4) == s + 4);
	RCHECK(jstr_linestart(s, s) == s);
	RCHECK(jstr_linestart("abc", "abc" + 3) == "abc");
	RCHECK(jstr_linestart("\nabc", "\nabc" + 4) == "\nabc" + 1);
	RCHECK(jstr_linestart("a\n\nb", "a\n\nb" + 4) == "a\n\nb" + 3);
	RCHECK(jstr_linestart("a\r\nb", "a\r\nb" + 4) == "a\r\nb" + 3);

	rigor_srand(0x3d4e5f60u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill_printable(buf, len);
		for (size_t i = 0; i < len; ++i)
			if ((rigor_rand() & 7u) == 0)
				buf[i] = '\n';
		for (size_t a = 0; a <= len; ++a) {
			for (size_t b = a; b <= len; ++b)
				RCHECK(jstr_linestart(buf + a, buf + b) ==
				       ref_linestart(buf + a, buf + b));
		}
	}

	return 0;
}
