/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Fuzz test for span functions: rplcspn, rplcallspn, rmspn,
 * rmallspn, stripspn. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/jstr.h"
#include "fuzz-shared.h"

/* ---- simple refs ---- */

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rplcallspn(char *s, size_t *sz, const char *reject, char rplc)
{
	size_t changed = 0;
	for (size_t i = 0; i < *sz; ++i)
		if (strchr(reject, (unsigned char)s[i])) {
			s[i] = rplc;
			++changed;
		}
	return changed;
}

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rplcspn(char *s, const char *reject, char rplc)
{
	size_t span_start = strcspn(s, reject);
	if (s[span_start] == '\0')
		return 0;
	size_t span_len = strspn(s + span_start, reject);
	memset(s + span_start, rplc, span_len);
	return span_len;
}

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rmspn(char *s, size_t *sz, const char *reject)
{
	size_t i = 0;
	while (i < *sz && !strchr(reject, (unsigned char)s[i]))
		++i;
	if (i == *sz)
		return 0;
	size_t span_start = i;
	while (i < *sz && strchr(reject, (unsigned char)s[i]))
		++i;
	size_t span_len = i - span_start;
	memmove(s + span_start, s + i, *sz - i + 1);
	*sz -= span_len;
	return span_len;
}

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rmallspn(char *s, size_t *sz, const char *reject)
{
	size_t rd = 0, wr = 0;
	for (; rd < *sz; ++rd)
		if (!strchr(reject, (unsigned char)s[rd]))
			s[wr++] = s[rd];
	s[wr] = '\0';
	*sz = wr;
	return rd - wr;
}

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_stripspn(char *s, size_t *sz, const char *reject)
{
	return simple_rmallspn(s, sz, reject);
}

/* ---- fuzz function ---- */

JSTR_ATTR_MAYBE_UNUSED
static void
fuzz_replace_spn(size_t iter)
{
	char h[FSTR], buf_j[FSTR], buf_s[FSTR];
	char reject[32];
	for (size_t i = 0; i < iter; ++i) {
		enum gen_mode mode = (enum gen_mode)(rand() % 4);
		size_t hl = gen_str(h, FSTR - 1, mode);
		if (hl == 0)
			continue;
		size_t rjl = (size_t)rand() % 31 + 1;
		gen_str(reject, rjl, GEN_ASCII);
		reject[rjl] = '\0';
		if (strlen(reject) == 0)
			continue;
		char rplc = (char)(' ' + (rand() % 95));

		/* --- rplcspn --- */
		if (hl == strlen(h)) {
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			size_t gj = jstr_rplcspn(buf_j, reject, rplc);
			size_t es = simple_rplcspn(buf_s, reject, rplc);
			assert(gj == es);
			assert(memcmp(buf_j, buf_s, hl + 1) == 0);
		}

		/* --- rplcallspn --- */
		if (hl == strlen(h)) {
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			size_t sj = hl, ss = hl;
			size_t gj = jstr_rplcallspn(buf_j, &sj, reject, rplc);
			size_t es = simple_rplcallspn(buf_s, &ss, reject, rplc);
			assert(gj == es);
			assert(sj == ss);
			assert(memcmp(buf_j, buf_s, sj) == 0);
		}

		/* --- rmspn --- */
		if (hl == strlen(h)) {
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			size_t sj = hl, ss = hl;
			size_t gj = jstr_rmspn(buf_j, &sj, reject);
			size_t es = simple_rmspn(buf_s, &ss, reject);
			assert(gj == es);
			assert(sj == ss);
			assert(memcmp(buf_j, buf_s, sj) == 0);
		}

		/* --- rmallspn --- */
		if (hl == strlen(h)) {
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			size_t sj = hl, ss = hl;
			size_t gj = jstr_rmallspn(buf_j, &sj, reject);
			size_t es = simple_rmallspn(buf_s, &ss, reject);
			assert(gj == es);
			assert(sj == ss);
			assert(memcmp(buf_j, buf_s, sj) == 0);
		}

		/* --- stripspn --- */
		if (hl == strlen(h)) {
			memcpy(buf_j, h, hl + 1);
			memcpy(buf_s, h, hl + 1);
			size_t sj = hl, ss = hl;
			size_t gj = jstr_stripspn(buf_j, &sj, reject);
			size_t es = simple_stripspn(buf_s, &ss, reject);
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
	PRINTERR("seed=%u iter=%zu\n", seed, iter);
#ifdef FUZZ_AFL
	while (__AFL_LOOP(10000))
#endif
	{
		srand(seed);
		TESTING(fuzz_replace_spn);
		fuzz_replace_spn(iter);
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
	fuzz_replace_spn(iter);
	return 0;
}

#endif
