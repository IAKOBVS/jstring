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
ref_ncasestr_len(const char *h, size_t hl, const char *n, size_t nl, size_t lim)
{
	if (hl < lim)
		lim = hl;
	if (nl == 0)
		return (char *)h;
	if (lim < nl)
		return NULL;
	for (size_t i = 0; i + nl <= lim; ++i) {
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

	RCHECK(jstr_strncasestr_len("hello WORLD", 11, "world", 5, 11) == (char *)"hello WORLD" + 6);
	RCHECK(jstr_strncasestr_len("hello WORLD", 11, "world", 5, 6) == NULL);
	RCHECK(jstr_strncasestr_len("hello WORLD", 11, "world", 5, 0) == NULL);
	RCHECK(jstr_strncasestr_len("hello WORLD", 11, "", 0, 0) == (char *)"hello WORLD");
	RCHECK(jstr_strncasestr_len("ABc", 3, "abc", 3, 3) == (char *)"ABc");
	RCHECK(jstr_strncasestr_len("ABc", 3, "abc", 3, 2) == NULL);
	RCHECK(jstr_strncasestr_len("xABc", 4, "abc", 3, 3) == NULL);
	RCHECK(jstr_strncasestr_len("xABc", 4, "abc", 3, 4) == (char *)"xABc" + 1);
	RCHECK(jstr_strncasestr_len("", 0, "a", 1, 100) == NULL);

	rigor_srand(0x7e57a11u);
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
			for (size_t lim = 0; lim <= hl + 2; ++lim)
				RCHECK(jstr_strncasestr_len(h, hl, n, nl, lim) ==
				       ref_ncasestr_len(h, hl, n, nl, lim));
		}
	}

	return 0;
}
