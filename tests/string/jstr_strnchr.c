/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* The generated header provides a static-inline jstr_strnchr() wrapper that
 * delegates to jstr_strnchr_len(s, c, strlen(s), n), so the searched span is
 * min(strlen(s), n) and the terminating NUL is never matched.  The library
 * symbol now implements the same contract (previously it could return the
 * NUL pointer for n > strlen). */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static char *
ref_strnchr(const char *s, int c, size_t n)
{
	size_t lim = strlen(s);
	if (n < lim)
		lim = n;
	for (size_t i = 0; i < lim; ++i)
		if ((unsigned char)s[i] == (unsigned char)c)
			return (char *)(s + i);
	return NULL;
}

int
main(void)
{
	char buf[1100];

	RCHECK(jstr_strnchr("abac", 'a', 4) == (char *)"abac");
	RCHECK(jstr_strnchr("abac", 'a', 1) == (char *)"abac");
	RCHECK(jstr_strnchr("abac", 'a', 0) == NULL);
	RCHECK(jstr_strnchr("abac", 'c', 3) == NULL);
	RCHECK(jstr_strnchr("abac", 'c', 4) == (char *)"abac" + 3);
	RCHECK(jstr_strnchr("abac", 'z', 4) == NULL);
	RCHECK(jstr_strnchr("abac", 'z', 99) == NULL);
	RCHECK(jstr_strnchr("", 'z', 0) == NULL);
	RCHECK(jstr_strnchr("abc", 'z', 99) == NULL);
	RCHECK(jstr_strnchr("a1B", '1', 3) == (char *)"a1B" + 1);
	/* Wrapper never matches the NUL terminator (documented strchr-like
	 * behaviour would return it for n > strlen). */
	RCHECK(jstr_strnchr("abc", 0, 4) == NULL);
	RCHECK(jstr_strnchr("", 0, 1) == NULL);

	for (size_t len = 0; len <= 40; ++len) {
		for (int c = 0; c < 256; ++c) {
			unsigned char filler = (unsigned char)(c ^ 0x5a);
			if (filler == 0)
				filler = 0x5b;
			memset(buf, filler, len);
			buf[len] = 0;
			const size_t pos = len ? (size_t)c % (len + 1) : 0;
			if (pos < len)
				buf[pos] = (unsigned char)c;
			for (size_t n = 0; n <= len + 2; ++n)
				RCHECK(jstr_strnchr(buf, c, n) == ref_strnchr(buf, c, n));
			if (c != 0)
				RCHECK(jstr_strnchr(buf, c, len + 1) ==
				       (pos < len ? buf + pos : NULL));
		}
	}

	rigor_srand(0x33445566u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill_printable(buf, len);
		for (int c = 0; c < 256; ++c)
			for (size_t n = 0; n <= len + 2; ++n)
				RCHECK(jstr_strnchr(buf, c, n) == ref_strnchr(buf, c, n));
	}

	return 0;
}
