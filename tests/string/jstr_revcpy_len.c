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
	unsigned char src[1200], dst[1300], want[1300];

	{
		char b[4];
		memset(b, 0x7f, sizeof b);
		jstr_revcpy_len(b, "", 0);
		RCHECK(b[0] == '\0');
		RCHECK(b[1] == 0x7f);
	}
	{
		char b[8];
		jstr_revcpy_len(b, "abc", 3);
		RCHECK(!strcmp(b, "cba"));
		RCHECK(b[3] == '\0');
	}
	{
		const char bin[5] = { 'a', 'b', 0, 'c', 'd' };
		char b[8];
		jstr_revcpy_len(b, bin, 5);
		RCHECK(b[0] == 'd' && b[1] == 'c' && b[2] == 0 && b[3] == 'b' && b[4] == 'a');
		RCHECK(b[5] == '\0');
	}

	rigor_srand(0xbeef1234u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill(src, len, 1);
		for (size_t i = 0; i < len; ++i)
			want[i] = src[len - 1 - i];
		want[len] = 0;
		memset(dst, 0x7f, len + 4);
		jstr_revcpy_len((char *)dst, (const char *)src, len);
		RCHECK(!memcmp(dst, want, len + 1));
		RCHECK(dst[len + 1] == 0x7f);
		RCHECK(dst[len + 2] == 0x7f);
	}

	return 0;
}
