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
ref_strcasechrnul(const char *s, int c)
{
	if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))) {
		const char *p = strchr(s, c);
		return (char *)(p ? p : s + strlen(s));
	}
	for (;; ++s)
		if (ref_lower(*s) == ref_lower(c) || *s == '\0')
			return (char *)s;
}

int
main(void)
{
	char buf[1100];

	RCHECK(jstr_strcasechrnul("aBc", 'b') == (char *)"aBc" + 1);
	RCHECK(jstr_strcasechrnul("aBc", 'C') == (char *)"aBc" + 2);
	RCHECK(jstr_strcasechrnul("abc", 'Z') == (char *)"abc" + 3);
	RCHECK(jstr_strcasechrnul("abc", '1') == (char *)"abc" + 3);
	RCHECK(jstr_strcasechrnul("a1b", '1') == (char *)"a1b" + 1);
	RCHECK(jstr_strcasechrnul("", 'a') == (char *)"");
	RCHECK(jstr_strcasechrnul("", '\0') == (char *)"");
	RCHECK(jstr_strcasechrnul("abc", '\0') == (char *)"abc" + 3);
	RCHECK(jstr_strcasechrnul("XYZ", 0xff) == (char *)"XYZ" + 3);

	for (int c = 0; c < 256; ++c) {
		for (int i = 0; i < 8; ++i) {
			memset(buf, 'A' + (i * 5) % 26, 6);
			buf[i] = (char)c;
			buf[6] = 0;
			RCHECK(jstr_strcasechrnul(buf, c) == ref_strcasechrnul(buf, c));
		}
	}

	rigor_srand(0x5a5a5a5au);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill_printable(buf, len);
		for (int c = 0; c < 256; ++c)
			RCHECK(jstr_strcasechrnul(buf, c) == ref_strcasechrnul(buf, c));
		for (size_t i = 0; i < len; ++i)
			if (buf[i] >= 'a' && buf[i] <= 'z')
				buf[i] = (char)(buf[i] - 'a' + 'A');
		for (int c = 'A'; c <= 'Z'; ++c)
			RCHECK(jstr_strcasechrnul(buf, c) == ref_strcasechrnul(buf, c));
	}

	return 0;
}
