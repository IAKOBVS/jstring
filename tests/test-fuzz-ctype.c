/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Fuzz test for ctype functions: isalpha, islower, isupper, isspace,
 * isblank, isdigit, isxdigit, ispunct, isgraph, isprint, iscntrl,
 * tolower, toupper. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/ctype.h"
#include "fuzz-shared.h"

JSTR_ATTR_MAYBE_UNUSED
static void
fuzz_ctype(size_t iter)
{
	char h[FSTR];
	for (size_t i = 0; i < iter; ++i) {
		enum gen_mode mode = (enum gen_mode)(rand() % 4);
		size_t hl = gen_str(h, FSTR - 1, mode);
		if (hl == 0)
			continue;

		/* Test character classification functions on each character */
		for (size_t j = 0; j < hl; ++j) {
			unsigned char c = (unsigned char)h[j];

			/* isalpha: alphabetic character */
			int ref_isalpha = isalpha(c) != 0;
			int got_isalpha = jstr_isalpha(c) != 0;
			assert(ref_isalpha == got_isalpha);

			/* islower: lowercase character */
			int ref_islower = islower(c) != 0;
			int got_islower = jstr_islower(c) != 0;
			assert(ref_islower == got_islower);

			/* isupper: uppercase character */
			int ref_isupper = isupper(c) != 0;
			int got_isupper = jstr_isupper(c) != 0;
			assert(ref_isupper == got_isupper);

			/* isspace: whitespace character */
			int ref_isspace = isspace(c) != 0;
			int got_isspace = jstr_isspace(c) != 0;
			assert(ref_isspace == got_isspace);

			/* isblank: blank character (space or tab) */
			int ref_isblank = isblank(c) != 0;
			int got_isblank = jstr_isblank(c) != 0;
			assert(ref_isblank == got_isblank);

			/* isdigit: decimal digit */
			int ref_isdigit = isdigit(c) != 0;
			int got_isdigit = jstr_isdigit(c) != 0;
			assert(ref_isdigit == got_isdigit);

			/* isxdigit: hexadecimal digit */
			int ref_isxdigit = isxdigit(c) != 0;
			int got_isxdigit = jstr_isxdigit(c) != 0;
			assert(ref_isxdigit == got_isxdigit);

			/* ispunct: punctuation character */
			int ref_ispunct = ispunct(c) != 0;
			int got_ispunct = jstr_ispunct(c) != 0;
			assert(ref_ispunct == got_ispunct);

			/* isgraph: graphic character (printable and not space) */
			int ref_isgraph = isgraph(c) != 0;
			int got_isgraph = jstr_isgraph(c) != 0;
			assert(ref_isgraph == got_isgraph);

			/* isprint: printable character (including space) */
			int ref_isprint = isprint(c) != 0;
			int got_isprint = jstr_isprint(c) != 0;
			assert(ref_isprint == got_isprint);

			/* iscntrl: control character */
			int ref_iscntrl = iscntrl(c) != 0;
			int got_iscntrl = jstr_iscntrl(c) != 0;
			assert(ref_iscntrl == got_iscntrl);

			/* tolower */
			unsigned char ref_tolower = tolower(c);
			unsigned char got_tolower = jstr_tolower(c);
			assert(ref_tolower == got_tolower);

			/* toupper */
			unsigned char ref_toupper = toupper(c);
			unsigned char got_toupper = jstr_toupper(c);
			assert(ref_toupper == got_toupper);
		}
	}
}

#ifndef FUZZ_LIBFUZZER

int
main(int argc, char **argv)
{
	START();
	unsigned seed = 0;
	if (argc > 1)
		seed = (unsigned)atoi(argv[1]);
	size_t iter = FITER;
#ifdef JSTR_TEST_SLOW
	iter *= 10;
#else
	if (iter > 500)
		iter = 500;
#endif
	if (iter < 1)
		iter = 1;
#ifdef FUZZ_AFL
	while (__AFL_LOOP(10000))
#endif
	{
		srand(seed);
		TESTING(fuzz_ctype);
		fuzz_ctype(iter);
	}
	SUCCESS();
	return EXIT_SUCCESS;
}

#else

int
LLVMFuzzerTestOneInput(const unsigned char *data, size_t size)
{
	if (size < 4)
		return 0;
	unsigned seed = (unsigned)data[0] | ((unsigned)data[1] << 8) |
	                ((unsigned)data[2] << 16) | ((unsigned)data[3] << 24);
	srand(seed);
	data += 4;
	size -= 4;
	size_t iter = size > 0 ? size : 10;
	if (iter > 100)
		iter = 100;
	fuzz_ctype(iter);
	return 0;
}

#endif