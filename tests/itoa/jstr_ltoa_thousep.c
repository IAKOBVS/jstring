/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jstr.h"
#include "../test.h"

static void
format_expected(int neg, unsigned long long mag, char sep, char *out)
{
	char digits[32];
	const int n = snprintf(digits, sizeof digits, "%llu", mag);
	int o = 0;
	assert(n > 0);
	if (neg)
		out[o++] = '-';
	for (int i = 0; i < n; ++i) {
		if (i != 0 && (n - i) % 3 == 0)
			out[o++] = sep;
		out[o++] = digits[i];
	}
	out[o] = '\0';
}

static void
check_signed(long value, char sep)
{
	char expected[64];
	const int neg = value < 0;
	const unsigned long long mag = neg ? (unsigned long long)(-(value + 1)) + 1ULL : (unsigned long long)value;
	char *s = NULL;
	size_t sz = 0, cap = 0;

	format_expected(neg, mag, sep, expected);
	assert(jstr_ltoa_thousep(&s, &sz, &cap, value, 10, sep) == JSTR_RET_SUCC);
	assert(sz == strlen(expected));
	assert(s[sz] == '\0');
	assert(strcmp(s, expected) == 0);
	assert(cap > sz);
	if (neg)
		assert(s[0] == '-');
	jstr_free(&s, &sz, &cap);
}

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

	static const unsigned long long mags[] = {
		0ULL, 1ULL, 9ULL, 10ULL, 99ULL, 100ULL, 999ULL, 1000ULL, 1001ULL,
		9999ULL, 10000ULL, 99999ULL, 100000ULL, 999999ULL, 1000000ULL,
		9999999ULL, 10000000ULL, 99999999ULL, 100000000ULL, 999999999ULL,
		1000000000ULL, 9999999999ULL, 100000000000ULL, 999999999999ULL,
		1000000000000ULL, 9999999999999ULL, 10000000000000ULL,
		99999999999999ULL, 100000000000000ULL, 999999999999999ULL,
		1000000000000000ULL, 9999999999999999ULL, 10000000000000000ULL,
		99999999999999999ULL, 100000000000000000ULL, 999999999999999999ULL,
		1000000000000000000ULL, 9999999999999999999ULL
	};

	/* Values with 1..19 decimal digits, both signs, on group boundaries. */
	for (size_t i = 0; i < sizeof mags / sizeof mags[0]; ++i) {
		const unsigned long long mag = mags[i];
		if (mag > (unsigned long long)LONG_MAX)
			continue;
		check_signed((long)mag, ',');
		if (mag != 0)
			check_signed(-(long)mag, ',');
	}

	check_signed(999, '.');
	check_signed(1000, '.');
	check_signed(-999, '.');
	check_signed(-1000, '.');
	check_signed(1234567, '.');
	check_signed(-1234567, '.');

	check_signed(LONG_MAX, ',');
	check_signed(LONG_MIN, ',');

	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(jstr_ltoa_thousep(&s, &sz, &cap, 999, 10, ',') == JSTR_RET_SUCC);
		assert(strcmp(s, "999") == 0);
		assert(strchr(s, ',') == NULL);
		assert(jstr_ltoa_thousep(&s, &sz, &cap, 0, 10, ',') == JSTR_RET_SUCC);
		assert(strcmp(s, "9990") == 0);
		assert(jstr_ltoa_thousep(&s, &sz, &cap, 1, 1, ',') == JSTR_RET_ERR);
		jstr_free(&s, &sz, &cap);
	}

	SUCCESS();
	return 0;
}
