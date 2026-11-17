/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static char *
ref_strchrinv(const char *s, int c)
{
	if ((unsigned char)c == 0)
		return *s ? (char *)s : NULL;
	for (;; ++s)
		if ((unsigned char)*s != (unsigned char)c)
			return (char *)s;
}

int
main(void)
{
	char buf[1100];

	RCHECK(jstr_strchrinv("aaaabc", 'a') == (char *)"aaaabc" + 4);
	RCHECK(jstr_strchrinv("b", 'a') == (char *)"b");
	RCHECK(jstr_strchrinv("", 'a') == (char *)"");
	RCHECK(jstr_strchrinv("aaaa", 'a') == (char *)"aaaa" + 4);
	RCHECK(jstr_strchrinv("abc", 0) == (char *)"abc");
	RCHECK(jstr_strchrinv("", 0) == NULL);
	RCHECK(jstr_strchrinv("aaa", 'a' + 256) == (char *)"aaa" + 3);

	/* c != 0: exactly the first position whose byte differs, else the NUL. */
	for (size_t len = 0; len <= 40; ++len) {
		for (int c = 1; c < 256; ++c) {
			memset(buf, (unsigned char)c, len);
			const size_t pos = len ? (size_t)(c - 1) % len : 0;
			if (pos < len)
				buf[pos] = (unsigned char)(c == 1 ? 2 : 1);
			buf[len] = 0;
			char *got = jstr_strchrinv(buf, c);
			RCHECK(got == ref_strchrinv(buf, c));
			RCHECK(got == buf + (pos < len ? pos : len));
		}
	}

	rigor_srand(0x2468ace0u);
	for (size_t i = 0; i < RIGOR_NLENS; ++i) {
		const size_t len = rigor_lens[i];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill_printable(buf, len);
		for (int c = 0; c < 256; ++c)
			RCHECK(jstr_strchrinv(buf, c) == ref_strchrinv(buf, c));
	}

	return 0;
}
