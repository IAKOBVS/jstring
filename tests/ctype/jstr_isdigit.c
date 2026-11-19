/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <ctype.h>
#include "jstr.h"

int
main(void)
{

	/* Differential test against libc over the full int domain -1..255,
	 * casting through unsigned char (the documented input contract). */
	for (int i = -1; i <= 255; ++i) {
		const int uc = (unsigned char)i;
		assert((jstr_isdigit(i) != 0) == (isdigit(uc) != 0));
	}

	/* EOF (-1) is explicitly probed: jstr casts to unsigned char, so it
	 * behaves like byte 0xff rather than libc's EOF sentinel. */
	assert(jstr_isdigit(-1) == jstr_isdigit(255));
	assert(jstr_isdigit(-1) == 0);
	assert((isdigit((unsigned char)-1) != 0) == (jstr_isdigit(-1) != 0));

	/* Explicit character-class boundaries. */
	assert(jstr_isdigit('0') != 0);
	assert(jstr_isdigit('5') != 0);
	assert(jstr_isdigit('9') != 0);
	assert(jstr_isdigit('/') == 0);
	assert(jstr_isdigit(':') == 0);
	assert(jstr_isdigit('a') == 0);
	assert(jstr_isdigit('A') == 0);
	assert(jstr_isdigit(' ') == 0);
	assert(jstr_isdigit(0x7f) == 0);
	assert(jstr_isdigit(0x80) == 0);
	assert(jstr_isdigit(0xff) == 0);

	return 0;
}
