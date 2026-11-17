/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static int
ref_lower(int c)
{
	c = (unsigned char)c;
	return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
}

static char *
ref_strncasechr(const char *s, int c, size_t n)
{
	const int alpha = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
	for (size_t i = 0; i < n; ++i) {
		const unsigned char b = (unsigned char)s[i];
		if (b == 0)
			break;
		if (alpha ? ref_lower(b) == ref_lower(c) : b == (unsigned char)c)
			return (char *)(s + i);
	}
	return NULL;
}

int
main(void)
{
	char buf[1100];

	RCHECK(jstr_strncasechr("aBac", 'B', 4) == (char *)"aBac" + 1);
	RCHECK(jstr_strncasechr("aBac", 'b', 2) == (char *)"aBac" + 1);
	RCHECK(jstr_strncasechr("aBac", 'b', 1) == NULL);
	RCHECK(jstr_strncasechr("aBac", 'z', 4) == NULL);
	RCHECK(jstr_strncasechr("aBac", '1', 4) == NULL);
	RCHECK(jstr_strncasechr("a1B", '1', 3) == (char *)"a1B" + 1);
	RCHECK(jstr_strncasechr("", 'a', 0) == NULL);
	/* Regression: the NUL terminator is not part of the searched span,
	 * matching jstr_strnchr()/the generated wrapper. */
	RCHECK(jstr_strncasechr("abc", '\0', 3) == NULL);
	RCHECK(jstr_strncasechr("abc", '\0', 4) == NULL);
	RCHECK(jstr_strncasechr("abc", 0xff, 3) == NULL);
	RCHECK(jstr_strncasechr("abcdef", 'z', 7) == NULL);
	RCHECK(jstr_strncasechr("abcdef", 'Z', 7) == NULL);

	for (size_t len = 0; len <= 40; ++len) {
		for (int c = 0; c < 256; ++c) {
			memset(buf, (unsigned char)(c ^ 0x5a), len);
			buf[len] = 0;
			const size_t pos = len ? (size_t)c % (len + 1) : 0;
			if (pos < len)
				buf[pos] = (unsigned char)c;
			for (size_t n = 0; n <= len + 2; ++n)
				RCHECK(jstr_strncasechr(buf, c, n) == ref_strncasechr(buf, c, n));
		}
	}

	rigor_srand(0x778899aau);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill_printable(buf, len);
		for (int c = 0; c < 256; ++c)
			for (size_t n = 0; n <= len + 2; ++n)
				RCHECK(jstr_strncasechr(buf, c, n) == ref_strncasechr(buf, c, n));
	}

	return 0;
}
