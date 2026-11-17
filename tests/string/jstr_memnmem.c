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
ref_memmem(const void *hs, size_t hl, const void *ne, size_t nl)
{
	const unsigned char *h = (const unsigned char *)hs;
	const unsigned char *n = (const unsigned char *)ne;
	if (nl == 0)
		return (void *)h;
	if (hl < nl)
		return NULL;
	for (size_t i = 0; i + nl <= hl; ++i)
		if (memcmp(h + i, n, nl) == 0)
			return (void *)(h + i);
	return NULL;
}

int
main(void)
{
	unsigned char h[1200], n[1200];

	RCHECK(jstr_memnmem("hello world", 11, "wo", 2, 8) == (void *)"hello world" + 6);
	RCHECK(jstr_memnmem("hello world", 11, "wo", 2, 4) == NULL);
	RCHECK(jstr_memnmem("hello world", 11, "wo", 2, 0) == NULL);
	RCHECK(jstr_memnmem("hello world", 11, "", 0, 0) == (void *)"hello world");
	RCHECK(jstr_memnmem("hello world", 11, "", 0, 99) == (void *)"hello world");
	RCHECK(jstr_memnmem("abcdef", 6, "cd", 2, 3) == NULL);
	RCHECK(jstr_memnmem("abcdef", 6, "cd", 2, 4) == (void *)"abcdef" + 2);
	RCHECK(jstr_memnmem("abcdef", 6, "f", 1, 6) == (void *)"abcdef" + 5);
	RCHECK(jstr_memnmem("abcdef", 6, "f", 1, 5) == NULL);
	RCHECK(jstr_memnmem("abcdef", 0, "a", 1, 100) == NULL);
	{
		const char bin[6] = { 'a', 0, 'b', 'c', 0, 'd' };
		RCHECK(jstr_memnmem(bin, 3, "c", 1, 6) == NULL);
		RCHECK(jstr_memnmem(bin, 6, "c", 1, 6) == bin + 3);
	}

	rigor_srand(0x00c0ffeeu);
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
			for (size_t lim = 0; lim <= hl + 2; ++lim) {
				const size_t eff = hl < lim ? hl : lim;
				RCHECK(jstr_memnmem(h, hl, n, nl, lim) == ref_memmem(h, eff, n, nl));
			}
		}
	}

	return 0;
}
