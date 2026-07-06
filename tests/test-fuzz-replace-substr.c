/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Fuzz test for substring replacement functions: rplcn_len_from,
 * rmn_len. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/jstr.h"
#include "fuzz-shared.h"

/* ---- simple refs ---- */

JSTR_ATTR_MAYBE_UNUSED
static int
simple_rplcn_len_from(char **s, size_t *sz, size_t *cap, size_t start_idx,
                      const char *find, size_t find_len,
                      const char *rplc, size_t rplc_len, size_t n)
{
	if (n == 0 || *sz == 0 || find_len == 0 || start_idx >= *sz)
		return 0;
	size_t changed = 0;
	size_t pos = start_idx;
	for (; changed < n; ++changed) {
		void *p = fuzz_memmem(*s + pos, *sz - pos, find, find_len);
		if (!p)
			break;
		size_t idx = (size_t)((const char *)p - *s);
		ptrdiff_t diff = (ptrdiff_t)(rplc_len) - (ptrdiff_t)(find_len);
		if (diff > 0) {
			size_t new_sz = *sz + (size_t)diff;
			if (new_sz > *cap) {
				size_t new_cap = *cap ? *cap * 2 : 64;
				while (new_cap < new_sz)
					new_cap *= 2;
				char *tmp = (char *)realloc(*s, new_cap + 1);
				assert(tmp);
				*s = tmp;
				*cap = new_cap;
			}
			memmove(*s + idx + rplc_len, *s + idx + find_len,
			        *sz - idx - find_len + 1);
		} else if (diff < 0) {
			memmove(*s + idx + rplc_len, *s + idx + find_len,
			        *sz - idx - find_len + 1);
		}
		memcpy(*s + idx, rplc, rplc_len);
		*sz = *sz + (size_t)diff;
		pos = idx + rplc_len;
	}
	(*s)[*sz] = '\0';
	return (int)changed;
}

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rmn_len(char *s, size_t *sz, const char *find, size_t find_len, size_t n)
{
	if (n == 0 || *sz == 0 || find_len == 0)
		return 0;
	size_t cap = (size_t)-1;
	return (size_t)simple_rplcn_len_from(&s, sz, &cap, 0,
	                                     find, find_len, "", 0, n);
}

/* ---- fuzz function ---- */

JSTR_ATTR_MAYBE_UNUSED
static void
fuzz_replace_substr(size_t iter)
{
	char h[FSTR], n_small[FSTR], r[FSTR];
	char *jbuf = NULL, *sbuf = NULL;
	size_t jsz = 0, jcap = 0, ssz = 0, scap = 0;

	for (size_t i = 0; i < iter; ++i) {
		enum gen_mode mode = (enum gen_mode)(rand() % 4);
		size_t hl = gen_str(h, FSTR - 1, mode);
		size_t nl = gen_str(n_small, FSTR - 1, mode);
		size_t rl = gen_str(r, FSTR - 1, mode);
		if (hl == 0 || nl == 0)
			continue;
		size_t start_idx = hl ? (size_t)rand() % hl : 0;
		size_t count = (size_t)rand() % 8;

		/* --- rplcn_len_from --- */
		assert(jstr_assign_len(&jbuf, &jsz, &jcap, h, hl) == JSTR_RET_SUCC);
		assert(jstr_assign_len(&sbuf, &ssz, &scap, h, hl) == JSTR_RET_SUCC);
		int gj = jstr_rplcn_len_from(&jbuf, &jsz, &jcap, start_idx,
		                             n_small, nl, r, rl, count);
		int es = simple_rplcn_len_from(&sbuf, &ssz, &scap, start_idx,
		                               n_small, nl, r, rl, count);
		if (gj < 0)
			assert(es < 0);
		else {
			assert((size_t)gj == (size_t)es);
			assert(jsz == ssz);
			assert(memcmp(jbuf, sbuf, jsz) == 0);
		}

		/* --- rmn_len --- */
		if (nl != 1 || h[0] != '\0') {
			assert(jstr_assign_len(&jbuf, &jsz, &jcap, h, hl) == JSTR_RET_SUCC);
			assert(jstr_assign_len(&sbuf, &ssz, &scap, h, hl) == JSTR_RET_SUCC);
			size_t rj = jstr_rmn_len(jbuf, &jsz, n_small, nl, count);
			size_t rs = simple_rmn_len(sbuf, &ssz, n_small, nl, count);
			assert(rj == rs);
			assert(jsz == ssz);
			assert(memcmp(jbuf, sbuf, jsz) == 0);
		}
	}

	free(jbuf);
	free(sbuf);
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
		TESTING(fuzz_replace_substr);
		fuzz_replace_substr(iter);
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
	fuzz_replace_substr(iter);
	return 0;
}

#endif
