/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Fuzz test for replace-char functions: rmnchr_len_from,
 * rplcnchr_len, rplcallchr_len, rplcchr_len, rmallchr_len. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/jstr.h"
#include "fuzz-shared.h"

/* ---- simple refs ---- */

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rmnchr_len_from(char *s, size_t *sz, size_t start_idx, int c, size_t n)
{
	if (n == 0 || start_idx >= *sz)
		return 0;
	size_t changed = 0;
	const char *src = s + start_idx;
	char *dst = s + start_idx;
	size_t k = *sz - start_idx;
	for (; k--;) {
		if ((unsigned char)*src == (unsigned char)c && n) {
			--n;
			++src;
			++changed;
		} else {
			*dst++ = *src++;
		}
	}
	*dst = '\0';
	*sz = JSTR_PTR_DIFF(dst, s);
	return changed;
}

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rplcnchr_len(char *s, size_t sz, char find, char rplc, size_t n)
{
	if (n == 0)
		return 0;
	size_t changed = 0;
	for (size_t i = 0; i < sz && changed < n; ++i)
		if (s[i] == find) {
			s[i] = rplc;
			++changed;
		}
	return changed;
}

JSTR_ATTR_MAYBE_UNUSED
static void
simple_rplcallchr_len(char *s, size_t sz, char find, char rplc)
{
	for (size_t i = 0; i < sz; ++i)
		if (s[i] == find)
			s[i] = rplc;
}

JSTR_ATTR_MAYBE_UNUSED
static int
simple_rplcchr_len(char *s, size_t sz, char find, char rplc)
{
	unsigned char *p = (unsigned char *)memchr(s, find, sz);
	if (!p)
		return 0;
	*p = (unsigned char)rplc;
	return 1;
}

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rmallchr_len(char *s, size_t *sz, int c)
{
	size_t changed = 0, rd = 0, wr = 0;
	for (; rd < *sz; ++rd)
		if ((unsigned char)s[rd] != (unsigned char)c)
			s[wr++] = s[rd];
		else
			++changed;
	s[wr] = '\0';
	*sz = wr;
	return changed;
}

/* ---- fuzz function ---- */

JSTR_ATTR_MAYBE_UNUSED
static void
fuzz_replace_char(size_t iter)
{
	char h[FSTR], buf_j[FSTR], buf_s[FSTR];
	for (size_t i = 0; i < iter; ++i) {
		enum gen_mode mode = (enum gen_mode)(rand() % 4);
		size_t hl = gen_str(h, FSTR - 1, mode);
		if (hl == 0)
			continue;
		char find = h[(size_t)rand() % hl];
		char rplc = (char)(' ' + (rand() % 95));
		size_t start_idx = (size_t)rand() % hl;
		size_t count = (size_t)rand() % 8;

		/* --- rmnchr_len_from --- */
		if (start_idx < hl && h[start_idx] != '\0') {
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			size_t sj = hl, ss = hl;
			size_t gj = jstr_rmnchr_len_from(buf_j, &sj, start_idx, find, count);
			size_t es = simple_rmnchr_len_from(buf_s, &ss, start_idx, find, count);
			assert(gj == es);
			assert(sj == ss);
			assert(memcmp(buf_j, buf_s, sj) == 0);
		}

		/* --- rplcnchr_len --- */
		{
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			size_t gj = jstr_rplcnchr_len(buf_j, hl, find, rplc, count);
			size_t es = simple_rplcnchr_len(buf_s, hl, find, rplc, count);
			assert(gj == es);
			assert(memcmp(buf_j, buf_s, hl) == 0);
		}

		/* --- rplcallchr_len --- */
		{
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			jstr_rplcallchr_len(buf_j, hl, find, rplc);
			simple_rplcallchr_len(buf_s, hl, find, rplc);
			assert(memcmp(buf_j, buf_s, hl) == 0);
		}

		/* --- rplcchr_len --- */
		{
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			int gj = jstr_rplcchr_len(buf_j, hl, find, rplc);
			int es = simple_rplcchr_len(buf_s, hl, find, rplc);
			assert(!!gj == !!es);
		}

		/* --- rmallchr_len --- */
		if (h[0] != '\0') {
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			size_t sj = hl, ss = hl;
			size_t gj = jstr_rmallchr_len(buf_j, &sj, find);
			size_t es = simple_rmallchr_len(buf_s, &ss, find);
			assert(gj == es);
			assert(sj == ss);
			assert(memcmp(buf_j, buf_s, sj) == 0);
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
		TESTING(fuzz_replace_char);
		fuzz_replace_char(iter);
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
	fuzz_replace_char(iter);
	return 0;
}

#endif
