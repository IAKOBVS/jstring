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

static void *
ref_memrmem(const void *hs, size_t hl, const void *ne, size_t nl)
{
	const unsigned char *h = (const unsigned char *)hs;
	const unsigned char *n = (const unsigned char *)ne;
	if (nl == 0)
		return (void *)(h + hl);
	if (hl < nl)
		return NULL;
	for (size_t i = hl - nl + 1; i > 0; --i)
		if (memcmp(h + i - 1, n, nl) == 0)
			return (void *)(h + i - 1);
	return NULL;
}

int
main(void)
{
	unsigned char h[1200], n[1200];

	RCHECK(jstr_memrmem("abXabY", 6, "ab", 2) == (void *)"abXabY" + 3);
	RCHECK(jstr_memrmem("ababab", 6, "ab", 2) == (void *)"ababab" + 4);
	RCHECK(jstr_memrmem("aaaa", 4, "aa", 2) == (void *)"aaaa" + 2);
	RCHECK(jstr_memrmem("abcd", 4, "xyz", 3) == NULL);
	RCHECK(jstr_memrmem("abcd", 4, "abcde", 5) == NULL);
	RCHECK(jstr_memrmem("abcd", 4, "", 0) == (void *)"abcd" + 4);
	RCHECK(jstr_memrmem("", 0, "", 0) == (void *)"");
	RCHECK(jstr_memrmem("abcabc", 6, "abc", 3) == (void *)"abcabc" + 3);
	RCHECK(jstr_memrmem("a", 1, "a", 1) == (void *)"a");
	{
		const char bin[7] = { 'a', 0, 'b', 'a', 0, 'b', 'c' };
		RCHECK(jstr_memrmem(bin, 7, "a\0b", 3) == bin + 3);
		RCHECK(jstr_memrmem(bin, 7, "\0b", 2) == bin + 4);
	}

	rigor_srand(0xabad1deau);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t hl = rigor_lens[li];
		if (!JSTR_TEST_SLOW && hl > 128)
			continue;
		rigor_fill(h, hl, 1);
		for (size_t k = 0; k < sizeof(needle_lens) / sizeof(needle_lens[0]); ++k) {
			const size_t nl = needle_lens[k];
			rigor_fill(n, nl, 1);
			if (nl && nl <= hl)
				memcpy(n, h + (hl - nl) / 2, nl);
			RCHECK(jstr_memrmem(h, hl, n, nl) == ref_memrmem(h, hl, n, nl));
		}
	}

	return 0;
}
