/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rplcn_backref_len_from_exec combines backreference
 * expansion, an n limit and start_idx; covers 0/middle/end/out-of-range
 * offsets, removal, nmatch truncation and epsilon matches. */

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

	/* From offset 2, n == 1 expands only the first match in scope. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcn_backref_len_from_exec(&re, jstr_struct(&r), 2, "\\1", 2, 0, 3, 1), 1);
	check(&r, "xxaab", 5);

	/* From offset 2, n == 2 expands both matches in scope. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcn_backref_len_from_exec(&re, jstr_struct(&r), 2, "\\2\\1", 4, 0, 3, 2), 2);
	check(&r, "xxbaba", 6);

	/* From offset 4 only the second match remains. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcn_backref_len_from_exec(&re, jstr_struct(&r), 4, "\\1", 2, 0, 3, 100), 1);
	check(&r, "xxaba", 5);

	/* n == 0 is a no-op even from a middle offset. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcn_backref_len_from_exec(&re, jstr_struct(&r), 2, "\\1", 2, 0, 3, 0), 0);
	check(&r, "xxabab", 6);

	/* start_idx == size and start_idx > size are no-ops. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcn_backref_len_from_exec(&re, jstr_struct(&r), 6, "\\1", 2, 0, 3, 9), 0);
	check(&r, "xxabab", 6);

	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcn_backref_len_from_exec(&re, jstr_struct(&r), 7, "\\1", 2, 0, 3, 9), 0);
	check(&r, "xxabab", 6);

	/* Empty replacement removes from the offset, bounded by n. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcn_backref_len_from_exec(&re, jstr_struct(&r), 2, "", 0, 0, 3, 2), 2);
	check(&r, "xx", 2);

	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcn_backref_len_from_exec(&re, jstr_struct(&r), 2, "", 0, 0, 3, 1), 1);
	check(&r, "xxab", 4);

	/* Non-digit escape after a real reference stays literal. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcn_backref_len_from_exec(&re, jstr_struct(&r), 2, "\\1\\n", 4, 0, 3, 2), 2);
	static const char bs_n[] = {'x', 'x', 'a', '\\', 'n', 'a', '\\', 'n'};
	check(&r, bs_n, sizeof(bs_n));

	/* nmatch == 1 makes the reference literal. */
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcn_backref_len_from_exec(&re, jstr_struct(&r), 2, "\\1", 2, 0, 1, 2), 2);
	check(&r, "xx\\1\\1", 6);
	jstr_re_free(&re);

	/* No match: zero, unchanged. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "zz", 0)));
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcn_backref_len_from_exec(&re, jstr_struct(&r), 2, "\\0", 2, 0, 2, 5), 0);
	check(&r, "xxabab", 6);
	jstr_re_free(&re);

	/* Plain template from a middle offset. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "(a)(b)", JSTR_RE_CF_EXTENDED)));
	fill(&r, "xxabab", 6);
	RCHECK_EQ(jstr_re_rplcn_backref_len_from_exec(&re, jstr_struct(&r), 2, "XY", 2, 0, 3, 2), 2);
	check(&r, "xxXYXY", 6);
	jstr_re_free(&re);

	/* Epsilon pattern on an empty subject runs at most once. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "(a*)", JSTR_RE_CF_EXTENDED)));
	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rplcn_backref_len_from_exec(&re, jstr_struct(&r), 0, "\\0", 2, 0, 2, 5), 1);
	check(&r, "", 0);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
