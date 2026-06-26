/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Fuzz test for to-case functions: toupperstr_len, tolowerstr_len. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/jstr.h"
#include "fuzz-shared.h"

JSTR_ATTR_MAYBE_UNUSED
static void
fuzz_tocase(size_t iter)
{
	char h[FSTR], buf_j[FSTR], buf_s[FSTR];
	for (size_t i = 0; i < iter; ++i) {
		enum gen_mode mode = (enum gen_mode)(rand() % 4);
		size_t hl = gen_str(h, FSTR - 1, mode);
		if (hl == 0)
			continue;

		/* toupperstr_len */
		memcpy(buf_j, h, hl + 1);
		memcpy(buf_s, h, hl + 1);
		jstr_toupperstr_len(buf_j, hl);
		for (size_t j = 0; j < hl; ++j)
			buf_s[j] = (char)jstr_toupper((unsigned char)buf_s[j]);
		assert(memcmp(buf_j, buf_s, hl) == 0);

		/* _p / _cpy variants stop at NUL, so only test when no embedded NULs */
		int has_nul = (strlen(h) != hl);

		if (!has_nul) {
			/* toupperstr_p (NUL-terminated, returns end ptr) */
			memcpy(buf_j, h, hl + 1);
			char *ep = jstr_toupperstr_p(buf_j);
			assert(ep == buf_j + hl);
			assert(memcmp(buf_j, buf_s, hl) == 0);
			assert(buf_j[hl] == '\0');

			/* toupperstrcpy_p (copy + toupper) */
			memcpy(buf_j, h, hl + 1);
			ep = jstr_toupperstrcpy_p(buf_j, h);
			assert(ep == buf_j + hl);
			assert(memcmp(buf_j, buf_s, hl) == 0);
			assert(buf_j[hl] == '\0');
		}

		/* toupperstrcpy_len (copy + toupper, explicit length) */
		memcpy(buf_j, h, hl + 1);
		jstr_toupperstrcpy_len(buf_j, h, hl);
		assert(memcmp(buf_j, buf_s, hl) == 0);
		assert(buf_j[hl] == '\0');

		/* tolowerstr_len (roundtrip) */
		jstr_tolowerstr_len(buf_j, hl);
		for (size_t j = 0; j < hl; ++j)
			buf_s[j] = (char)jstr_tolower((unsigned char)buf_s[j]);
		assert(memcmp(buf_j, buf_s, hl) == 0);

		if (!has_nul) {
			/* tolowerstr_p */
			memcpy(buf_j, buf_s, hl + 1);
			char *ep = jstr_tolowerstr_p(buf_j);
			assert(ep == buf_j + hl);
			assert(memcmp(buf_j, buf_s, hl) == 0);
			assert(buf_j[hl] == '\0');

			/* tolowerstrcpy_p */
			memcpy(buf_j, h, hl + 1);
			ep = jstr_tolowerstrcpy_p(buf_j, h);
			assert(ep == buf_j + hl);
			assert(memcmp(buf_j, buf_s, hl) == 0);
			assert(buf_j[hl] == '\0');
		}

		/* tolowerstrcpy_len */
		memcpy(buf_j, h, hl + 1);
		jstr_tolowerstrcpy_len(buf_j, h, hl);
		assert(memcmp(buf_j, buf_s, hl) == 0);
		assert(buf_j[hl] == '\0');
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
	/* PRINTERR("seed=%u iter=%zu\n", seed, iter); */
#ifdef FUZZ_AFL
	while (__AFL_LOOP(10000))
#endif
	{
		srand(seed);
		TESTING(fuzz_tocase);
		fuzz_tocase(iter);
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
	fuzz_tocase(iter);
	return 0;
}

#endif
