/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jstr.h"

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	char buf[] = "-123";
	assert(jstr_atoll_len(buf, 0) == 0);
	assert(jstr_atoll_len(buf, 1) == 0);
	assert(jstr_atoll_len(buf, 4) == -123);
	char buf2[] = "456";
	assert(jstr_atoll_len(buf2, 3) == 456);

	assert(jstr_atoll_len("", 0) == 0);
	assert(jstr_atoll_len("-", 1) == 0);
	assert(jstr_atoll_len("-0", 2) == 0);
	assert(jstr_atoll_len("0", 1) == 0);

	assert(jstr_atoll_len("007", 3) == 7);
	assert(jstr_atoll_len("000000001", 9) == 1);
	assert(jstr_atoll_len("12345", 3) == 123);
	assert(jstr_atoll_len("12345", 5) == 12345);
	assert(jstr_atoll_len("-12345", 6) == -12345);

	/* Documented extrema: LLONG_MAX and LLONG_MIN + 1 are representable. */
	{
		char s[64];
		snprintf(s, sizeof s, "%lld", LLONG_MAX);
		assert(jstr_atoll_len(s, strlen(s)) == LLONG_MAX);
		snprintf(s, sizeof s, "%lld", LLONG_MIN + 1);
		assert(jstr_atoll_len(s, strlen(s)) == LLONG_MIN + 1);
	}

	/* The documented grammar is an optional leading '-' only. */
	assert(jstr_atoll_len("+1", 2) != 1);
	assert(jstr_atoll_len(" 1", 2) != 1);
	assert(jstr_atoll_len("\t1", 2) != 1);
	assert(jstr_atoll_len("12a", 3) != 12);

	/* LLONG_MIN itself would overflow the signed accumulator; not tested. */

	return 0;
}
