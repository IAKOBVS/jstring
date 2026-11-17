/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: last-backreference scan. The function is always called with a
 * pointer to the first backreference in production, so tests do the same. */

#define __STDC_NO_VLA__ 1

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "../rigor.h"
#include "regex.h"

static void
check(const char *tpl, size_t len, long expected_off)
{
	const char *p = jstr_internal_re_rplcbackreflast((const unsigned char *)tpl, len);
	if (expected_off < 0) {
		RCHECK(p == NULL);
	} else {
		RCHECK(p == tpl + (size_t)expected_off);
	}
}

int
main(void)
{
	/* Too short for a trailing backref pair. */
	check("\\1", 2, -1);
	check("\\1a", 3, -1);
	check("", 0, -1);
	check("\\", 1, -1);

	/* Only the leading backref: no second one. */
	check("\\1ab", 4, -1);

	/* Two backrefs: returns just past the last one. */
	check("\\1\\2", 4, 4);
	check("\\1x\\2", 5, 5);
	check("\\1xx\\2yy", 8, 6);
	check("\\9x\\0", 5, 5);

	/* Non-digit escapes are skipped when scanning backwards. */
	check("\\1x\\y\\2z", 8, 7);

	/* A trailing lone backslash must not be dereferenced. */
	check("\\1a\\", 4, -1);

	/* Returned pointer is aligned to a two-byte pair and within bounds. */
	const char *tpl = "\\3foo\\8bar\\5";
	const size_t len = strlen(tpl);
	const char *p = jstr_internal_re_rplcbackreflast((const unsigned char *)tpl, len);
	RCHECK(p != NULL);
	RCHECK(p <= tpl + len);
	RCHECK((size_t)(p - tpl) >= 2);
	RCHECK_EQ(p[-1] >= '0' && p[-1] <= '9', 1);

	return EXIT_SUCCESS;
}
