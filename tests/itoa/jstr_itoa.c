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
check_p(int value, unsigned int base, const char *expected)
{
	char buf[64];
	const size_t n = strlen(expected);
	memset(buf, 0xA5, sizeof buf);
	char *end = jstr_itoa_p(value, buf, base);
	assert(end == buf + n);
	assert(buf[n] == '\0');
	assert(memcmp(buf, expected, n + 1) == 0);
	assert((unsigned char)buf[n + 1] == 0xA5);
}

static void
check_safe(int value, const char *expected)
{
	char *s = NULL;
	size_t sz = 0, cap = 0;
	const size_t n = strlen(expected);
	assert(jstr_itoa(&s, &sz, &cap, value, 10) == JSTR_RET_SUCC);
	assert(sz == n);
	assert(s[sz] == '\0');
	assert(memcmp(s, expected, n + 1) == 0);
	assert(cap > sz);
	/* Round-trip through the wider parser so INT_MIN does not overflow. */
	assert(jstr_atoll_len(s, sz) == (long long)value);
	jstr_free(&s, &sz, &cap);
}

int
main(void)
{

	check_p(0, 10, "0");
	check_p(1, 10, "1");
	check_p(-1, 10, "-1");
	check_p(42, 10, "42");
	check_p(-42, 10, "-42");
	check_p(INT_MAX, 10, "2147483647");
	check_p(INT_MIN, 10, "-2147483648");
	check_p(255, 16, "ff");
	check_p(10, 2, "1010");
	check_p(-15, 16, "-f");

	check_safe(0, "0");
	check_safe(1, "1");
	check_safe(-1, "-1");
	check_safe(42, "42");
	check_safe(-42, "-42");
	check_safe(INT_MAX, "2147483647");
	check_safe(INT_MIN, "-2147483648");

	/* Safe functions append at the current size. */
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(jstr_itoa(&s, &sz, &cap, -123, 10) == JSTR_RET_SUCC);
		assert(strcmp(s, "-123") == 0);
		assert(sz == 4);
		assert(jstr_itoa(&s, &sz, &cap, 0, 10) == JSTR_RET_SUCC);
		assert(strcmp(s, "-1230") == 0);
		assert(sz == 5);
		assert(s[sz] == '\0');

		/* Invalid base is rejected without changing the content. */
		{
			const size_t saved_sz = sz;
			assert(jstr_itoa(&s, &sz, &cap, 1, 1) == JSTR_RET_ERR);
			assert(sz == saved_sz);
			assert(strcmp(s, "-1230") == 0);
		}
		jstr_free(&s, &sz, &cap);
	}

	return 0;
}
