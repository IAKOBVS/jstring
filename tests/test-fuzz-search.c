/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Fuzz test for search functions: memmem, memrmem, strstr_len,
 * strrstr_len, strcasestr_len, countchr_len, count_len,
 * starts_len, ends_len. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/jstr.h"
#include "fuzz-shared.h"

JSTR_ATTR_MAYBE_UNUSED
static void
fuzz_search(size_t iter)
{
	char h[FSTR], n_small[FSTR];
	for (size_t i = 0; i < iter; ++i) {
		enum gen_mode hmode = (enum gen_mode)(rand() % 4);
		enum gen_mode nmode = (enum gen_mode)(rand() % 4);
		size_t hl = gen_str(h, FSTR - 1, hmode);
		size_t nl = gen_str(n_small, FSTR - 1, nmode);

		/* --- memmem --- */
		void *got = jstr_memmem(h, hl, n_small, nl);
		void *exp = simple_memmem(h, hl, n_small, nl);
		assert(got == exp);

		/* --- memrmem --- */
		void *got_rm = jstr_memrmem(h, hl, n_small, nl);
		void *exp_rm = fuzz_memrmem(h, hl, n_small, nl);
		assert(got_rm == exp_rm);

		/* --- strstr_len --- */
		got = jstr_strstr_len(h, hl, n_small, nl);
		exp = simple_memmem(h, hl, n_small, nl);
		assert(got == exp);

		/* --- strrstr_len --- */
		got = jstr_strrstr_len(h, hl, n_small, nl);
		exp = fuzz_memrmem(h, hl, n_small, nl);
		assert(got == exp);

		/* --- strcasestr_len --- */
		if (nl > 0 && hl >= nl) {
			got = jstr_strcasestr_len(h, hl, n_small, nl);
			int found_g = got != NULL;
			int found_e = 0;
			for (size_t j = 0; j <= hl - nl; ++j) {
				int match = 1;
				for (size_t k = 0; k < nl; ++k)
					if (jstr_tolower((unsigned char)h[j + k]) != jstr_tolower((unsigned char)n_small[k])) {
						match = 0;
						break;
					}
				if (match) {
					found_e = 1;
					break;
				}
			}
			assert(found_g == found_e);
		}

		/* --- countchr_len --- */
		if (hl > 0) {
			size_t cnt_g = jstr_countchr_len(h, (unsigned char)n_small[0], hl);
			size_t cnt_e = simple_countchr_len(h, (unsigned char)n_small[0], hl);
			assert(cnt_g == cnt_e);
		}

		/* --- count_len (skips by find_len, no overlap) --- */
		if (nl > 0) {
			size_t cl_g = jstr_count_len(h, hl, n_small, nl);
			void *p = h;
			size_t cl_e = 0;
			while ((p = simple_memmem(p, JSTR_PTR_DIFF(h + hl, p), n_small, nl))) {
				++cl_e;
				p = (char *)p + nl;
			}
			assert(cl_g == cl_e);
		}

		/* --- starts / ends --- */
		if (nl > 0) {
			int sg = jstr_starts_len(h, hl, n_small, nl);
			int se = (hl >= nl && memcmp(h, n_small, nl) == 0);
			assert(!!sg == !!se);

			int eg = jstr_ends_len(h, hl, n_small, nl);
			int ee = (hl >= nl && memcmp(h + hl - nl, n_small, nl) == 0);
			assert(!!eg == !!ee);
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
	/* PRINTERR("seed=%u iter=%zu\n", seed, iter); */
#ifdef FUZZ_AFL
	while (__AFL_LOOP(10000))
#endif
	{
		srand(seed);
		TESTING(fuzz_search);
		fuzz_search(iter);
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
	fuzz_search(iter);
	return 0;
}

#endif
