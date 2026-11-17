/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rplcn_len_from_exec combines an n limit with start_idx;
 * covers 0/middle/end/out-of-range offsets, n == 0, removal and binary
 * replacement bytes. */

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

	/* From offset 2, n == 1 replaces the a at index 3. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_from_exec(&re, jstr_struct(&r), 2, "Z", 1, 0, 1), 1);
	check(&r, "banZna", 6);

	/* From offset 2, n == 2 replaces indices 3 and 5. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_from_exec(&re, jstr_struct(&r), 2, "Z", 1, 0, 2), 2);
	check(&r, "banZnZ", 6);

	/* From offset 3 the same two matches remain in scope. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_from_exec(&re, jstr_struct(&r), 3, "Z", 1, 0, 100), 2);
	check(&r, "banZnZ", 6);

	/* From offset 4 only the final a is in scope. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_from_exec(&re, jstr_struct(&r), 4, "Z", 1, 0, 100), 1);
	check(&r, "bananZ", 6);

	/* Full range with n == 2. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_from_exec(&re, jstr_struct(&r), 0, "Z", 1, 0, 2), 2);
	check(&r, "bZnZna", 6);

	/* n == 0 is a no-op regardless of the offset. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_from_exec(&re, jstr_struct(&r), 2, "Z", 1, 0, 0), 0);
	check(&r, "banana", 6);

	/* start_idx == size and start_idx > size are no-ops. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_from_exec(&re, jstr_struct(&r), 6, "Z", 1, 0, 9), 0);
	check(&r, "banana", 6);

	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_from_exec(&re, jstr_struct(&r), 9, "Z", 1, 0, 9), 0);
	check(&r, "banana", 6);
	jstr_re_free(&re);

	/* Empty replacement removes from the offset onward, bounded by n. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a", 0)));
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_from_exec(&re, jstr_struct(&r), 2, "", 0, 0, 2), 2);
	check(&r, "bann", 4);

	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_from_exec(&re, jstr_struct(&r), 4, "", 0, 0, 100), 1);
	check(&r, "banan", 5);
	jstr_re_free(&re);

	/* Binary replacement bytes at a middle offset. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "na", 0)));
	static const char bin[] = {'X', '\0'};
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_from_exec(&re, jstr_struct(&r), 2, bin, sizeof(bin), 0, 1), 1);
	static const char bin_out[] = {'b', 'a', 'X', '\0', 'n', 'a'};
	check(&r, bin_out, sizeof(bin_out));
	jstr_re_free(&re);

	/* Extended class from an offset. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "[0-9]", JSTR_RE_CF_EXTENDED)));
	fill(&r, "a1b2c3", 6);
	RCHECK_EQ(jstr_re_rplcn_len_from_exec(&re, jstr_struct(&r), 3, "N", 1, 0, 1), 1);
	check(&r, "a1bNc3", 6);

	fill(&r, "a1b2c3", 6);
	RCHECK_EQ(jstr_re_rplcn_len_from_exec(&re, jstr_struct(&r), 3, "N", 1, 0, 100), 2);
	check(&r, "a1bNcN", 6);
	jstr_re_free(&re);

	/* Epsilon pattern on an empty subject inserts once. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a*", 0)));
	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rplcn_len_from_exec(&re, jstr_struct(&r), 0, "Z", 1, 0, 5), 1);
	check(&r, "Z", 1);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
