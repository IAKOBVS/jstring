/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Tests for itoa.h SAFE wrappers:
 * - countudigits must reserve enough space even with an exactly-fitting
 *   capacity (shrink_to_fit first), for T_MIN/T_MAX values.
 * - thousep SAFE wrappers must not request absurd allocations
 *   (old formula reserved ~value/3 bytes) and must produce correct output.
 * - Invalid bases (<2, >36) must return JSTR_RET_ERR, not crash or loop.
 * - Signed T_MIN must render correctly (negation before unsigned cast).
 *
 * Base 1 is deliberately NOT tested here: it would hang forever in a
 * regressed implementation instead of failing fast. */

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

/* Fill J with N 'x' chars and shrink so capacity == size + 1 exactly. */
static void fill_exact(char **s, size_t *sz, size_t *cap, size_t n) {
	assert(jstr_reserve(s, sz, cap, n + 1) == JSTR_RET_SUCC);
	memset(*s, 'x', n);
	*sz = n;
	*(*s + n) = '\0';
	assert(jstr_shrink_to_fit(s, sz, cap) == JSTR_RET_SUCC);
	assert(*cap == *sz + 1);
}

static void test_safe_underflow_tight_cap(void) {
	char *s = NULL;
	size_t sz = 0, cap = 0;

	/* jstr_itoa INT_MIN: "-2147483648" needs 11+NUL at offset sz. */
	fill_exact(&s, &sz, &cap, 40);
	assert(jstr_itoa(&s, &sz, &cap, INT_MIN, 10) == JSTR_RET_SUCC);
	assert(sz == 40 + 11);
	assert(memcmp(s + 40, "-2147483648", 12) == 0);

	/* jstr_ulltoa ULLONG_MAX: 20 digits. */
	fill_exact(&s, &sz, &cap, 40);
	assert(jstr_ulltoa(&s, &sz, &cap, ULLONG_MAX, 10) == JSTR_RET_SUCC);
	assert(sz == 40 + 20);
	assert(memcmp(s + 40, "18446744073709551615", 21) == 0);

	/* jstr_lltoa LLONG_MIN: sign + 19 digits. */
	fill_exact(&s, &sz, &cap, 40);
	assert(jstr_lltoa(&s, &sz, &cap, LLONG_MIN, 10) == JSTR_RET_SUCC);
	assert(sz == 40 + 20);
	assert(memcmp(s + 40, "-9223372036854775808", 21) == 0);

	jstr_free(&s, &sz, &cap);
}

static void test_safe_thousep(void) {
	char *s = NULL;
	size_t sz = 0, cap = 0;

	/* ULLONG_MAX grouped: "18,446,744,073,709,551,615" (26 chars).
	 * The old countudigits tried to reserve value/3 (~6.1e18) bytes,
	 * which fails to allocate -> error return. Must succeed now. */
	sz = 0;
	assert(jstr_ulltoa_thousep(&s, &sz, &cap, ULLONG_MAX, 10, ',') == JSTR_RET_SUCC);
	assert(sz == 26);
	assert(memcmp(s, "18,446,744,073,709,551,615", 27) == 0);
	/* Capacity sanity: must be tiny, not proportional to the value. */
	assert(cap < 1000);

	/* LLONG_MIN grouped: "-9,223,372,036,854,775,808" (26 chars). */
	sz = 0;
	assert(jstr_lltoa_thousep(&s, &sz, &cap, LLONG_MIN, 10, ',') == JSTR_RET_SUCC);
	assert(sz == 26);
	assert(memcmp(s, "-9,223,372,036,854,775,808", 27) == 0);

	/* Small values still group correctly and append. */
	sz = 0;
	assert(jstr_itoa_thousep(&s, &sz, &cap, 1234567, 10, ',') == JSTR_RET_SUCC);
	assert(memcmp(s, "1,234,567", 10) == 0);

	/* Tight-cap variant: grouping of INT_MIN after exact fit. */
	fill_exact(&s, &sz, &cap, 30);
	assert(jstr_itoa_thousep(&s, &sz, &cap, INT_MIN, 10, ',') == JSTR_RET_SUCC);
	assert(sz == 30 + 14);
	assert(memcmp(s + 30, "-2,147,483,648", 15) == 0);

	jstr_free(&s, &sz, &cap);
}

static void test_invalid_bases(void) {
	char *s = NULL;
	size_t sz = 0, cap = 0;
	const unsigned int bad_bases[] = {0, /* 1 would hang a regressed impl */
	                                  37, 100, UINT_MAX};

	for (size_t i = 0; i < sizeof(bad_bases) / sizeof(bad_bases[0]); ++i) {
		sz = 0;
		if (bad_bases[i] == 0)
			continue; /* covered separately below via _p-free path check */
		assert(jstr_ulltoa(&s, &sz, &cap, 42ULL, bad_bases[i]) == JSTR_RET_ERR);
		assert(sz == 0);
		assert(jstr_itoa(&s, &sz, &cap, -42, bad_bases[i]) == JSTR_RET_ERR);
		assert(sz == 0);
		assert(jstr_ulltoa_thousep(&s, &sz, &cap, 42ULL, bad_bases[i], ',') == JSTR_RET_ERR);
		assert(sz == 0);
		assert(jstr_lltoa_thousep(&s, &sz, &cap, -42LL, bad_bases[i], ',') == JSTR_RET_ERR);
		assert(sz == 0);
	}

	jstr_free(&s, &sz, &cap);
}

/* Bases 0 and 1 previously hit division-by-zero / infinite loops inside
 * countudigits/_p before validation existed. With validation they must
 * return errors without any arithmetic on base. */
static void test_base_zero_one(void) {
	char *s = NULL;
	size_t sz = 0, cap = 0;

	assert(jstr_ulltoa(&s, &sz, &cap, 42ULL, 0) == JSTR_RET_ERR);
	assert(jstr_ulltoa(&s, &sz, &cap, 42ULL, 1) == JSTR_RET_ERR);
	assert(jstr_itoa(&s, &sz, &cap, 42, 0) == JSTR_RET_ERR);
	assert(jstr_itoa(&s, &sz, &cap, 42, 1) == JSTR_RET_ERR);

	jstr_free(&s, &sz, &cap);
}

static void test_roundtrip_power_of_two_and_ten(void) {
	static const unsigned int bases[] = {2, 8, 10, 16};
	static const long long vals[] = {0, 1, -1, 42, -42, LLONG_MAX, LLONG_MIN, 123456789};
	char buf[128];

	for (size_t b = 0; b < sizeof(bases) / sizeof(bases[0]); ++b) {
		for (size_t v = 0; v < sizeof(vals) / sizeof(vals[0]); ++v) {
			char *s = NULL;
			size_t sz = 0, cap = 0;
			assert(jstr_lltoa(&s, &sz, &cap, vals[v], bases[b]) == JSTR_RET_SUCC);
			memcpy(buf, s, sz);
			buf[sz] = '\0';
			long long back = strtoll(buf, NULL, (int)bases[b]);
			assert(back == vals[v]);
			jstr_free(&s, &sz, &cap);
		}
	}
}

int main(void) {
	test_safe_underflow_tight_cap();
	test_safe_thousep();
	test_invalid_bases();
	test_base_zero_one();
	test_roundtrip_power_of_two_and_ten();
	return EXIT_SUCCESS;
}
