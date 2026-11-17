/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rplcall_len_from_exec replaces every match at/after
 * start_idx, with 0/middle/end/out-of-range offsets, removal and binary
 * replacement data. */

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

	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a", 0)));

	/* From 0 this equals the non-from variant. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcall_len_from_exec(&re, jstr_struct(&r), 0, "XY", 2, 0), 3);
	check(&r, "bXYnXYnXY", 9);

	/* From offset 2 only indices 3 and 5 are in scope. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcall_len_from_exec(&re, jstr_struct(&r), 2, "Z", 1, 0), 2);
	check(&r, "banZnZ", 6);

	/* Offset 3 includes the same two matches. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcall_len_from_exec(&re, jstr_struct(&r), 3, "Z", 1, 0), 2);
	check(&r, "banZnZ", 6);

	/* Offset 4 leaves only the final a. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcall_len_from_exec(&re, jstr_struct(&r), 4, "Z", 1, 0), 1);
	check(&r, "bananZ", 6);

	/* start_idx == size and start_idx > size are no-ops. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcall_len_from_exec(&re, jstr_struct(&r), 6, "Z", 1, 0), 0);
	check(&r, "banana", 6);

	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcall_len_from_exec(&re, jstr_struct(&r), 9, "Z", 1, 0), 0);
	check(&r, "banana", 6);

	/* Empty replacement removes matches from the offset onward. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcall_len_from_exec(&re, jstr_struct(&r), 2, "", 0, 0), 2);
	check(&r, "bann", 4);

	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcall_len_from_exec(&re, jstr_struct(&r), 4, "", 0, 0), 1);
	check(&r, "banan", 5);
	jstr_re_free(&re);

	/* Binary replacement data from a middle offset. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "na", 0)));
	static const char bin[] = {'X', '\0'};
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcall_len_from_exec(&re, jstr_struct(&r), 2, bin, sizeof(bin), 0), 2);
	static const char bin_out[] = {'b', 'a', 'X', '\0', 'X', '\0'};
	check(&r, bin_out, sizeof(bin_out));
	jstr_re_free(&re);

	/* Extended class from an offset. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "[0-9]", JSTR_RE_CF_EXTENDED)));
	fill(&r, "a1b2c3", 6);
	RCHECK_EQ(jstr_re_rplcall_len_from_exec(&re, jstr_struct(&r), 3, "N", 1, 0), 2);
	check(&r, "a1bNcN", 6);

	fill(&r, "a1b2c3", 6);
	RCHECK_EQ(jstr_re_rplcall_len_from_exec(&re, jstr_struct(&r), 4, "", 0, 0), 1);
	check(&r, "a1b2c", 5);
	jstr_re_free(&re);

	/* Epsilon pattern on an empty subject inserts once. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a*", 0)));
	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rplcall_len_from_exec(&re, jstr_struct(&r), 0, "Z", 1, 0), 1);
	check(&r, "Z", 1);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
