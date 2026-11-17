/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: in-loop NOTBOL helper looks strictly at the byte before the
 * cursor; without JSTR_RE_CF_NEWLINE it is always NOTBOL (even at start). */

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
	const char *s = "x\ny\nz";
	const size_t sz = strlen(s);

	for (size_t i = 0; i <= sz; ++i) {
		const int expect_nl = (i > 0 && s[i - 1] == '\n') ? 0 : JSTR_RE_EF_NOTBOL;
		RCHECK_EQ(jstr_internal_re_notbol_inloop(s + i, i, JSTR_RE_CF_NEWLINE), expect_nl);
		RCHECK_EQ(jstr_internal_re_notbol_inloop(s + i, i, 0), JSTR_RE_EF_NOTBOL);
		RCHECK_EQ(jstr_internal_re_notbol_inloop(s + i, i, JSTR_RE_CF_ICASE), JSTR_RE_EF_NOTBOL);
		RCHECK_EQ(jstr_internal_re_notbol_inloop(s + i, i, JSTR_RE_CF_EXTENDED | JSTR_RE_CF_NEWLINE), expect_nl);
	}

	/* curr_idx == 0 never looks backwards, so NEWLINE cannot help. */
	RCHECK_EQ(jstr_internal_re_notbol_inloop("abc", 0, JSTR_RE_CF_NEWLINE), JSTR_RE_EF_NOTBOL);
	RCHECK_EQ(jstr_internal_re_notbol_inloop("abc", 0, 0), JSTR_RE_EF_NOTBOL);

	/* Explicit newline boundaries. The first argument points at the
	 * current search position, hence the +1/+2 offsets. */
	RCHECK_EQ(jstr_internal_re_notbol_inloop("\nb" + 1, 1, JSTR_RE_CF_NEWLINE), 0);
	RCHECK_EQ(jstr_internal_re_notbol_inloop("\nb" + 1, 1, 0), JSTR_RE_EF_NOTBOL);
	RCHECK_EQ(jstr_internal_re_notbol_inloop("a\nb" + 2, 2, JSTR_RE_CF_NEWLINE), 0);
	RCHECK_EQ(jstr_internal_re_notbol_inloop("a\nb" + 1, 1, JSTR_RE_CF_NEWLINE), JSTR_RE_EF_NOTBOL);
	RCHECK_EQ(jstr_internal_re_notbol_inloop("ab" + 1, 1, JSTR_RE_CF_NEWLINE), JSTR_RE_EF_NOTBOL);

	for (size_t i = 0; i <= sz; ++i) {
		const int r = jstr_internal_re_notbol_inloop(s + i, i, JSTR_RE_CF_NEWLINE);
		RCHECK(r == 0 || r == JSTR_RE_EF_NOTBOL);
	}

	return EXIT_SUCCESS;
}
