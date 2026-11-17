/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rplc_backref_len_exec expands \0..\9 in the first-match
 * replacement, with non-participating groups, out-of-range references,
 * non-digit escapes, plain templates and removal. */

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
	RCHECK_EQ(r->data[len], '\0');
}

static void
check(const jstr_ty *r, const char *want, size_t len)
{
	RCHECK_EQ(r->size, len);
	RCHECK_MEM(r->data, want, len);
	RCHECK_EQ(r->data[len], '\0');
	RCHECK(r->capacity > r->size);
}

int
main(void)
{
	jstr_ty r = JSTR_INIT;
	jstr_re_ty re;

	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "(a)(b)", JSTR_RE_CF_EXTENDED)));

	/* \2\1 swaps the two captured groups inside the match. */
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_exec(&re, jstr_struct(&r), "\\2\\1", 4, 0, 3), 1);
	check(&r, "zba", 3);

	/* Wrapping a group keeps the surrounding prefix. */
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_exec(&re, jstr_struct(&r), "<\\2>", 4, 0, 3), 1);
	check(&r, "z<b>", 4);

	/* \0 is the whole match. */
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_exec(&re, jstr_struct(&r), "\\0!", 3, 0, 3), 1);
	check(&r, "zab!", 4);

	/* Repeating a group. */
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_exec(&re, jstr_struct(&r), "\\1\\1", 4, 0, 3), 1);
	check(&r, "zaa", 3);

	/* Non-digit escapes after a real reference stay literal. */
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_exec(&re, jstr_struct(&r), "\\1\\n", 4, 0, 3), 1);
	static const char bs_n[] = {'z', 'a', '\\', 'n'};
	check(&r, bs_n, sizeof(bs_n));

	/* A plain template falls back to literal replacement. */
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_exec(&re, jstr_struct(&r), "XY", 2, 0, 3), 1);
	check(&r, "zXY", 3);

	/* Empty replacement removes the match. */
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_exec(&re, jstr_struct(&r), "", 0, 0, 3), 1);
	check(&r, "z", 1);

	/* Out-of-range \9 stays literal. */
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_exec(&re, jstr_struct(&r), "\\9", 2, 0, 3), 1);
	check(&r, "z\\9", 3);

	/* nmatch == 1 makes \1 out of range, hence literal. */
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_exec(&re, jstr_struct(&r), "\\1", 2, 0, 1), 1);
	check(&r, "z\\1", 3);
	jstr_re_free(&re);

	/* No match: zero, unchanged. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "zz", 0)));
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_exec(&re, jstr_struct(&r), "\\1", 2, 0, 2), 0);
	check(&r, "zab", 3);
	jstr_re_free(&re);

	/* Left/right alternation: an unmatched group stays literal while the
	 * participating group expands. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "(a)|(b)", JSTR_RE_CF_EXTENDED)));
	fill(&r, "b", 1);
	RCHECK_EQ(jstr_re_rplc_backref_len_exec(&re, jstr_struct(&r), "<\\1\\2>", 6, 0, 3), 1);
	check(&r, "<\\1b>", 5);

	fill(&r, "a", 1);
	RCHECK_EQ(jstr_re_rplc_backref_len_exec(&re, jstr_struct(&r), "<\\1\\2>", 6, 0, 3), 1);
	check(&r, "<a\\2>", 5);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
