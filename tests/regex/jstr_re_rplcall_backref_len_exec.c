/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rplcall_backref_len_exec expands backreferences in every
 * match, covering groups, \0, plain templates, removal, nmatch truncation,
 * escapes, alternation and epsilon matches. */

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

	/* Each match collapses to its first group. */
	fill(&r, "abab", 4);
	RCHECK_EQ(jstr_re_rplcall_backref_len_exec(&re, jstr_struct(&r), "\\1", 2, 0, 3), 2);
	check(&r, "aa", 2);

	/* \0 plus a suffix grows each match. */
	fill(&r, "abab", 4);
	RCHECK_EQ(jstr_re_rplcall_backref_len_exec(&re, jstr_struct(&r), "\\0!", 3, 0, 3), 2);
	check(&r, "ab!ab!", 6);

	/* Swapping groups per match. */
	fill(&r, "abab", 4);
	RCHECK_EQ(jstr_re_rplcall_backref_len_exec(&re, jstr_struct(&r), "\\2\\1", 4, 0, 3), 2);
	check(&r, "baba", 4);

	/* Prefix and suffix survive untouched. */
	fill(&r, "xabyab", 6);
	RCHECK_EQ(jstr_re_rplcall_backref_len_exec(&re, jstr_struct(&r), "\\2\\1", 4, 0, 3), 2);
	check(&r, "xbayba", 6);

	/* Plain template (no backref) is copied literally per match. */
	fill(&r, "abab", 4);
	RCHECK_EQ(jstr_re_rplcall_backref_len_exec(&re, jstr_struct(&r), "XY", 2, 0, 3), 2);
	check(&r, "XYXY", 4);

	/* Empty replacement removes every match. */
	fill(&r, "abab", 4);
	RCHECK_EQ(jstr_re_rplcall_backref_len_exec(&re, jstr_struct(&r), "", 0, 0, 3), 2);
	check(&r, "", 0);

	/* nmatch == 1 makes the group reference literal. */
	fill(&r, "abab", 4);
	RCHECK_EQ(jstr_re_rplcall_backref_len_exec(&re, jstr_struct(&r), "\\1", 2, 0, 1), 2);
	check(&r, "\\1\\1", 4);

	/* Non-digit escape after a real reference stays literal per match. */
	fill(&r, "abab", 4);
	RCHECK_EQ(jstr_re_rplcall_backref_len_exec(&re, jstr_struct(&r), "\\1\\n", 4, 0, 3), 2);
	static const char bs_n[] = {'a', '\\', 'n', 'a', '\\', 'n'};
	check(&r, bs_n, sizeof(bs_n));

	/* Out-of-range \9 stays literal per match. */
	fill(&r, "abab", 4);
	RCHECK_EQ(jstr_re_rplcall_backref_len_exec(&re, jstr_struct(&r), "\\9", 2, 0, 3), 2);
	check(&r, "\\9\\9", 4);
	jstr_re_free(&re);

	/* No match: zero, unchanged. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "zz", 0)));
	fill(&r, "abab", 4);
	RCHECK_EQ(jstr_re_rplcall_backref_len_exec(&re, jstr_struct(&r), "\\0", 2, 0, 2), 0);
	check(&r, "abab", 4);
	jstr_re_free(&re);

	/* Non-participating group in an alternation. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "(a)|(b)", JSTR_RE_CF_EXTENDED)));
	fill(&r, "ab", 2);
	RCHECK_EQ(jstr_re_rplcall_backref_len_exec(&re, jstr_struct(&r), "<\\1\\2>", 6, 0, 3), 2);
	check(&r, "<a\\2><\\1b>", 10);
	jstr_re_free(&re);

	/* Epsilon pattern on an empty subject runs at most once. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "(a*)", JSTR_RE_CF_EXTENDED)));
	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rplcall_backref_len_exec(&re, jstr_struct(&r), "\\0", 2, 0, 2), 1);
	check(&r, "", 0);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
