/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rplcall_backref_len_from_exec expands backreferences for
 * every match at/after start_idx, with 0/middle/end/out-of-range offsets,
 * removal, nmatch truncation, escapes and epsilon matches. */

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

	/* From 0 this equals the non-from variant. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcall_backref_len_from_exec(&re, jstr_struct(&r), 0, "\\0!", 3, 0, 3), 2);
	check(&r, "xxab!ab!", 8);

	/* From offset 2 both matches remain in scope. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcall_backref_len_from_exec(&re, jstr_struct(&r), 2, "\\1", 2, 0, 3), 2);
	check(&r, "xxaa", 4);

	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcall_backref_len_from_exec(&re, jstr_struct(&r), 2, "\\2\\1", 4, 0, 3), 2);
	check(&r, "xxbaba", 6);

	/* From offset 4 only the second match remains. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcall_backref_len_from_exec(&re, jstr_struct(&r), 4, "\\1", 2, 0, 3), 1);
	check(&r, "xxaba", 5);

	/* start_idx == size and start_idx > size are no-ops. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcall_backref_len_from_exec(&re, jstr_struct(&r), 6, "\\1", 2, 0, 3), 0);
	check(&r, "xxabab", 6);

	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcall_backref_len_from_exec(&re, jstr_struct(&r), 7, "\\1", 2, 0, 3), 0);
	check(&r, "xxabab", 6);

	/* Empty replacement removes matches in scope. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcall_backref_len_from_exec(&re, jstr_struct(&r), 2, "", 0, 0, 3), 2);
	check(&r, "xx", 2);

	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcall_backref_len_from_exec(&re, jstr_struct(&r), 4, "", 0, 0, 3), 1);
	check(&r, "xxab", 4);

	/* Non-digit escape after a real reference stays literal per match. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcall_backref_len_from_exec(&re, jstr_struct(&r), 2, "\\1\\n", 4, 0, 3), 2);
	static const char bs_n[] = {'x', 'x', 'a', '\\', 'n', 'a', '\\', 'n'};
	check(&r, bs_n, sizeof(bs_n));

	/* nmatch == 1 makes the reference literal. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcall_backref_len_from_exec(&re, jstr_struct(&r), 2, "\\1", 2, 0, 1), 2);
	check(&r, "xx\\1\\1", 6);

	/* Plain template from a middle offset. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcall_backref_len_from_exec(&re, jstr_struct(&r), 2, "XY", 2, 0, 3), 2);
	check(&r, "xxXYXY", 6);
	jstr_re_free(&re);

	/* No match: zero, unchanged. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "zz", 0)));
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcall_backref_len_from_exec(&re, jstr_struct(&r), 2, "\\0", 2, 0, 2), 0);
	check(&r, "xxabab", 6);
	jstr_re_free(&re);

	/* Non-participating group with an offset. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "(a)|(b)", JSTR_RE_CF_EXTENDED)));
	fill(&r, "ab", 2);
	RCHECK_EQ(jstr_re_rplcall_backref_len_from_exec(&re, jstr_struct(&r), 1, "<\\1\\2>", 6, 0, 3), 1);
	check(&r, "a<\\1b>", 6);

	fill(&r, "ab", 2);
	RCHECK_EQ(jstr_re_rplcall_backref_len_from_exec(&re, jstr_struct(&r), 0, "<\\1\\2>", 6, 0, 3), 2);
	check(&r, "<a\\2><\\1b>", 10);
	jstr_re_free(&re);

	/* Epsilon pattern on an empty subject runs at most once. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "(a*)", JSTR_RE_CF_EXTENDED)));
	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rplcall_backref_len_from_exec(&re, jstr_struct(&r), 0, "\\0", 2, 0, 2), 1);
	check(&r, "", 0);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
