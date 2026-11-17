/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static int
lc(int c)
{
	c = (unsigned char)c;
	return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
}

static char *
ref_casestr_len(const char *h, size_t hl, const char *n, size_t nl)
{
	if (nl == 0)
		return (char *)h;
	if (hl < nl)
		return NULL;
	for (size_t i = 0; i + nl <= hl; ++i) {
		size_t j = 0;
		while (j < nl && lc(h[i + j]) == lc(n[j]))
			++j;
		if (j == nl)
			return (char *)(h + i);
	}
	return NULL;
}

int
main(void)
{
	char h[1200], n[1200];
	jstr_twoway_ty t;

	jstr_strcasestr_len_comp(&t, "", 0);
	RCHECK(jstr_strcasestr_len_exec(&t, "abc", 3, "", 0) == (char *)"abc");
	jstr_strcasestr_len_comp(&t, "a", 1);
	RCHECK(jstr_strcasestr_len_exec(&t, "xxAxx", 5, "a", 1) == (char *)"xxAxx" + 2);
	RCHECK(jstr_strcasestr_len_exec(&t, "xxxxx", 5, "a", 1) == NULL);
	jstr_strcasestr_len_comp(&t, "ab", 2);
	RCHECK(jstr_strcasestr_len_exec(&t, "x", 1, "ab", 2) == NULL);
	RCHECK(jstr_strcasestr_len_exec(&t, "xxABxx", 6, "ab", 2) == (char *)"xxABxx" + 2);
	jstr_strcasestr_len_comp(&t, "abc", 3);
	RCHECK(jstr_strcasestr_len_exec(&t, "xxABCxx", 7, "abc", 3) == (char *)"xxABCxx" + 2);
	RCHECK(jstr_strcasestr_len_exec(&t, "xxABCxx", 4, "abc", 3) == NULL);
	RCHECK(jstr_strcasestr_len_exec(&t, "ABcabc", 6, "abc", 3) == (char *)"ABcabc");
	{
		char longn[97];
		char longh[220];
		memset(longn, 'a', 95);
		longn[95] = 'B';
		longn[96] = 0;
		memset(longh, 'x', 110);
		memcpy(longh + 100, longn, 96);
		memset(longh + 196, 'x', 20);
		jstr_strcasestr_len_comp(&t, longn, 96);
		RCHECK(jstr_strcasestr_len_exec(&t, longh, 216, longn, 96) == longh + 100);
		RCHECK(jstr_strcasestr_len_exec(&t, longh, 150, longn, 96) == NULL);
	}

	rigor_srand(0x5eeddef0u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t hl = rigor_lens[li];
		if (!JSTR_TEST_SLOW && hl > 128)
			continue;
		rigor_fill_printable(h, hl);
		for (size_t i = 0; i < hl; ++i)
			if ((rigor_rand() & 1u) && h[i] >= 'a' && h[i] <= 'z')
				h[i] = (char)(h[i] - 'a' + 'A');
		for (size_t k = 0; k < RIGOR_NLENS; ++k) {
			const size_t nl = rigor_lens[k];
			if (nl > 128)
				continue;
			rigor_fill_printable(n, nl);
			if (nl && nl <= hl)
				memcpy(n, h + (hl - nl) / 2, nl);
			jstr_strcasestr_len_comp(&t, n, nl);
			RCHECK(jstr_strcasestr_len_exec(&t, h, hl, n, nl) ==
			       ref_casestr_len(h, hl, n, nl));
			for (size_t lim = 0; lim <= hl + 2; ++lim) {
				const size_t eff = hl < lim ? hl : lim;
				RCHECK(jstr_strcasestr_len_exec(&t, h, eff, n, nl) ==
				       ref_casestr_len(h, eff, n, nl));
			}
		}
	}

	return 0;
}
