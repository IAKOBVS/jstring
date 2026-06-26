/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Fuzz test for itoa.h: ulltoa/lltoa/utoa/ltoa/itoa with various bases,
 * thousand-separator variants, atou/atoi/atol/atoull _len variants. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/jstr.h"
#include "fuzz-shared.h"

static char *
ref_ulltoa_p(unsigned long long n, char *buf, unsigned int base)
{
	char tmp[64];
	size_t i = 0;
	if (n == 0) {
		tmp[i++] = '0';
	} else {
		while (n) {
			unsigned int d = (unsigned int)(n % base);
			tmp[i++] = (char)(d < 10 ? '0' + d : 'a' + d - 10);
			n /= base;
		}
	}
	while (i)
		*buf++ = tmp[--i];
	*buf = '\0';
	return buf;
}

static int
ref_streq(const char *a, const char *b)
{
	while (*a && *b && *a == *b) { ++a; ++b; }
	return *a == *b;
}

JSTR_ATTR_MAYBE_UNUSED
static void
fuzz_itoa(size_t iter)
{
	char ref_buf[128], j_buf[128];

	for (size_t i = 0; i < iter; ++i) {
		unsigned long long val = (unsigned long long)rand();
		val |= ((unsigned long long)rand()) << 32;
		/* jstr_ulltoa_p only handles digits 0-9 (no a-f), so restrict to bases 2-10 */
		unsigned int base = (unsigned int)(rand() % 9) + 2;

		/* --- ulltoa_p --- */
		memset(j_buf, 0xAA, sizeof(j_buf));
		char *je = jstr_ulltoa_p(val, j_buf, base);
		char *re = ref_ulltoa_p(val, ref_buf, base);
		assert(ref_streq(j_buf, ref_buf));
		assert(*je == '\0');
		assert((size_t)(je - j_buf) == (size_t)(re - ref_buf));
		(void)re;

		/* --- utoa_p (unsigned int) --- */
		memset(j_buf, 0xAA, sizeof(j_buf));
		unsigned int uval = (unsigned int)(val & 0xFFFFFFFF);
		je = jstr_utoa_p(uval, j_buf, base);
		ref_ulltoa_p(uval, ref_buf, base);
		assert(ref_streq(j_buf, ref_buf));
		assert(*je == '\0');

		/* --- lltoa_p (signed) --- */
		long long sval = (long long)val;
		memset(j_buf, 0xAA, sizeof(j_buf));
		je = jstr_lltoa_p(sval, j_buf, base);
		/* reference for signed */
		if (sval == 0) {
			ref_buf[0] = '0';
			ref_buf[1] = '\0';
			re = ref_buf + 1;
		} else if (sval < 0) {
			ref_buf[0] = '-';
			re = ref_ulltoa_p((unsigned long long)(-sval), ref_buf + 1, base);
		} else {
			re = ref_ulltoa_p((unsigned long long)sval, ref_buf, base);
		}
		assert(ref_streq(j_buf, ref_buf));
		assert(*je == '\0');

		/* --- itoa_p (int) --- */
		int ival = (int)(sval & 0x7FFFFFFF);
		if (rand() % 2) ival = -ival;
		memset(j_buf, 0xAA, sizeof(j_buf));
		je = jstr_itoa_p(ival, j_buf, base);
		if (ival == 0) {
			ref_buf[0] = '0';
			ref_buf[1] = '\0';
			re = ref_buf + 1;
		} else if (ival < 0) {
			ref_buf[0] = '-';
			re = ref_ulltoa_p((unsigned long long)(-(long long)ival), ref_buf + 1, base);
		} else {
			re = ref_ulltoa_p((unsigned long long)ival, ref_buf, base);
		}
		assert(ref_streq(j_buf, ref_buf));
		assert(*je == '\0');

		/* --- thousand-sep variants --- */
		char sep = ',';
		memset(j_buf, 0xAA, sizeof(j_buf));
		je = jstr_ulltoa_thousep_p(val, j_buf, base, sep);
		assert(*je == '\0');
		if (val <= 999) {
			assert(ref_streq(j_buf, ref_buf));
		} else {
			/* at least check format: groups of 3 separated by sep */
			size_t jl = (size_t)(je - j_buf);
			if (jl > 3) {
				assert(j_buf[jl - 4] == sep);
			}
		}
	}

	/* --- safe variants (dynamic allocation, each call appends at *s+*sz) --- */
	char *s = NULL;
	size_t sz = 0, cap = 0;
	for (size_t i = 0; i < iter; ++i) {
		unsigned int uval = (unsigned int)(rand() & 0xFFFF);
		unsigned int base = 10;
		int ret;

		sz = 0;
		ret = jstr_utoa(&s, &sz, &cap, uval, base);
		assert(ret == JSTR_RET_SUCC);
		{char *re = ref_ulltoa_p(uval, ref_buf, base);(void)re;}
		assert(ref_streq(s, ref_buf));
		assert(sz == strlen(s));

		long long sval = (long long)(int)(rand() & 0x7FFF);
		if (rand() % 2) sval = -sval;
		sz = 0;
		ret = jstr_lltoa(&s, &sz, &cap, sval, base);
		assert(ret == JSTR_RET_SUCC);
		if (sval < 0) {
			ref_buf[0] = '-';
			ref_ulltoa_p((unsigned long long)(-sval), ref_buf + 1, base);
		} else {
			ref_ulltoa_p((unsigned long long)sval, ref_buf, base);
		}
		assert(ref_streq(s, ref_buf));
		assert(sz == strlen(s));

		/* thousand-sep safe */
		sz = 0;
		ret = jstr_ulltoa_thousep(&s, &sz, &cap, 123456, 10, ',');
		assert(ret == JSTR_RET_SUCC);
		assert(ref_streq(s, "123,456"));

		sz = 0;
		ret = jstr_lltoa_thousep(&s, &sz, &cap, -123456, 10, ',');
		assert(ret == JSTR_RET_SUCC);
		assert(ref_streq(s, "-123,456"));
	}
	free(s);

	/* --- _len conversion variants --- */
	for (size_t i = 0; i < iter; ++i) {
		char num_buf[32];
		size_t nl = gen_str(num_buf, 30, GEN_ASCII);
		for (size_t j = 0; j < nl; ++j) {
			if (num_buf[j] < '0' || num_buf[j] > '9') {
				if (j > 0 || num_buf[j] != '-')
					num_buf[j] = '0' + (rand() % 10);
			}
		}
		num_buf[nl] = '\0';

		unsigned int u = jstr_atou_len(num_buf, nl);
		unsigned long ul = jstr_atoul_len(num_buf, nl);
		unsigned long long ull = jstr_atoull_len(num_buf, nl);
		int i2 = jstr_atoi_len(num_buf, nl);
		long l = jstr_atol_len(num_buf, nl);
		long long ll = jstr_atoll_len(num_buf, nl);
		(void)u;
		(void)ul;
		(void)ull;
		(void)i2;
		(void)l;
		(void)ll;
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
		TESTING(fuzz_itoa);
		fuzz_itoa(iter);
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
	fuzz_itoa(iter);
	return 0;
}

#endif
