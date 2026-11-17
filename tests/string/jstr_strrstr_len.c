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
ref_strrstr_len(const char *hs, size_t hl, const char *ne, size_t nl)
{
	if (nl == 0)
		return (char *)hs + hl;
	if (hl < nl)
		return NULL;
	for (size_t i = hl - nl + 1; i > 0; --i)
		if (memcmp(hs + i - 1, ne, nl) == 0)
			return (char *)(hs + i - 1);
	return NULL;
}

int
main(void)
{
	char h[1200], n[1200];

	RCHECK(jstr_strrstr_len("abXabY", 6, "ab", 2) == (char *)"abXabY" + 3);
	RCHECK(jstr_strrstr_len("ababab", 6, "ab", 2) == (char *)"ababab" + 4);
	RCHECK(jstr_strrstr_len("aaaa", 4, "aa", 2) == (char *)"aaaa" + 2);
	RCHECK(jstr_strrstr_len("abcd", 4, "xyz", 3) == NULL);
	RCHECK(jstr_strrstr_len("abcd", 4, "abcde", 5) == NULL);
	RCHECK(jstr_strrstr_len("abcd", 4, "", 0) == (char *)"abcd" + 4);
	RCHECK(jstr_strrstr_len("", 0, "", 0) == (char *)"");
	RCHECK(jstr_strrstr_len("abcabc", 6, "abc", 3) == (char *)"abcabc" + 3);

	rigor_srand(0x600df00du);
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
			RCHECK(jstr_strrstr_len(h, hl, n, nl) == ref_strrstr_len(h, hl, n, nl));
		}
	}

	return 0;
}
