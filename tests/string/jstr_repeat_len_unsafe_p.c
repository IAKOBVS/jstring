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
	unsigned char src[1300], buf[6300], want[6300];

	{
		char b[32] = "AB";
		char *p = jstr_repeat_len_unsafe_p(b, 2, 1);
		RCHECK(p == b + 2);
		RCHECK(!strcmp(b, "AB"));
	}
	{
		char b[32] = "AB";
		char *p = jstr_repeat_len_unsafe_p(b, 2, 0);
		RCHECK(p == b + 2);
		RCHECK(!strcmp(b, "AB"));
	}
	{
		char b[32] = "AB";
		char *p = jstr_repeat_len_unsafe_p(b, 2, 3);
		RCHECK(p == b + 6);
		RCHECK(!strcmp(b, "ABABAB"));
	}
	{
		char b[32] = "x";
		char *p = jstr_repeat_len_unsafe_p(b, 1, 5);
		RCHECK(p == b + 5);
		RCHECK(!strcmp(b, "xxxxx"));
	}
	{
		char b[8] = "";
		char *p = jstr_repeat_len_unsafe_p(b, 0, 1);
		RCHECK(p == b);
		RCHECK(b[0] == 0);
	}

	rigor_srand(0x52455054u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t sl = rigor_lens[li];
		if (!JSTR_TEST_SLOW && sl > 64)
			continue;
		rigor_fill(src, sl, 1);
		for (size_t n = 1; n <= 6; ++n) {
			const size_t total = sl * n;
			if (sl == 0 && n >= 2)
				continue; /* unsafe: buffer would only be one byte */
			for (size_t i = 0; i < total; ++i)
				want[i] = src[i % sl];
			want[total] = 0;
			memset(buf, 0x7f, total + 3);
			memcpy(buf, src, sl);
			buf[sl] = 0;
			char *p = jstr_repeat_len_unsafe_p((char *)buf, sl, n);
			RCHECK(p == (char *)buf + total);
			RCHECK(!memcmp(buf, want, total + 1));
			RCHECK(buf[total + 1] == 0x7f);
		}
	}

	return 0;
}
