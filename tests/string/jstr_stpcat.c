/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

int
main(void)
{
	char src[1200], dst[2400];

	{
		char b[32] = "hello";
		char *p = jstr_stpcat(b, " world");
		RCHECK(p == b + 11);
		RCHECK(!strcmp(b, "hello world"));
	}
	{
		char b[32] = "";
		char *p = jstr_stpcat(b, "abc");
		RCHECK(p == b + 3);
		RCHECK(!strcmp(b, "abc"));
	}
	{
		char b[32] = "abc";
		char *p = jstr_stpcat(b, "");
		RCHECK(p == b + 3);
		RCHECK(!strcmp(b, "abc"));
	}
	{
		char b[8] = "";
		char *p = jstr_stpcat(b, "");
		RCHECK(p == b);
		RCHECK(b[0] == '\0');
	}

	rigor_srand(0x5151u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t hl = rigor_lens[li];
		if (!JSTR_TEST_SLOW && hl > 128)
			continue;
		rigor_fill_printable(src, hl);
		for (size_t k = 0; k < RIGOR_NLENS; ++k) {
			const size_t sl = rigor_lens[k];
			if (sl > 128)
				continue;
			rigor_fill_printable(dst, sl);
			char *p = jstr_stpcat(dst, src);
			RCHECK(p == dst + sl + hl);
			RCHECK(strlen(dst) == sl + hl);
			RCHECK(!memcmp(dst + sl, src, hl));
			RCHECK(dst[sl + hl] == '\0');
		}
	}

	return 0;
}
