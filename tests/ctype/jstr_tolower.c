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

	/* Differential test over the full byte domain.  jstr_tolower casts its
	 * argument through unsigned char, so 0xff is a valid input byte; libc is
	 * queried with the same cast to stay locale-consistent with the existing
	 * tests. */
	for (int i = 0; i <= 255; ++i) {
		const int uc = (unsigned char)i;
		assert(jstr_tolower(i) == tolower(uc));
	}

	/* Already-lowercase, uppercase, and no-change cases. */
	for (int c = 'a'; c <= 'z'; ++c) {
		assert(jstr_tolower(c) == c);
		assert(jstr_tolower(c - 'a' + 'A') == c);
	}
	assert(jstr_tolower('0') == '0');
	assert(jstr_tolower('9') == '9');
	assert(jstr_tolower(' ') == ' ');
	assert(jstr_tolower('@') == '@');
	assert(jstr_tolower('[') == '[');
	assert(jstr_tolower('`') == '`');
	assert(jstr_tolower('{') == '{');
	assert(jstr_tolower(0) == 0);
	assert(jstr_tolower(0x7f) == 0x7f);
	assert(jstr_tolower(0x80) == 0x80);
	assert(jstr_tolower(0xfe) == 0xfe);
	assert(jstr_tolower(0xff) == 0xff);

	/* Idempotence across every byte. */
	for (int i = 0; i <= 255; ++i)
		assert(jstr_tolower(jstr_tolower(i)) == jstr_tolower(i));

	/* EOF caveat: jstr treats -1 as byte 0xff (documented unsigned-char
	 * cast); libc would return the EOF sentinel (-1). */
	assert(jstr_tolower(-1) == jstr_tolower(255));
	assert(jstr_tolower(-1) == 255);

	return 0;
}
