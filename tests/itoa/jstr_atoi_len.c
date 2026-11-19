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
	assert(jstr_atoi_len(buf, 0) == 0);
	assert(jstr_atoi_len(buf, 1) == 0);
	assert(jstr_atoi_len(buf, 4) == -123);
	char buf2[] = "456";
	assert(jstr_atoi_len(buf2, 3) == 456);

	/* Empty input, a lone sign, and zero with a sign. */
	assert(jstr_atoi_len("", 0) == 0);
	assert(jstr_atoi_len("-", 1) == 0);
	assert(jstr_atoi_len("-0", 2) == 0);
	assert(jstr_atoi_len("0", 1) == 0);

	/* Leading zeros and exact-length prefixes. */
	assert(jstr_atoi_len("007", 3) == 7);
	assert(jstr_atoi_len("000000001", 9) == 1);
	assert(jstr_atoi_len("12345", 3) == 123);
	assert(jstr_atoi_len("12345", 5) == 12345);
	assert(jstr_atoi_len("-12345", 6) == -12345);

	/* Documented extrema: INT_MAX and INT_MIN + 1 are representable. */
	{
		char s[32];
		snprintf(s, sizeof s, "%d", INT_MAX);
		assert(jstr_atoi_len(s, strlen(s)) == INT_MAX);
		snprintf(s, sizeof s, "%d", INT_MIN + 1);
		assert(jstr_atoi_len(s, strlen(s)) == INT_MIN + 1);
	}

	/*
	 * The documented grammar is an optional leading '-' only. Whitespace and
	 * '+' are not skipped/accepted; only assert that they are not silently
	 * treated as the positive value requested by the caller.
	 */
	assert(jstr_atoi_len("+1", 2) != 1);
	assert(jstr_atoi_len(" 1", 2) != 1);
	assert(jstr_atoi_len("\t1", 2) != 1);
	assert(jstr_atoi_len("12a", 3) != 12);

	/*
	 * No overflow/saturation behavior is documented: parsing INT_MIN itself
	 * would overflow the intermediate signed accumulator, so it is not tested.
	 */

	return 0;
}
