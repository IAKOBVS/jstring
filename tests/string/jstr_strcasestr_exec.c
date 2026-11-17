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
ref_casestr(const char *h, size_t hl, const char *n, size_t nl)
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

	jstr_strcasestr_comp(&t, "");
	RCHECK(jstr_strcasestr_exec(&t, "abc", "") == (char *)"abc");
	jstr_strcasestr_comp(&t, "a");
	RCHECK(jstr_strcasestr_exec(&t, "xxAxx", "a") == (char *)"xxAxx" + 2);
	RCHECK(jstr_strcasestr_exec(&t, "xxxxx", "a") == NULL);
	jstr_strcasestr_comp(&t, "ab");
	RCHECK(jstr_strcasestr_exec(&t, "xxABxx", "ab") == (char *)"xxABxx" + 2);
	RCHECK(jstr_strcasestr_exec(&t, "xxxxxx", "ab") == NULL);
	jstr_strcasestr_comp(&t, "abc");
	RCHECK(jstr_strcasestr_exec(&t, "xxABCxx", "abc") == (char *)"xxABCxx" + 2);
	RCHECK(jstr_strcasestr_exec(&t, "ABcabc", "abc") == (char *)"ABcabc");
	{
		char longn[97];
		char longh[220];
		memset(longn, 'a', 95);
		longn[95] = 'B';
		longn[96] = 0;
		memset(longh, 'x', 110);
		memcpy(longh + 100, longn, 96);
		memset(longh + 196, 'x', 20);
		longh[216] = 0;
		jstr_strcasestr_comp(&t, longn);
		RCHECK(jstr_strcasestr_exec(&t, longh, longn) == longh + 100);
		longh[150] = 0;
		RCHECK(jstr_strcasestr_exec(&t, longh, longn) == NULL);
	}

	rigor_srand(0x5eed9abcu);
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
			jstr_strcasestr_comp(&t, n);
			RCHECK(jstr_strcasestr_exec(&t, h, n) == ref_casestr(h, hl, n, nl));
		}
	}

	return 0;
}
