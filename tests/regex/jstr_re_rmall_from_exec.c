/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rmall_from_exec removes every match at or after start_idx,
 * with 0/middle/end/out-of-range offsets and buffer invariants. */

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

	/* From 0 this equals rmall. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmall_from_exec(&re, jstr_struct(&r), 0, 0), 3);
	check(&r, "bnn", 3);

	/* From offset 3 only the last two a's remain in scope. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmall_from_exec(&re, jstr_struct(&r), 3, 0), 2);
	check(&r, "bann", 4);

	/* From offset 4 only the final a is removed. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmall_from_exec(&re, jstr_struct(&r), 4, 0), 1);
	check(&r, "banan", 5);

	/* start_idx == size and start_idx > size are no-ops. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmall_from_exec(&re, jstr_struct(&r), 6, 0), 0);
	check(&r, "banana", 6);

	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmall_from_exec(&re, jstr_struct(&r), 10, 0), 0);
	check(&r, "banana", 6);

	/* No match anywhere. */
	fill(&r, "xyz", 3);
	RCHECK_EQ(jstr_re_rmall_from_exec(&re, jstr_struct(&r), 0, 0), 0);
	check(&r, "xyz", 3);
	jstr_re_free(&re);

	/* Multi-byte matches from a middle offset. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "na", 0)));
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmall_from_exec(&re, jstr_struct(&r), 2, 0), 2);
	check(&r, "ba", 2);

	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmall_from_exec(&re, jstr_struct(&r), 3, 0), 1);
	check(&r, "bana", 4);
	jstr_re_free(&re);

	/* Class matches from an offset. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "[0-9]", JSTR_RE_CF_EXTENDED)));
	fill(&r, "a1b2c3", 6);
	RCHECK_EQ(jstr_re_rmall_from_exec(&re, jstr_struct(&r), 3, 0), 2);
	check(&r, "a1bc", 4);

	fill(&r, "a1b2c3", 6);
	RCHECK_EQ(jstr_re_rmall_from_exec(&re, jstr_struct(&r), 4, 0), 1);
	check(&r, "a1b2c", 5);
	jstr_re_free(&re);

	/* Empty subject with epsilon pattern runs at most once. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a*", 0)));
	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rmall_from_exec(&re, jstr_struct(&r), 0, 0), 1);
	check(&r, "", 0);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
