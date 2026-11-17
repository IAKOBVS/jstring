/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Regression coverage: jstr_strrpbrk() returns the last matching byte of a
 * NUL-terminated string, or NULL. See jstr_strrpbrk_len.c. */

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

static char *
ref_strrpbrk(const char *s, const char *accept)
{
	const size_t n = strlen(s);
	if (!accept[0])
		return NULL;
	for (size_t i = n; i > 0; --i)
		if (ref_in((unsigned char)s[i - 1], accept))
			return (char *)(s + i - 1);
	return NULL;
}

int
main(void)
{
	static const char *const sets[] = { "", "a", "ab", "c", "xy", "/", "\x01\x7f\x80\xff" };
	const char *s;
	char buf[300];

	RCHECK(jstr_strrpbrk("", "a") == NULL);
	RCHECK(jstr_strrpbrk("abc", "") == NULL);
	RCHECK(jstr_strrpbrk("abc", "z") == NULL);

	s = "aab";
	RCHECK(jstr_strrpbrk(s, "a") == s + 1);
	s = "abcba";
	RCHECK(jstr_strrpbrk(s, "a") == s + 4);
	RCHECK(jstr_strrpbrk(s, "c") == s + 2);
	s = "hello world";
	RCHECK(jstr_strrpbrk(s, " ") == s + 5);
	s = "a1b2c3d";
	RCHECK(jstr_strrpbrk(s, "2") == s + 3);
	s = "/usr/local";
	RCHECK(jstr_strrpbrk(s, "/") == s + 4);

	/* Long inputs: the last matching byte, not the mirrored offset. */
	for (size_t len = 3; len <= 128; ++len) {
		memset(buf, 'q', len);
		buf[len] = '\0';
		buf[1] = 'Z';
		buf[len - 2] = 'Z';
		RCHECK(jstr_strrpbrk(buf, "Z") == buf + len - 2);
	}

	rigor_srand(0x13572468u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		if (len >= sizeof(buf))
			continue;
		rigor_fill_printable(buf, len);
		for (size_t si = 0; si < sizeof(sets) / sizeof(sets[0]); ++si)
			RCHECK(jstr_strrpbrk(buf, sets[si]) == ref_strrpbrk(buf, sets[si]));
	}

	return 0;
}
