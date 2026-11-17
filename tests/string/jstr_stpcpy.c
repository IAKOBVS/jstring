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
	char src[1200], dst[1300];

	{
		char b[8];
		memset(b, 0x7f, sizeof b);
		char *p = jstr_stpcpy(b, "");
		RCHECK(p == b);
		RCHECK(b[0] == '\0');
		RCHECK(b[1] == 0x7f);
	}
	{
		char b[8];
		memset(b, 0x7f, sizeof b);
		char *p = jstr_stpcpy(b, "abc");
		RCHECK(p == b + 3);
		RCHECK(!strcmp(b, "abc"));
		RCHECK(b[4] == 0x7f);
	}
	{
		char b[16];
		char *p = jstr_stpcpy(b, "hello world");
		RCHECK(p == b + 11);
		RCHECK(!strcmp(b, "hello world"));
	}

	rigor_srand(0x5150u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill_printable(src, len);
		memset(dst, 0x7f, len + 3);
		char *p = jstr_stpcpy(dst, src);
		RCHECK(p == dst + len);
		RCHECK(!memcmp(dst, src, len));
		RCHECK(dst[len] == '\0');
		RCHECK(dst[len + 1] == 0x7f);
		RCHECK(dst[len + 2] == 0x7f);
	}

	return 0;
}
