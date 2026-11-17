/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rplc_backref_len_from_exec expands backreferences for the
 * first match at/after start_idx, with middle/end/out-of-range offsets,
 * removal, plain templates and non-participating groups. */

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

	/* Offset 0 / 1 both reach the match at index 1. */
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_from_exec(&re, jstr_struct(&r), 0, "\\2\\1", 4, 0, 3), 1);
	check(&r, "zba", 3);

	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_from_exec(&re, jstr_struct(&r), 1, "\\2\\1", 4, 0, 3), 1);
	check(&r, "zba", 3);

	/* Offset 2 leaves only "b", which cannot match. */
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_from_exec(&re, jstr_struct(&r), 2, "\\2\\1", 4, 0, 3), 0);
	check(&r, "zab", 3);

	/* start_idx == size and start_idx > size are no-ops. */
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_from_exec(&re, jstr_struct(&r), 3, "\\1", 2, 0, 3), 0);
	check(&r, "zab", 3);

	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_from_exec(&re, jstr_struct(&r), 4, "\\1", 2, 0, 3), 0);
	check(&r, "zab", 3);

	/* A middle offset keeps the untouched prefix and suffix. */
	fill(&r, "zzabab", 6);
	RCHECK_EQ(jstr_re_rplc_backref_len_from_exec(&re, jstr_struct(&r), 2, "<\\1>", 4, 0, 3), 1);
	check(&r, "zz<a>ab", 7);

	fill(&r, "zzabab", 6);
	RCHECK_EQ(jstr_re_rplc_backref_len_from_exec(&re, jstr_struct(&r), 2, "\\2\\1", 4, 0, 3), 1);
	check(&r, "zzbaab", 6);

	/* Empty replacement removes the match at the offset. */
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_from_exec(&re, jstr_struct(&r), 1, "", 0, 0, 3), 1);
	check(&r, "z", 1);
	jstr_re_free(&re);

	/* No match at all. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "zz", 0)));
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_from_exec(&re, jstr_struct(&r), 0, "\\1", 2, 0, 2), 0);
	check(&r, "zab", 3);
	jstr_re_free(&re);

	/* Plain template falls back to literal replacement from the offset. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "(a)(b)", JSTR_RE_CF_EXTENDED)));
	fill(&r, "zzabab", 6);
	RCHECK_EQ(jstr_re_rplc_backref_len_from_exec(&re, jstr_struct(&r), 2, "XY", 2, 0, 3), 1);
	check(&r, "zzXYab", 6);

	/* nmatch == 1 makes the group reference literal. */
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_from_exec(&re, jstr_struct(&r), 1, "\\1", 2, 0, 1), 1);
	check(&r, "z\\1", 3);

	/* Non-digit escape after a real reference stays literal. */
	fill(&r, "zab", 3);
	RCHECK_EQ(jstr_re_rplc_backref_len_from_exec(&re, jstr_struct(&r), 1, "\\1\\n", 4, 0, 3), 1);
	static const char bs_n[] = {'z', 'a', '\\', 'n'};
	check(&r, bs_n, sizeof(bs_n));
	jstr_re_free(&re);

	/* Left/right alternation with a non-participating group from offset 0. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "(a)|(b)", JSTR_RE_CF_EXTENDED)));
	fill(&r, "zb", 2);
	RCHECK_EQ(jstr_re_rplc_backref_len_from_exec(&re, jstr_struct(&r), 0, "<\\1\\2>", 6, 0, 3), 1);
	check(&r, "z<\\1b>", 6);

	fill(&r, "zb", 2);
	RCHECK_EQ(jstr_re_rplc_backref_len_from_exec(&re, jstr_struct(&r), 1, "<\\1\\2>", 6, 0, 3), 1);
	check(&r, "z<\\1b>", 6);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
