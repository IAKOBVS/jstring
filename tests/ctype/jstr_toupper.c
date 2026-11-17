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

	/* Differential test over the full byte domain (unsigned-char cast, see
	 * jstr_tolower.c). */
	for (int i = 0; i <= 255; ++i) {
		const int uc = (unsigned char)i;
		assert(jstr_toupper(i) == toupper(uc));
	}

	/* Already-uppercase, lowercase, and no-change cases. */
	for (int c = 'A'; c <= 'Z'; ++c) {
		assert(jstr_toupper(c) == c);
		assert(jstr_toupper(c - 'A' + 'a') == c);
	}
	assert(jstr_toupper('0') == '0');
	assert(jstr_toupper('9') == '9');
	assert(jstr_toupper(' ') == ' ');
	assert(jstr_toupper('@') == '@');
	assert(jstr_toupper('[') == '[');
	assert(jstr_toupper('`') == '`');
	assert(jstr_toupper('{') == '{');
	assert(jstr_toupper(0) == 0);
	assert(jstr_toupper(0x7f) == 0x7f);
	assert(jstr_toupper(0x80) == 0x80);
	assert(jstr_toupper(0xfe) == 0xfe);
	assert(jstr_toupper(0xff) == 0xff);

	/* Idempotence across every byte. */
	for (int i = 0; i <= 255; ++i)
		assert(jstr_toupper(jstr_toupper(i)) == jstr_toupper(i));

	/* EOF caveat: jstr treats -1 as byte 0xff (documented unsigned-char
	 * cast); libc would return the EOF sentinel (-1). */
	assert(jstr_toupper(-1) == jstr_toupper(255));
	assert(jstr_toupper(-1) == 255);

	return 0;
}
