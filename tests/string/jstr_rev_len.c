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
	unsigned char buf[1200], want[1200];

	{
		char b[4] = "abc";
		jstr_rev_len(b, 3);
		RCHECK(!strcmp(b, "cba"));
		jstr_rev_len(b, 3);
		RCHECK(!strcmp(b, "abc"));
	}
	{
		char b[4];
		memset(b, 0x7f, sizeof b);
		jstr_rev_len(b, 0);
		RCHECK(b[0] == 0x7f && b[1] == 0x7f);
	}
	{
		char b[4];
		memset(b, 0x7f, sizeof b);
		jstr_rev_len(b, 1);
		RCHECK(b[0] == 0x7f);
		RCHECK(b[1] == 0x7f);
	}
	{
		char b[6] = { 'a', 'b', 0, 'c', 'd', 'e' };
		jstr_rev_len(b, 5);
		RCHECK(b[0] == 'd' && b[1] == 'c' && b[2] == 0 && b[3] == 'b' && b[4] == 'a');
		RCHECK(b[5] == 'e');
	}

	rigor_srand(0xbeeff00du);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill(buf, len, 1);
		rigor_fill(want, len, 1);
		for (size_t i = 0; i < len; ++i)
			want[i] = buf[len - 1 - i];
		jstr_rev_len((char *)buf, len);
		RCHECK(!memcmp(buf, want, len));
		jstr_rev_len((char *)buf, len);
	}

	return 0;
}
