/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: direct exercise of the core n-replacement engine, both literal
 * and backreference modes, including n/start_idx boundaries and errors. */

#define __STDC_NO_VLA__ 1

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "../rigor.h"
#include "regex.h"

static void
fill(jstr_ty *r, const char *s, size_t len)
{
	r->size = 0;
	RCHECK(!jstr_chk(jstr_assign_len(jstr_struct(r), s, len)));
	RCHECK_EQ(r->size, len);
}

static void
check(const jstr_ty *r, const char *want, size_t len)
{
	RCHECK_EQ(r->size, len);
	RCHECK_MEM(r->data, want, len);
	RCHECK_EQ(r->data[len], '\0');
}

static jstr_re_off_ty
run(jstr_re_ty *re, jstr_ty *r, size_t start, const char *rplc, size_t rplc_len,
    size_t nmatch, size_t n, int backref)
{
	return jstr_internal_re_rplcn_backref_len_from_exec(re, jstr_struct(r), start,
	                                                    rplc, rplc_len, 0, nmatch, n, backref);
}

int
main(void)
{
	jstr_ty r = JSTR_INIT;
	jstr_re_ty re;

	/* Literal replacement, n limit. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a", 0)));
	fill(&r, "banana", 6);
	RCHECK_EQ(run(&re, &r, 0, "X", 1, 1, 2, 0), 2);
	check(&r, "bXnXna", 6);

	fill(&r, "banana", 6);
	RCHECK_EQ(run(&re, &r, 0, "X", 1, 1, 0, 0), 0);
	check(&r, "banana", 6);

	/* Empty replacement is removal. */
	fill(&r, "banana", 6);
	RCHECK_EQ(run(&re, &r, 0, "", 0, 1, 2, 0), 2);
	check(&r, "bnna", 4);
	jstr_re_free(&re);

	/* Backreference expansion of \0 and \1. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "\\(a\\)", 0)));
	fill(&r, "ab", 2);
	RCHECK_EQ(run(&re, &r, 0, "\\0\\0", 4, 2, 1, 1), 1);
	check(&r, "aab", 3);

	fill(&r, "ab", 2);
	RCHECK_EQ(run(&re, &r, 0, "[\\1]", 4, 2, 1, 1), 1);
	check(&r, "[a]b", 4);
	jstr_re_free(&re);

	/* Non-participating group degrades to a literal pair. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "\\(a\\)\\|\\(b\\)", 0)));
	fill(&r, "b", 1);
	RCHECK_EQ(run(&re, &r, 0, "[\\1\\2]", 6, 3, 1, 1), 1);
	check(&r, "[\\1b]", 5);

	/* nmatch too small: the backreference stays literal. */
	fill(&r, "a", 1);
	RCHECK_EQ(run(&re, &r, 0, "\\1", 2, 1, 1, 1), 1);
	check(&r, "\\1", 2);
	jstr_re_free(&re);

	/* start_idx: only matches at/after the offset are touched. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "[0-9]\\{1,\\}", 0)));
	fill(&r, "abc123def456", 12);
	RCHECK_EQ(run(&re, &r, 6, "X", 1, 1, 1, 0), 1);
	check(&r, "abc123defX", 10);

	fill(&r, "abc123def456", 12);
	RCHECK_EQ(run(&re, &r, 12, "X", 1, 1, 1, 0), 0);
	check(&r, "abc123def456", 12);

	fill(&r, "abc123def456", 12);
	RCHECK_EQ(run(&re, &r, 13, "X", 1, 1, 1, 0), 0);
	check(&r, "abc123def456", 12);
	jstr_re_free(&re);

	/* Empty subject with an epsilon pattern inserts once. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a*", 0)));
	fill(&r, "", 0);
	RCHECK_EQ(run(&re, &r, 0, "Z", 1, 1, 1, 0), 1);
	check(&r, "Z", 1);
	jstr_re_free(&re);

	/* n == 1 with backrefs still expands (does not fall back to literal). */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "\\([a-z]\\)", 0)));
	fill(&r, "a1b2", 4);
	RCHECK_EQ(run(&re, &r, 0, "<\\1>", 4, 2, 1, 1), 1);
	check(&r, "<a>1b2", 6);
	jstr_re_free(&re);

	/* A template that is not detected as a backref is copied literally. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a", 0)));
	fill(&r, "aa", 2);
	RCHECK_EQ(run(&re, &r, 0, "x\\yz", 4, 1, 2, 1), 2);
	check(&r, "x\\yzx\\yz", 8);

	/* Trailing lone backslash with n > 1 is emitted literally. */
	fill(&r, "aa", 2);
	RCHECK_EQ(run(&re, &r, 0, "q\\", 2, 1, 2, 1), 2);
	check(&r, "q\\q\\", 4);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
