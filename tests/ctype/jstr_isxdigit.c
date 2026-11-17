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
		assert((jstr_isxdigit(i) != 0) == (isxdigit(uc) != 0));
	}

	/* EOF (-1) is explicitly probed: jstr casts to unsigned char, so it
	 * behaves like byte 0xff rather than libc's EOF sentinel. */
	assert(jstr_isxdigit(-1) == jstr_isxdigit(255));
	assert(jstr_isxdigit(-1) == 0);
	assert((isxdigit((unsigned char)-1) != 0) == (jstr_isxdigit(-1) != 0));

	/* Explicit character-class boundaries. */
	assert(jstr_isxdigit('0') != 0);
	assert(jstr_isxdigit('9') != 0);
	assert(jstr_isxdigit('a') != 0);
	assert(jstr_isxdigit('f') != 0);
	assert(jstr_isxdigit('A') != 0);
	assert(jstr_isxdigit('F') != 0);
	assert(jstr_isxdigit('g') == 0);
	assert(jstr_isxdigit('G') == 0);
	assert(jstr_isxdigit('@') == 0);
	assert(jstr_isxdigit('/') == 0);
	assert(jstr_isxdigit(':') == 0);
	assert(jstr_isxdigit(' ') == 0);
	assert(jstr_isxdigit(0x80) == 0);
	assert(jstr_isxdigit(0xff) == 0);

	return 0;
}
