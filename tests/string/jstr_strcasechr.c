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
ref_strcasechr(const char *s, int c)
{
	if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')))
		return strchr(s, c);
	for (; *s; ++s)
		if (ref_lower(*s) == ref_lower(c))
			return (char *)s;
	return NULL;
}

int
main(void)
{
	char buf[1100];

	RCHECK(jstr_strcasechr("aBc", 'b') == (char *)"aBc" + 1);
	RCHECK(jstr_strcasechr("aBc", 'C') == (char *)"aBc" + 2);
	RCHECK(jstr_strcasechr("aBc", 'A') == (char *)"aBc");
	RCHECK(jstr_strcasechr("abc", 'Z') == NULL);
	RCHECK(jstr_strcasechr("abc", '1') == NULL);
	RCHECK(jstr_strcasechr("a1b", '1') == (char *)"a1b" + 1);
	RCHECK(jstr_strcasechr("", 'a') == NULL);
	RCHECK(jstr_strcasechr("", '\0') == (char *)"");
	RCHECK(jstr_strcasechr("abc", '\0') == (char *)"abc" + 3);
	RCHECK(jstr_strcasechr("ABC", 0x80) == NULL);

	for (int c = 0; c < 256; ++c) {
		for (int i = 0; i < 8; ++i) {
			memset(buf, 'a' + (i * 3) % 26, 6);
			buf[i] = (char)c;
			buf[6] = 0;
			RCHECK(jstr_strcasechr(buf, c) == ref_strcasechr(buf, c));
		}
	}

	rigor_srand(0xa5a5a5a5u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill_printable(buf, len);
		for (int c = 0; c < 256; ++c)
			RCHECK(jstr_strcasechr(buf, c) == ref_strcasechr(buf, c));
		/* Mixed case: upper-case the whole buffer and retry letters. */
		for (size_t i = 0; i < len; ++i)
			if (buf[i] >= 'a' && buf[i] <= 'z')
				buf[i] = (char)(buf[i] - 'a' + 'A');
		for (int c = 'a'; c <= 'z'; ++c)
			RCHECK(jstr_strcasechr(buf, c) == ref_strcasechr(buf, c));
	}

	return 0;
}
