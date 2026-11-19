/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>

#include "../../test.h"
#include "internal/musl/ctype.h"

int
main(void)
{

	for (int c = 0; c < 256; c++) {
		const int got = jstr_isalpha(c);
		const int exp = isalpha((unsigned char)c);
		assert((got != 0) == (exp != 0));
		assert(got == (got != 0));
	}

	{
		static const unsigned char yes[] = { 'a', 'm', 'z', 'A', 'M', 'Z' };
		static const unsigned char no[] = { 0, '0', '@', '[', '`', '{', '/', 0x80, 0xFF };
		for (size_t i = 0; i < sizeof yes; ++i)
			assert(jstr_isalpha(yes[i]) != 0);
		for (size_t i = 0; i < sizeof no; ++i)
			assert(jstr_isalpha(no[i]) == 0);
	}

	{
		static const int vals[] = { -1, -2, -127, -128, -255, -256, INT_MIN, INT_MAX };
		for (size_t i = 0; i < sizeof vals / sizeof vals[0]; ++i)
			assert((jstr_isalpha(vals[i]) != 0) == (isalpha((unsigned char)vals[i]) != 0));
	}

	/* The implementation casts to unsigned char, so EOF behaves like 0xFF. */
	assert(jstr_isalpha(EOF) == 0);
	assert((jstr_isalpha(EOF) != 0) == (isalpha((unsigned char)EOF) != 0));

	return EXIT_SUCCESS;
}
