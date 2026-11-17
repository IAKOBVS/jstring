/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rm_exec removes exactly the first match, preserves the
 * remaining bytes, updates size and keeps the NUL/capacity invariants. */

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

	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rm_exec(&re, jstr_struct(&r), 0), 1);
	check(&r, "bnana", 5);

	fill(&r, "aaa", 3);
	RCHECK_EQ(jstr_re_rm_exec(&re, jstr_struct(&r), 0), 1);
	check(&r, "aa", 2);

	fill(&r, "a", 1);
	RCHECK_EQ(jstr_re_rm_exec(&re, jstr_struct(&r), 0), 1);
	check(&r, "", 0);

	/* No match leaves the string byte-identical. */
	fill(&r, "xyz", 3);
	RCHECK_EQ(jstr_re_rm_exec(&re, jstr_struct(&r), 0), 0);
	check(&r, "xyz", 3);

	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rm_exec(&re, jstr_struct(&r), 0), 0);
	check(&r, "", 0);

	/* A match at the very end must terminate the shortened string. */
	fill(&r, "xyza", 4);
	RCHECK_EQ(jstr_re_rm_exec(&re, jstr_struct(&r), 0), 1);
	check(&r, "xyz", 3);
	jstr_re_free(&re);

	/* Case-insensitive compilation drives the search. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "A", JSTR_RE_CF_ICASE)));
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rm_exec(&re, jstr_struct(&r), 0), 1);
	check(&r, "bnana", 5);
	jstr_re_free(&re);

	/* Multi-byte literal match. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "na", 0)));
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rm_exec(&re, jstr_struct(&r), 0), 1);
	check(&r, "bana", 4);
	jstr_re_free(&re);

	/* Extended class match. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "[0-9]+", JSTR_RE_CF_EXTENDED)));
	fill(&r, "ab123cd", 7);
	RCHECK_EQ(jstr_re_rm_exec(&re, jstr_struct(&r), 0), 1);
	check(&r, "abcd", 4);

	/* Epsilon pattern on an empty subject runs at most once. */
	jstr_re_free(&re);
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a*", 0)));
	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rm_exec(&re, jstr_struct(&r), 0), 1);
	check(&r, "", 0);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
