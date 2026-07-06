/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Fuzz test for stdstring.h functions: strdup, memdup, bzero, stpset,
 * mempcpy, mempmove, memccpy, strnlen, memnchr, comparison loop variants,
 * atoi/atou/atod. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/jstr.h"
#include "fuzz-shared.h"

#include <math.h>

JSTR_ATTR_MAYBE_UNUSED
static void
fuzz_stdstring(size_t iter)
{
	char h[FSTR];
	for (size_t i = 0; i < iter; ++i) {
		enum gen_mode hmode = (enum gen_mode)(rand() % 4);
		size_t hl = gen_str(h, FSTR - 1, hmode);

		/* --- strdup / strdup_len / memdup --- */
		if (hl <= FSTR / 2) {
			char *d = jstr_strdup_len(h, hl);
			assert(d != NULL || hl == 0);
			if (d) {
				assert(memcmp(d, h, hl) == 0);
				assert(d[hl] == '\0');
				free(d);
			}
		}
		if (strlen(h) == hl) {
			char *d = jstr_strdup(h);
			if (d) {
				assert(strcmp(d, h) == 0);
				free(d);
			}
		}
		void *md = jstr_memdup(h, hl);
		assert(md != NULL || hl == 0);
		if (md) {
			assert(memcmp(md, h, hl) == 0);
			free(md);
		}

		/* --- bzero --- */
		char bz[FSTR + 1];
		memcpy(bz, h, hl + 1);
		jstr_bzero(bz, hl);
		for (size_t j = 0; j < hl; ++j)
			assert(bz[j] == 0);
		assert(bz[hl] == h[hl]);

		/* --- stpset / stpset_len / mempset --- */
		memcpy(bz, h, hl + 1);
		int sc = 'A' + (rand() % 26);
		char *sp = jstr_stpset_len(bz, sc, hl);
		assert(sp == bz + hl);
		for (size_t j = 0; j < hl; ++j)
			assert(bz[j] == (char)sc);
		assert(bz[hl] == '\0');

		memcpy(bz, h, hl + 1);
		sp = (char *)jstr_mempset(bz, sc, hl);
		assert(sp == bz + hl);
		for (size_t j = 0; j < hl; ++j)
			assert(bz[j] == (char)sc);

		/* stpset (NUL-terminated — skips if embedded NUL) */
		if (strlen(h) == hl) {
			memcpy(bz, h, hl + 1);
			char *sp2 = jstr_stpset(bz, sc);
			(void)sp2;
			for (size_t j = 0; j < hl; ++j)
				assert(bz[j] == (char)sc);
		}

		/* --- mempcpy --- */
		char mc[FSTR];
		void *mcp = jstr_mempcpy(mc, h, hl);
		assert(mcp == mc + hl);
		assert(memcmp(mc, h, hl) == 0);

		/* --- mempmove (non-overlapping) --- */
		char mm[FSTR];
		void *mmp = jstr_mempmove(mm, h, hl);
		assert(mmp == mm + hl);
		assert(memcmp(mm, h, hl) == 0);

		/* --- memccpy --- */
		char mcc[FSTR];
		int stopc = ' ' + (rand() % 95);
		void *mccp = jstr_memccpy(mcc, h, stopc, hl);
		if (mccp) {
			size_t ncopy = (size_t)((char *)mccp - mcc);
			assert(ncopy <= hl);
			assert(mcc[ncopy - 1] == (char)stopc);
		}

		/* --- strcpy_len / stpcpy_len --- */
		char scopy[FSTR];
		jstr_strcpy_len(scopy, h, hl);
		assert(memcmp(scopy, h, hl) == 0);
		assert(scopy[hl] == '\0');

		char *secp = jstr_stpcpy_len(scopy, h, hl);
		assert(secp == scopy + hl);
		assert(memcmp(scopy, h, hl) == 0);

		/* --- strnlen / strnlen_loop --- */
		size_t bound = (size_t)(rand() % (hl + 10));
		size_t nl1 = jstr_strnlen(h, bound);
		size_t nl2 = jstr_strnlen_loop(h, bound);
		assert(nl1 == nl2);
		assert(nl1 <= bound);
		assert(nl1 == strlen(h) || nl1 == bound);

		/* --- memnchr --- */
		int target = ' ' + (rand() % 95);
		size_t search_n = (size_t)(rand() % (hl + 5));
		void *mcp2 = jstr_memnchr(h, target, hl, search_n);
		size_t limit = hl < search_n ? hl : search_n;
		if (mcp2) {
			size_t off = (size_t)((char *)mcp2 - h);
			assert(off < limit);
			assert(h[off] == (char)target);
		} else {
			for (size_t j = 0; j < limit; ++j)
				assert(h[j] != (char)target);
		}

		/* --- strnchr_len --- */
		char *scp = jstr_strnchr_len(h, target, hl, search_n);
		assert(scp == (char *)mcp2);

		/* --- comparison variants --- */
		char h2[FSTR];
		size_t h2l = gen_str(h2, FSTR - 1, hmode);
		(void)h2l;
		size_t cmp_n = hl < h2l ? hl : h2l;
		if (cmp_n > 0)
			cmp_n = (size_t)(rand() % cmp_n) + 1;
		int cmp1 = jstr_strncasecmp(h, h2, cmp_n);
		int cmp2 = jstr_strncasecmp(h, h2, cmp_n);
		assert((cmp1 == 0) == (cmp2 == 0));
		{int _r = jstr_strncasecmpeq(h, h2, cmp_n);(void)_r;}
		{int _r = jstr_memcasecmpeq(h, h2, cmp_n);(void)_r;}
		int cmpeq_len = jstr_strcasecmpeq_len(h, h2, cmp_n);
		int cmpeq_ll = jstr_strcasecmpeq_len_loop(h, h2, cmp_n);
		assert(!!cmpeq_len == !!cmpeq_ll);

		/* --- stpmove / strmove_len / stpmove_len --- */
		char mv[FSTR];
		memcpy(mv, h, hl + 1);
		char *mve = jstr_stpmove(mv, "XYZ");
		assert(mve == mv + 3);
		assert(memcmp(mv, "XYZ", 3) == 0);
		assert(mv[3] == '\0');

		if (hl + 2 < FSTR) {
			memcpy(mv, h, hl + 1);
			jstr_strmove_len(mv + 2, mv, hl);
			assert(memcmp(mv + 2, h, hl) == 0);
			assert(mv[hl + 2] == '\0');

			memcpy(mv, h, hl + 1);
			mve = jstr_stpmove_len(mv + 2, mv, hl);
			assert(mve == mv + 2 + hl);
			assert(memcmp(mv + 2, h, hl) == 0);
		}

		/* --- memcmpeq_loop / strcmpeq_loop --- */
		int meq = jstr_memcmpeq_loop(h, h, hl);
		assert(meq == 0);
		if (hl > 0) {
			memcpy(h2, h, hl);
			h2[hl - 1] = (char)(h[hl - 1] ^ 1);
			meq = jstr_memcmpeq_loop(h, h2, hl);
			assert(meq != 0);
		}
	}

	/* --- numeric conversions --- */
	char num_buf[FSTR];
	for (size_t i = 0; i < iter; ++i) {
		/* Generate a decimal number string */
		size_t nl = gen_str(num_buf, FSTR - 1, GEN_ASCII);
		/* filter: only digits and optional leading '-' */
		int neg = 0;
		if (nl > 0 && num_buf[0] == '-') {
			neg = 1;
		}
		size_t dj = neg ? 1 : 0;
		for (; dj < nl; ++dj) {
			if (num_buf[dj] < '0' || num_buf[dj] > '9') {
				num_buf[dj] = '0' + (rand() % 10);
			}
		}
		num_buf[nl] = '\0';

		unsigned int uval = jstr_atou(num_buf);
		int ival = jstr_atoi(num_buf);
		unsigned long ulval = jstr_atoul(num_buf);
		long lval = jstr_atol(num_buf);
		unsigned long long ullval = jstr_atoull(num_buf);
		long long llval = jstr_atoll(num_buf);
		(void)uval;
		(void)ival;
		(void)ulval;
		(void)lval;
		(void)ullval;
		(void)llval;

		double dval = jstr_atod(num_buf);
		(void)dval;
		float fval = jstr_atof(num_buf);
		(void)fval;
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
		TESTING(fuzz_stdstring);
		fuzz_stdstring(iter);
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
	fuzz_stdstring(iter);
	return 0;
}

#endif
