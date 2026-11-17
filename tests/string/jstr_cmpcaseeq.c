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
ref_cmpcaseeq(const char *a, size_t al, const char *b, size_t bl)
{
	if (al != bl)
		return 1;
	for (size_t i = 0; i < al; ++i)
		if (ref_lower(a[i]) != ref_lower(b[i]))
			return 1;
	return 0;
}

int
main(void)
{
	char a[1100], b[1100];

	RCHECK_EQ(jstr_cmpcaseeq("", 0, "", 0), 0);
	RCHECK(jstr_cmpcaseeq("abc", 3, "ABC", 3) == 0);
	RCHECK(jstr_cmpcaseeq("AbC", 3, "aBc", 3) == 0);
	RCHECK(jstr_cmpcaseeq("abc", 3, "abd", 3) != 0);
	RCHECK(jstr_cmpcaseeq("abc", 3, "ab", 2) != 0);
	RCHECK(jstr_cmpcaseeq("", 0, "a", 1) != 0);
	RCHECK(jstr_cmpcaseeq("a", 1, "", 0) != 0);
	RCHECK(jstr_cmpcaseeq("a1B", 3, "A1b", 3) == 0);
	RCHECK(jstr_cmpcaseeq("a1B", 3, "A2b", 3) != 0);
	RCHECK(jstr_cmpcaseeq("aB", 2, "ab", 1) != 0);

	rigor_srand(0xc001d00du);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill_printable(a, len);
		memcpy(b, a, len);
		b[len] = 0;
		RCHECK_EQ((jstr_cmpcaseeq(a, len, b, len) == 0),
		          (ref_cmpcaseeq(a, len, b, len) == 0));
		/* Same length, one folded byte differs. */
		if (len) {
			const size_t k = len / 2;
			b[k] = (unsigned char)(a[k] == 'z' ? 'y' : 'z');
			RCHECK_EQ((jstr_cmpcaseeq(a, len, b, len) == 0),
			          (ref_cmpcaseeq(a, len, b, len) == 0));
		}
		/* Different lengths never compare equal even with equal prefix. */
		if (len)
			RCHECK(jstr_cmpcaseeq(a, len, a, len - 1) != 0);
	}

	return 0;
}
