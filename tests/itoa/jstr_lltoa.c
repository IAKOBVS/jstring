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
check_p(long long value, unsigned int base, const char *expected)
{
	char buf[64];
	const size_t n = strlen(expected);
	memset(buf, 0xA5, sizeof buf);
	char *end = jstr_lltoa_p(value, buf, base);
	assert(end == buf + n);
	assert(buf[n] == '\0');
	assert(memcmp(buf, expected, n + 1) == 0);
	assert((unsigned char)buf[n + 1] == 0xA5);
}

static void
check_safe(long long value, const char *expected)
{
	char *s = NULL;
	size_t sz = 0, cap = 0;
	const size_t n = strlen(expected);
	assert(jstr_lltoa(&s, &sz, &cap, value, 10) == JSTR_RET_SUCC);
	assert(sz == n);
	assert(s[sz] == '\0');
	assert(memcmp(s, expected, n + 1) == 0);
	assert(cap > sz);
	if (value != LLONG_MIN)
		assert(jstr_atoll_len(s, sz) == value);
	jstr_free(&s, &sz, &cap);
}

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

	check_p(0, 10, "0");
	check_p(1, 10, "1");
	check_p(-1, 10, "-1");
	check_p(42, 10, "42");
	check_p(-42, 10, "-42");
	check_p(LLONG_MAX, 10, "9223372036854775807");
	check_p(LLONG_MIN, 10, "-9223372036854775808");
	check_p(255, 16, "ff");
	check_p(10, 2, "1010");
	check_p(-15, 16, "-f");

	check_safe(0, "0");
	check_safe(1, "1");
	check_safe(-1, "-1");
	check_safe(42, "42");
	check_safe(-42, "-42");
	check_safe(LLONG_MAX, "9223372036854775807");
	check_safe(LLONG_MIN, "-9223372036854775808");
	check_safe(LLONG_MIN + 1, "-9223372036854775807");

	/* Safe functions append at the current size. */
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(jstr_lltoa(&s, &sz, &cap, -123, 10) == JSTR_RET_SUCC);
		assert(strcmp(s, "-123") == 0);
		assert(sz == 4);
		assert(jstr_lltoa(&s, &sz, &cap, 0, 10) == JSTR_RET_SUCC);
		assert(strcmp(s, "-1230") == 0);
		assert(sz == 5);
		assert(s[sz] == '\0');

		{
			const size_t saved_sz = sz;
			assert(jstr_lltoa(&s, &sz, &cap, 1, 1) == JSTR_RET_ERR);
			assert(sz == saved_sz);
			assert(strcmp(s, "-1230") == 0);
		}
		jstr_free(&s, &sz, &cap);
	}

	SUCCESS();
	return 0;
}
