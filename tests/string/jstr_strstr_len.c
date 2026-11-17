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
ref_strstr_len(const char *h, size_t hl, const char *n, size_t nl)
{
	if (nl == 0)
		return (char *)h;
	if (hl < nl)
		return NULL;
	for (size_t i = 0; i + nl <= hl; ++i)
		if (memcmp(h + i, n, nl) == 0)
			return (char *)(h + i);
	return NULL;
}

int
main(void)
{
	char h[1200], n[1200];

	RCHECK(jstr_strstr_len("hello world", 11, "wo", 2) == (char *)"hello world" + 6);
	RCHECK(jstr_strstr_len("hello", 5, "xyz", 3) == NULL);
	RCHECK(jstr_strstr_len("hello", 5, "", 0) == (char *)"hello");
	RCHECK(jstr_strstr_len("", 0, "", 0) == (char *)"");
	RCHECK(jstr_strstr_len("", 0, "a", 1) == NULL);
	RCHECK(jstr_strstr_len("abcabc", 6, "abc", 3) == (char *)"abcabc");
	RCHECK(jstr_strstr_len("xabcabc", 7, "abc", 3) == (char *)"xabcabc" + 1);
	RCHECK(jstr_strstr_len("ababab", 6, "ab", 2) == (char *)"ababab");
	RCHECK(jstr_strstr_len("hello", 4, "o", 1) == NULL);
	RCHECK(jstr_strstr_len("hello", 5, "o", 1) == (char *)"hello" + 4);

	rigor_srand(0x0b0b0b0bu);
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
			RCHECK(jstr_strstr_len(h, hl, n, nl) == ref_strstr_len(h, hl, n, nl));
		}
	}

	return 0;
}
