/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rm_from_exec honours start_idx (0, middle, end and
 * out-of-range), removes exactly one match, and preserves buffer invariants. */

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

	/* start_idx 0 is the ordinary first match. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rm_from_exec(&re, jstr_struct(&r), 0, 0), 1);
	check(&r, "bnana", 5);

	/* A middle start skips earlier matches. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rm_from_exec(&re, jstr_struct(&r), 2, 0), 1);
	check(&r, "banna", 5);

	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rm_from_exec(&re, jstr_struct(&r), 4, 0), 1);
	check(&r, "banan", 5);

	/* start_idx == size: nothing to scan. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rm_from_exec(&re, jstr_struct(&r), 6, 0), 0);
	check(&r, "banana", 6);

	/* start_idx > size is rejected without touching the buffer. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rm_from_exec(&re, jstr_struct(&r), 7, 0), 0);
	check(&r, "banana", 6);

	/* No match anywhere after the offset. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rm_from_exec(&re, jstr_struct(&r), 0, 0), 1);
	check(&r, "bnana", 5);
	jstr_re_free(&re);

	/* Multi-byte match at a middle offset. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "na", 0)));
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rm_from_exec(&re, jstr_struct(&r), 2, 0), 1);
	check(&r, "bana", 4);

	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rm_from_exec(&re, jstr_struct(&r), 3, 0), 1);
	check(&r, "bana", 4);
	jstr_re_free(&re);

	/* Empty subject with an epsilon pattern is processed once. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a*", 0)));
	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rm_from_exec(&re, jstr_struct(&r), 0, 0), 1);
	check(&r, "", 0);

	/* Empty subject with a non-matching pattern is a no-op. */
	jstr_re_free(&re);
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a", 0)));
	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rm_from_exec(&re, jstr_struct(&r), 0, 0), 0);
	check(&r, "", 0);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
