/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rmall_exec removes every match, reports the count, and
 * preserves NUL/size/capacity invariants. */

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
	RCHECK_EQ(jstr_re_rmall_exec(&re, jstr_struct(&r), 0), 3);
	check(&r, "bnn", 3);

	fill(&r, "aaaa", 4);
	RCHECK_EQ(jstr_re_rmall_exec(&re, jstr_struct(&r), 0), 4);
	check(&r, "", 0);

	fill(&r, "a", 1);
	RCHECK_EQ(jstr_re_rmall_exec(&re, jstr_struct(&r), 0), 1);
	check(&r, "", 0);

	/* No matches: zero changed, byte-identical result. */
	fill(&r, "xyz", 3);
	RCHECK_EQ(jstr_re_rmall_exec(&re, jstr_struct(&r), 0), 0);
	check(&r, "xyz", 3);

	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rmall_exec(&re, jstr_struct(&r), 0), 0);
	check(&r, "", 0);
	jstr_re_free(&re);

	/* Multi-byte matches are consumed left to right. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "na", 0)));
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmall_exec(&re, jstr_struct(&r), 0), 2);
	check(&r, "ba", 2);
	jstr_re_free(&re);

	/* Vowel stripping through an extended character class. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "[aeiou]", JSTR_RE_CF_EXTENDED)));
	fill(&r, "education", 9);
	RCHECK_EQ(jstr_re_rmall_exec(&re, jstr_struct(&r), 0), 5);
	check(&r, "dctn", 4);

	fill(&r, "xyz", 3);
	RCHECK_EQ(jstr_re_rmall_exec(&re, jstr_struct(&r), 0), 0);
	check(&r, "xyz", 3);
	jstr_re_free(&re);

	/* Case-insensitive removal. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "A", JSTR_RE_CF_ICASE)));
	fill(&r, "Banana", 6);
	RCHECK_EQ(jstr_re_rmall_exec(&re, jstr_struct(&r), 0), 3);
	check(&r, "Bnn", 3);
	jstr_re_free(&re);

	/* Epsilon pattern on an empty subject runs at most once. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a*", 0)));
	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rmall_exec(&re, jstr_struct(&r), 0), 1);
	check(&r, "", 0);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
