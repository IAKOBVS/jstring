/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Regression coverage: jstr_strrpbrk_len() returns the LAST byte matching
 * accept, or NULL. Previously it computed s + sz - rcspn, which returned a
 * mirrored offset (often not in accept) or s + sz on no match. */

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
ref_strrpbrk(const char *s, const char *accept, size_t n)
{
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
	unsigned char buf[1200];

	RCHECK(jstr_strrpbrk_len("", "a", 0) == NULL);
	RCHECK(jstr_strrpbrk_len("abc", "", 3) == NULL);
	RCHECK(jstr_strrpbrk_len("abc", "", 0) == NULL);
	RCHECK(jstr_strrpbrk_len("abc", "z", 3) == NULL);

	s = "aab";
	RCHECK(jstr_strrpbrk_len(s, "a", 3) == s + 1);
	s = "abcba";
	RCHECK(jstr_strrpbrk_len(s, "a", 5) == s + 4);
	RCHECK(jstr_strrpbrk_len(s, "c", 5) == s + 2);
	s = "/usr/local";
	RCHECK(jstr_strrpbrk_len(s, "/", 10) == s + 4);
	s = "hello world";
	RCHECK(jstr_strrpbrk_len(s, " ", 11) == s + 5);
	s = "xxyzX";
	RCHECK(jstr_strrpbrk_len(s, "xy", 5) == s + 2);
	s = "abZcd";
	RCHECK(jstr_strrpbrk_len(s, "baZ", 5) == s + 2);

	/* Truncated span: only bytes inside [0, sz) participate. */
	s = "abcdef";
	RCHECK(jstr_strrpbrk_len(s, "b", 3) == s + 1);
	RCHECK(jstr_strrpbrk_len(s, "e", 4) == NULL);
	RCHECK(jstr_strrpbrk_len(s, "d", 4) == s + 3);

	/* Differential over all lengths and accept sets. */
	rigor_srand(0x2468ace0u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill(buf, len + 2, 1);
		for (size_t n = 0; n <= len + 2; ++n)
			for (size_t si = 0; si < sizeof(sets) / sizeof(sets[0]); ++si)
				RCHECK(jstr_strrpbrk_len((const char *)buf, sets[si], n) ==
				       ref_strrpbrk((const char *)buf, sets[si], n));
	}

	return 0;
}
