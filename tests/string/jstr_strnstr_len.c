/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static const size_t needle_lens[] = { 0, 1, 2, 3, 4, 7, 8, 9, 15, 16, 17, 23, 24,
                                      31, 32, 33, 47, 48, 63, 64, 65, 80, 96, 127, 128 };

static char *
ref_strnstr_len(const char *h, size_t hl, const char *n, size_t nl, size_t lim)
{
	if (hl < lim)
		lim = hl;
	if (nl == 0)
		return (char *)h;
	if (lim < nl)
		return NULL;
	for (size_t i = 0; i + nl <= lim; ++i)
		if (memcmp(h + i, n, nl) == 0)
			return (char *)(h + i);
	return NULL;
}

int
main(void)
{
	char h[1200], n[1200];

	RCHECK(jstr_strnstr_len("hello world", 11, "wo", 2, 8) == (char *)"hello world" + 6);
	RCHECK(jstr_strnstr_len("hello world", 11, "wo", 2, 4) == NULL);
	RCHECK(jstr_strnstr_len("hello world", 11, "wo", 2, 0) == NULL);
	RCHECK(jstr_strnstr_len("hello world", 11, "", 0, 0) == (char *)"hello world");
	RCHECK(jstr_strnstr_len("abcdef", 6, "cd", 2, 3) == NULL);
	RCHECK(jstr_strnstr_len("abcdef", 6, "cd", 2, 4) == (char *)"abcdef" + 2);
	RCHECK(jstr_strnstr_len("abcdef", 6, "f", 1, 5) == NULL);
	RCHECK(jstr_strnstr_len("abcdef", 6, "f", 1, 6) == (char *)"abcdef" + 5);
	RCHECK(jstr_strnstr_len("abc", 3, "abcd", 4, 100) == NULL);

	rigor_srand(0x2e2e2e2eu);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t hl = rigor_lens[li];
		if (!JSTR_TEST_SLOW && hl > 128)
			continue;
		rigor_fill_printable(h, hl);
		for (size_t k = 0; k < sizeof(needle_lens) / sizeof(needle_lens[0]); ++k) {
			const size_t nl = needle_lens[k];
			rigor_fill_printable(n, nl);
			if (nl && nl <= hl)
				memcpy(n, h + (hl - nl) / 2, nl);
			for (size_t lim = 0; lim <= hl + 2; ++lim)
				RCHECK(jstr_strnstr_len(h, hl, n, nl, lim) ==
				       ref_strnstr_len(h, hl, n, nl, lim));
		}
	}

	return 0;
}
