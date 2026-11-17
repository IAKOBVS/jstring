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
ref_strstr(const char *h, size_t hl, const char *n, size_t nl)
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
	jstr_twoway_ty t;

	jstr_strstr_comp(&t, "");
	RCHECK(jstr_strstr_exec(&t, "abc", "") == (char *)"abc");
	jstr_strstr_comp(&t, "ab");
	RCHECK(jstr_strstr_exec(&t, "xxabxx", "ab") == (char *)"xxabxx" + 2);
	RCHECK(jstr_strstr_exec(&t, "xxxxxx", "ab") == NULL);
	jstr_strstr_comp(&t, "1");
	RCHECK(jstr_strstr_exec(&t, "a1b1", "1") == (char *)"a1b1" + 1);
	jstr_strstr_comp(&t, "abc");
	RCHECK(jstr_strstr_exec(&t, "abcabc", "abc") == (char *)"abcabc");
	RCHECK(jstr_strstr_exec(&t, "xabcabc", "abc") == (char *)"xabcabc" + 1);
	{
		char longn[97];
		char longh[220];
		memset(longn, 'a', 95);
		longn[95] = 'b';
		longn[96] = 0;
		memset(longh, 'x', 110);
		memcpy(longh + 100, longn, 96);
		memset(longh + 196, 'x', 20);
		longh[216] = 0;
		jstr_strstr_comp(&t, longn);
		RCHECK(jstr_strstr_exec(&t, longh, longn) == longh + 100);
		longh[150] = 0;
		RCHECK(jstr_strstr_exec(&t, longh, longn) == NULL);
	}

	rigor_srand(0x51ee7u);
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
			jstr_strstr_comp(&t, n);
			RCHECK(jstr_strstr_exec(&t, h, n) == ref_strstr(h, hl, n, nl));
		}
	}

	return 0;
}
