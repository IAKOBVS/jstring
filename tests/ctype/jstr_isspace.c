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
		assert((jstr_isspace(i) != 0) == (isspace(uc) != 0));
	}

	/* EOF (-1) is explicitly probed: jstr casts to unsigned char, so it
	 * behaves like byte 0xff rather than libc's EOF sentinel. */
	assert(jstr_isspace(-1) == jstr_isspace(255));
	assert(jstr_isspace(-1) == 0);
	assert((isspace((unsigned char)-1) != 0) == (jstr_isspace(-1) != 0));

	/* Explicit character-class boundaries. */
	assert(jstr_isspace(' ') != 0);
	assert(jstr_isspace('\t') != 0);
	assert(jstr_isspace('\n') != 0);
	assert(jstr_isspace('\v') != 0);
	assert(jstr_isspace('\f') != 0);
	assert(jstr_isspace('\r') != 0);
	assert(jstr_isspace('a') == 0);
	assert(jstr_isspace('0') == 0);
	assert(jstr_isspace(0) == 0);
	assert(jstr_isspace(0x7f) == 0);
	assert(jstr_isspace(0x80) == 0);
	assert(jstr_isspace(0xa0) == 0);
	assert(jstr_isspace(0xff) == 0);

	return 0;
}
