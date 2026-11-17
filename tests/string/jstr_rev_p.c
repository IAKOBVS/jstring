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
	char buf[1200], orig[1200];

	{
		char b[8] = "";
		char *p = jstr_rev_p(b);
		RCHECK(p == b);
		RCHECK(b[0] == '\0');
	}
	{
		char b[8] = "abc";
		char *p = jstr_rev_p(b);
		RCHECK(p == b + 3);
		RCHECK(!strcmp(b, "cba"));
		RCHECK(b[3] == '\0');
	}
	{
		char b[16] = "hello world";
		char *p = jstr_rev_p(b);
		RCHECK(p == b + 11);
		RCHECK(!strcmp(b, "dlrow olleh"));
	}

	rigor_srand(0xbeeffaceu);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill_printable(buf, len);
		memcpy(orig, buf, len);
		char *p = jstr_rev_p(buf);
		RCHECK(p == buf + len);
		RCHECK(buf[len] == '\0');
		for (size_t i = 0; i < len; ++i)
			RCHECK(buf[i] == orig[len - 1 - i]);
		char *q = jstr_rev_p(buf);
		RCHECK(q == buf + len);
		RCHECK(!memcmp(buf, orig, len));
	}

	return 0;
}
