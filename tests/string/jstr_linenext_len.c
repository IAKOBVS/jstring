/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static const char *
ref_linenext_len(const char *start, const char *end)
{
	const char *p = start;
	while (p < end && *p != '\n')
		++p;
	if (p == end)
		return NULL;
	return p[1] ? p + 1 : NULL;
}

int
main(void)
{
	const char s[] = "abc\ndef\nghi";
	char buf[1100];

	RCHECK(jstr_linenext_len(s, s + 11) == s + 4);
	RCHECK(jstr_linenext_len(s + 4, s + 11) == s + 8);
	RCHECK(jstr_linenext_len(s + 8, s + 11) == NULL);
	RCHECK(jstr_linenext_len(s, s + 3) == NULL);
	RCHECK(jstr_linenext_len(s, s) == NULL);
	RCHECK(jstr_linenext_len(s + 1, s + 2) == NULL);
	RCHECK(jstr_linenext_len("abc\n", "abc\n" + 4) == NULL);
	RCHECK(jstr_linenext_len("\nabc", "\nabc" + 4) == "\nabc" + 1);
	RCHECK(jstr_linenext_len("a\n\nb", "a\n\nb" + 4) == "a\n\nb" + 2);
	RCHECK(jstr_linenext_len("a\n\nb" + 2, "a\n\nb" + 4) == "a\n\nb" + 3);
	RCHECK(jstr_linenext_len("a\r\nb", "a\r\nb" + 4) == "a\r\nb" + 3);
	RCHECK(jstr_linenext_len("abc", "abc" + 3) == NULL);
	RCHECK(jstr_linenext_len("", "") == NULL);

	rigor_srand(0x0a0b0c0du);
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
				RCHECK(jstr_linenext_len(buf + a, buf + b) ==
				       ref_linenext_len(buf + a, buf + b));
		}
	}

	return 0;
}
