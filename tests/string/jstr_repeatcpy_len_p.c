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
	unsigned char src[1300], dst[6300], want[6300];

	RCHECK(jstr_repeatcpy_len_p((char *)dst, "ab", 2, 3) == (char *)dst + 6);
	RCHECK(!memcmp(dst, "ababab", 7));
	RCHECK(jstr_repeatcpy_len_p((char *)dst, "x", 1, 5) == (char *)dst + 5);
	RCHECK(!memcmp(dst, "xxxxx", 6));
	RCHECK(jstr_repeatcpy_len_p((char *)dst, "ab", 2, 0) == (char *)dst);
	RCHECK(dst[0] == 0);
	RCHECK(jstr_repeatcpy_len_p((char *)dst, "", 0, 9) == (char *)dst);
	RCHECK(dst[0] == 0);
	RCHECK(jstr_repeatcpy_len_p((char *)dst, "abc", 3, 1) == (char *)dst + 3);
	RCHECK(!memcmp(dst, "abc", 4));
	{
		const char bin[2] = { 'a', 0 };
		RCHECK(jstr_repeatcpy_len_p((char *)dst, bin, 2, 3) == (char *)dst + 6);
		RCHECK(dst[0] == 'a' && dst[1] == 0 && dst[2] == 'a' && dst[3] == 0 && dst[4] == 'a' && dst[5] == 0);
		RCHECK(dst[6] == 0);
	}

	rigor_srand(0x4e455054u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t sl = rigor_lens[li];
		if (!JSTR_TEST_SLOW && sl > 64)
			continue;
		rigor_fill(src, sl, 1);
		for (size_t n = 0; n <= 6; ++n) {
			const size_t total = sl * n;
			for (size_t i = 0; i < total; ++i)
				want[i] = src[i % (sl ? sl : 1)];
			want[total] = 0;
			memset(dst, 0x7f, total + 3);
			char *p = jstr_repeatcpy_len_p((char *)dst, (const char *)src, sl, n);
			RCHECK(p == (char *)dst + total);
			RCHECK(!memcmp(dst, want, total + 1));
			RCHECK(dst[total + 1] == 0x7f);
		}
	}

	return 0;
}
