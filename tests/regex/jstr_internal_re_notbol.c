/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: boundary/flag matrix for the anchored-match NOTBOL helper. */

#define __STDC_NO_VLA__ 1

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "../rigor.h"
#include "regex.h"

int
main(void)
{
	const char *s = "a\nbc\n";
	const size_t sz = strlen(s);

	/* Exhaustive index/flag matrix: only offset 0 or a preceding newline
	 * (with JSTR_RE_CF_NEWLINE) may claim beginning-of-line. */
	for (size_t i = 0; i <= sz; ++i) {
		const int expect_plain = i == 0 ? 0 : JSTR_RE_EF_NOTBOL;
		const int expect_nl = (i == 0 || s[i - 1] == '\n') ? 0 : JSTR_RE_EF_NOTBOL;
		RCHECK_EQ(jstr_internal_re_notbol(s, i, 0), expect_plain);
		RCHECK_EQ(jstr_internal_re_notbol(s, i, JSTR_RE_CF_NEWLINE), expect_nl);
		/* Unrelated flags must not change the decision. */
		RCHECK_EQ(jstr_internal_re_notbol(s, i, JSTR_RE_CF_ICASE), expect_plain);
		RCHECK_EQ(jstr_internal_re_notbol(s, i, JSTR_RE_CF_EXTENDED | JSTR_RE_CF_NEWLINE), expect_nl);
	}

	/* Explicit spot checks. */
	RCHECK_EQ(jstr_internal_re_notbol("", 0, 0), 0);
	RCHECK_EQ(jstr_internal_re_notbol("", 0, JSTR_RE_CF_NEWLINE), 0);
	RCHECK_EQ(jstr_internal_re_notbol("abc", 0, 0), 0);
	RCHECK_EQ(jstr_internal_re_notbol("abc", 1, 0), JSTR_RE_EF_NOTBOL);
	RCHECK_EQ(jstr_internal_re_notbol("abc", 3, 0), JSTR_RE_EF_NOTBOL);
	RCHECK_EQ(jstr_internal_re_notbol("\n", 1, JSTR_RE_CF_NEWLINE), 0);
	RCHECK_EQ(jstr_internal_re_notbol("\n", 1, 0), JSTR_RE_EF_NOTBOL);
	RCHECK_EQ(jstr_internal_re_notbol("ab\ncd", 3, JSTR_RE_CF_NEWLINE), 0);
	RCHECK_EQ(jstr_internal_re_notbol("ab\ncd", 2, JSTR_RE_CF_NEWLINE), JSTR_RE_EF_NOTBOL);
	RCHECK_EQ(jstr_internal_re_notbol("ab\ncd", 4, JSTR_RE_CF_NEWLINE), JSTR_RE_EF_NOTBOL);

	/* Return value is either 0 or exactly the NOTBOL flag. */
	for (size_t i = 0; i <= sz; ++i) {
		const int r = jstr_internal_re_notbol(s, i, JSTR_RE_CF_NEWLINE);
		RCHECK(r == 0 || r == JSTR_RE_EF_NOTBOL);
	}

	return EXIT_SUCCESS;
}
