/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static int
ref_lower(int c)
{
	c = (unsigned char)c;
	return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
}

static int
ref_endscase(const char *hs, const char *ne)
{
	const size_t hl = strlen(hs), nl = strlen(ne);
	if (hl < nl)
		return 0;
	for (size_t i = 0; i < nl; ++i)
		if (ref_lower(hs[hl - nl + i]) != ref_lower(ne[i]))
			return 0;
	return 1;
}

int
main(void)
{
	char h[1100], n[1100];

	RCHECK(jstr_endscase("hello world", "WORLD"));
	RCHECK(jstr_endscase("hello world", "world"));
	RCHECK(!jstr_endscase("hello world", "worldx"));
	RCHECK(!jstr_endscase("hello world", "hello"));
	RCHECK(jstr_endscase("hello", ""));
	RCHECK(jstr_endscase("", ""));
	RCHECK(!jstr_endscase("", "a"));
	RCHECK(jstr_endscase("abc", "bc"));
	RCHECK(jstr_endscase("abc", "ABC"));
	RCHECK(jstr_endscase("abc", "abc"));
	RCHECK(!jstr_endscase("abc", "ab"));
	RCHECK(jstr_endscase("a1B", "1b"));

	rigor_srand(0xf00dbabu);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t hl = rigor_lens[li];
		if (!JSTR_TEST_SLOW && hl > 128)
			continue;
		rigor_fill_printable(h, hl);
		for (size_t ni = 0; ni < RIGOR_NLENS; ++ni) {
			const size_t nl = rigor_lens[ni];
			if (!JSTR_TEST_SLOW && nl > 128)
				continue;
			if (nl <= hl)
				memcpy(n, h + hl - nl, nl);
			else
				rigor_fill_printable(n, nl);
			n[nl] = 0;
			RCHECK_EQ(jstr_endscase(h, n), ref_endscase(h, n));
			if (nl) {
				n[0] = (unsigned char)(n[0] == 'z' ? 'y' : 'z');
				RCHECK_EQ(jstr_endscase(h, n), ref_endscase(h, n));
			}
		}
	}

	return 0;
}
