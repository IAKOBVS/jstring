/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: first-backreference scan of a replacement template. */

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
	const char *p = jstr_internal_re_rplcbackreffirst(tpl, len);
	if (expected_off < 0) {
		RCHECK(p == NULL);
	} else {
		RCHECK(p == tpl + (size_t)expected_off);
		RCHECK_EQ(*p, '\\');
	}
}

int
main(void)
{
	check("", 0, -1);
	check("a", 1, -1);
	check("\\", 1, -1);
	check("\\1", 2, 0);
	check("a\\1", 3, 1);
	check("x\\yz", 4, -1);
	check("a\\xbc", 5, -1);
	/* Regression: non-digit escape immediately before the end used to
	 * advance past the template and call memchr with a wrapped length. */
	check("ab\\x", 4, -1);
	check("a\\xb", 4, -1);
	check("\\x", 2, -1);
	check("\\\\x", 3, -1);
	check("ab\\\\x", 5, -1);
	check("abc", 3, -1);
	check("\\\\1", 3, -1);
	check("a\\xb\\2", 6, 4);
	check("\\1\\2", 4, 0);
	check("\\9", 2, 0);
	check("x\\0y", 4, 1);
	check("no ref here", 11, -1);

	/* The scan never inspects the byte past the supplied length: a
	 * template ending in a lone backslash is a non-match, not an OOB read. */
	check("ending\\", 7, -1);

	/* Every returned pointer addresses a backslash followed by a digit. */
	const char *tpl = "pre\\2mid\\7post";
	const char *p = jstr_internal_re_rplcbackreffirst(tpl, strlen(tpl));
	RCHECK(p != NULL);
	RCHECK_EQ((size_t)(p - tpl), 3);
	RCHECK_EQ(p[0], '\\');
	RCHECK(p[1] >= '0' && p[1] <= '9');

	return EXIT_SUCCESS;
}
