/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <ctype.h>
#include "jstr.h"

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	/* Differential test against libc over the full int domain -1..255,
	 * casting through unsigned char (the documented input contract). */
	for (int i = -1; i <= 255; ++i) {
		const int uc = (unsigned char)i;
		assert((jstr_islower(i) != 0) == (islower(uc) != 0));
	}

	/* EOF (-1) is explicitly probed: jstr casts to unsigned char, so it
	 * behaves like byte 0xff rather than libc's EOF sentinel. */
	assert(jstr_islower(-1) == jstr_islower(255));
	assert(jstr_islower(-1) == 0);
	assert((islower((unsigned char)-1) != 0) == (jstr_islower(-1) != 0));

	/* Explicit character-class boundaries. */
	assert(jstr_islower('a') != 0);
	assert(jstr_islower('m') != 0);
	assert(jstr_islower('z') != 0);
	assert(jstr_islower('A') == 0);
	assert(jstr_islower('Z') == 0);
	assert(jstr_islower('0') == 0);
	assert(jstr_islower('@') == 0);
	assert(jstr_islower('[') == 0);
	assert(jstr_islower('`') == 0);
	assert(jstr_islower('{') == 0);
	assert(jstr_islower(0x80) == 0);

	return 0;
}
