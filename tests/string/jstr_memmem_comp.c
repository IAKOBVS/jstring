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
	jstr_twoway_ty t;

	jstr_memmem_comp(&t, "", 0);
	RCHECK(jstr_memmem_exec(&t, "abc", 3, "", 0) == (void *)"abc");
	jstr_memmem_comp(&t, "ab", 2);
	RCHECK(jstr_memmem_exec(&t, "xxabxx", 6, "ab", 2) == (void *)"xxabxx" + 2);
	RCHECK(jstr_memmem_exec(&t, "xxxxxx", 6, "ab", 2) == NULL);
	RCHECK(jstr_memmem_exec(&t, "ab", 1, "ab", 2) == NULL);
	jstr_memmem_comp(&t, "abc", 3);
	RCHECK(jstr_memmem_exec(&t, "abcabc", 6, "abc", 3) == (void *)"abcabc");
	RCHECK(jstr_memmem_exec(&t, "xabcabc", 7, "abc", 3) == (void *)"xabcabc" + 1);
	{
		const char bin[8] = { 'a', 0, 'b', 'a', 0, 'b', 'c', 0 };
		jstr_memmem_comp(&t, "a\0b", 3);
		RCHECK(jstr_memmem_exec(&t, bin, 8, "a\0b", 3) == bin);
		RCHECK(jstr_memmem_exec(&t, bin + 1, 7, "a\0b", 3) == bin + 3);
	}
	{
		char longn[96];
		char longh[220];
		memset(longn, 'a', 95);
		longn[95] = 'b';
		memset(longh, 'x', 110);
		memcpy(longh + 100, longn, 96);
		memset(longh + 196, 'x', 20);
		jstr_memmem_comp(&t, longn, 96);
		RCHECK(jstr_memmem_exec(&t, longh, 216, longn, 96) == longh + 100);
	}

	rigor_srand(0xfeedfaceu);
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
			jstr_memmem_comp(&t, n, nl);
			RCHECK(jstr_memmem_exec(&t, h, hl, n, nl) == ref_memmem(h, hl, n, nl));
			if (nl) {
				jstr_memmem_comp(&t, n, nl);
				RCHECK(jstr_memmem_exec(&t, h, nl - 1, n, nl) == NULL);
			}
		}
	}

	return 0;
}
