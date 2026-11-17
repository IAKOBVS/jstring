/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static size_t
ref_count(const char *s, const char *find)
{
	const size_t fl = strlen(find);
	const size_t sl = strlen(s);
	if (fl == 0)
		return 0;
	size_t cnt = 0, i = 0;
	while (i + fl <= sl) {
		if (memcmp(s + i, find, fl) == 0) {
			++cnt;
			i += fl;
		} else {
			++i;
		}
	}
	return cnt;
}

int
main(void)
{
	char h[1100], n[1100];

	RCHECK(jstr_count("hello", "") == 0);
	RCHECK(jstr_count("", "abc") == 0);
	RCHECK(jstr_count("hello", "l") == 2);
	RCHECK(jstr_count("hello", "ll") == 1);
	RCHECK(jstr_count("aaaa", "aa") == 2);
	RCHECK(jstr_count("aaaaa", "aa") == 2);
	RCHECK(jstr_count("aaa", "aa") == 1);
	RCHECK(jstr_count("abcabcabc", "abc") == 3);
	RCHECK(jstr_count("abcabcabc", "abcd") == 0);
	RCHECK(jstr_count("aaaa", "a") == 4);
	RCHECK(jstr_count("abab", "ba") == 1);
	RCHECK(jstr_count("x", "x") == 1);

	rigor_srand(0x1a2b3c4du);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t hl = rigor_lens[li];
		if (!JSTR_TEST_SLOW && hl > 128)
			continue;
		rigor_fill_printable(h, hl);
		for (size_t ni = 0; ni < RIGOR_NLENS; ++ni) {
			const size_t nl = rigor_lens[ni];
			if (!JSTR_TEST_SLOW && nl > 128)
				continue;
			rigor_fill_printable(n, nl);
			/* Ensure the needle actually occurs on some iterations. */
			if (nl && nl <= hl)
				memcpy(n, h + (hl - nl) / 2, nl);
			n[nl] = 0;
			RCHECK(jstr_count(h, n) == ref_count(h, n));
		}
		RCHECK(jstr_count(h, h) == (hl ? 1 : 0));
	}

	return 0;
}
