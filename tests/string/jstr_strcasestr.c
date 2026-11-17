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

	RCHECK(jstr_strcasestr("HeLLo World", "world") == (char *)"HeLLo World" + 6);
	RCHECK(jstr_strcasestr("HeLLo World", "HELLO") == (char *)"HeLLo World");
	RCHECK(jstr_strcasestr("abc", "xyz") == NULL);
	RCHECK(jstr_strcasestr("abc", "") == (char *)"abc");
	RCHECK(jstr_strcasestr("", "") == (char *)"");
	RCHECK(jstr_strcasestr("", "a") == NULL);
	RCHECK(jstr_strcasestr("abc", "ABCD") == NULL);
	RCHECK(jstr_strcasestr("123ABC456", "abc") == (char *)"123ABC456" + 3);
	RCHECK(jstr_strcasestr("aaaAAAaaa", "AaA") == (char *)"aaaAAAaaa");
	RCHECK(jstr_strcasestr("x", "X") == (char *)"x");

	rigor_srand(0x5eed1234u);
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
			RCHECK(jstr_strcasestr(h, n) == ref_casestr(h, hl, n, nl));
		}
	}

	return 0;
}
