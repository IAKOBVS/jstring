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
check_p(unsigned long long value, unsigned int base, const char *expected)
{
	char buf[64];
	const size_t n = strlen(expected);
	memset(buf, 0xA5, sizeof buf);
	char *end = jstr_ulltoa_p(value, buf, base);
	assert(end == buf + n);
	assert(buf[n] == '\0');
	assert(memcmp(buf, expected, n + 1) == 0);
	assert((unsigned char)buf[n + 1] == 0xA5);
}

static void
check_safe(unsigned long long value, const char *expected)
{
	char *s = NULL;
	size_t sz = 0, cap = 0;
	const size_t n = strlen(expected);
	assert(jstr_ulltoa(&s, &sz, &cap, value, 10) == JSTR_RET_SUCC);
	assert(sz == n);
	assert(s[sz] == '\0');
	assert(memcmp(s, expected, n + 1) == 0);
	assert(cap > sz);
	assert(jstr_atoull_len(s, sz) == value);
	jstr_free(&s, &sz, &cap);
}

int
main(void)
{

	check_p(0, 10, "0");
	check_p(1, 10, "1");
	check_p(9, 10, "9");
	check_p(10, 10, "10");
	check_p(42, 10, "42");
	check_p(ULLONG_MAX, 10, "18446744073709551615");
	check_p(255, 16, "ff");
	check_p(10, 2, "1010");
	check_p(8, 8, "10");

	check_safe(0, "0");
	check_safe(1, "1");
	check_safe(9, "9");
	check_safe(10, "10");
	check_safe(42, "42");
	check_safe(ULLONG_MAX, "18446744073709551615");

	/* Safe functions append at the current size. */
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(jstr_ulltoa(&s, &sz, &cap, 123, 10) == JSTR_RET_SUCC);
		assert(strcmp(s, "123") == 0);
		assert(sz == 3);
		assert(jstr_ulltoa(&s, &sz, &cap, 0, 10) == JSTR_RET_SUCC);
		assert(strcmp(s, "1230") == 0);
		assert(sz == 4);
		assert(s[sz] == '\0');

		{
			const size_t saved_sz = sz;
			assert(jstr_ulltoa(&s, &sz, &cap, 1, 1) == JSTR_RET_ERR);
			assert(sz == saved_sz);
			assert(strcmp(s, "1230") == 0);
		}
		jstr_free(&s, &sz, &cap);
	}

	return 0;
}
