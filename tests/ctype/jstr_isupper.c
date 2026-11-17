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
		assert((jstr_isupper(i) != 0) == (isupper(uc) != 0));
	}

	/* EOF (-1) is explicitly probed: jstr casts to unsigned char, so it
	 * behaves like byte 0xff rather than libc's EOF sentinel. */
	assert(jstr_isupper(-1) == jstr_isupper(255));
	assert(jstr_isupper(-1) == 0);
	assert((isupper((unsigned char)-1) != 0) == (jstr_isupper(-1) != 0));

	/* Explicit character-class boundaries. */
	assert(jstr_isupper('A') != 0);
	assert(jstr_isupper('M') != 0);
	assert(jstr_isupper('Z') != 0);
	assert(jstr_isupper('a') == 0);
	assert(jstr_isupper('z') == 0);
	assert(jstr_isupper('0') == 0);
	assert(jstr_isupper('@') == 0);
	assert(jstr_isupper('[') == 0);
	assert(jstr_isupper('`') == 0);
	assert(jstr_isupper('{') == 0);
	assert(jstr_isupper(0x80) == 0);

	return 0;
}
