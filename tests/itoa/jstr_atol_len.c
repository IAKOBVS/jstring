/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jstr.h"

int
main(void)
{

	char buf[] = "-123";
	assert(jstr_atol_len(buf, 0) == 0);
	assert(jstr_atol_len(buf, 1) == 0);
	assert(jstr_atol_len(buf, 4) == -123);
	char buf2[] = "456";
	assert(jstr_atol_len(buf2, 3) == 456);

	assert(jstr_atol_len("", 0) == 0);
	assert(jstr_atol_len("-", 1) == 0);
	assert(jstr_atol_len("-0", 2) == 0);
	assert(jstr_atol_len("0", 1) == 0);

	assert(jstr_atol_len("007", 3) == 7);
	assert(jstr_atol_len("000000001", 9) == 1);
	assert(jstr_atol_len("12345", 3) == 123);
	assert(jstr_atol_len("12345", 5) == 12345);
	assert(jstr_atol_len("-12345", 6) == -12345);

	/* Documented extrema: LONG_MAX and LONG_MIN + 1 are representable. */
	{
		char s[64];
		snprintf(s, sizeof s, "%ld", LONG_MAX);
		assert(jstr_atol_len(s, strlen(s)) == LONG_MAX);
		snprintf(s, sizeof s, "%ld", LONG_MIN + 1);
		assert(jstr_atol_len(s, strlen(s)) == LONG_MIN + 1);
	}

	/* The documented grammar is an optional leading '-' only. */
	assert(jstr_atol_len("+1", 2) != 1);
	assert(jstr_atol_len(" 1", 2) != 1);
	assert(jstr_atol_len("\t1", 2) != 1);
	assert(jstr_atol_len("12a", 3) != 12);

	/* LONG_MIN itself would overflow the signed accumulator; not tested. */

	return 0;
}
