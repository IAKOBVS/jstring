/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* jstr_strrchrinv() returns the last byte not equal to c, or NULL when every
 * byte matches; matches jstr_memrchrinv(). */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static char *
ref_strrchrinv(const char *s, int c)
{
	const unsigned char *p = (const unsigned char *)s;
	size_t n = strlen(s);
	for (; n > 0; --n)
		if (p[n - 1] == (unsigned char)c)
			return (char *)(p + n - 1);
	return NULL;
}

int
main(void)
{
	char buf[1100];

	RCHECK(jstr_strrchrinv("hello", 'o') == (char *)"hello" + 4);
	RCHECK(jstr_strrchrinv("hello", 'l') == (char *)"hello" + 3);
	RCHECK(jstr_strrchrinv("hello", 'h') == (char *)"hello");
	RCHECK(jstr_strrchrinv("hello", 'x') == NULL);
	RCHECK(jstr_strrchrinv("", 'x') == NULL);
	RCHECK(jstr_strrchrinv("aaa", 0) == NULL);
	RCHECK(jstr_strrchrinv("aXaXa", 'X') == (char *)"aXaXa" + 3);

	for (size_t len = 0; len <= 40; ++len) {
		for (int c = 1; c < 256; ++c) {
			unsigned char filler = (unsigned char)(c ^ 0x5a);
			if (filler == 0)
				filler = 0x5b;
			memset(buf, filler, len + 1);
			const size_t pos = len ? (size_t)(c - 1) % len : 0;
			if (pos < len)
				buf[pos] = (unsigned char)c;
			buf[len] = 0;
			char *got = jstr_strrchrinv(buf, c);
			RCHECK(got == ref_strrchrinv(buf, c));
			RCHECK(got == (pos < len ? buf + pos : NULL));
		}
	}

	rigor_srand(0x0badf00du);
	for (size_t i = 0; i < RIGOR_NLENS; ++i) {
		const size_t len = rigor_lens[i];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill_printable(buf, len);
		for (int c = 0; c < 256; ++c)
			RCHECK(jstr_strrchrinv(buf, c) == ref_strrchrinv(buf, c));
	}

	return 0;
}
