/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rmn_exec removes at most n matches, including n == 0,
 * n == 1, partial and full exhaustion, and preserves buffer invariants. */

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

	/* n == 0 is a no-op even when matches exist. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_exec(&re, jstr_struct(&r), 0, 0), 0);
	check(&r, "banana", 6);

	/* n == 1 takes the single-removal fast path. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_exec(&re, jstr_struct(&r), 0, 1), 1);
	check(&r, "bnana", 5);

	/* n == 2 removes the first two matches only. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_exec(&re, jstr_struct(&r), 0, 2), 2);
	check(&r, "bnna", 4);

	/* n larger than the match count removes everything. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_exec(&re, jstr_struct(&r), 0, 100), 3);
	check(&r, "bnn", 3);

	fill(&r, "aaaa", 4);
	RCHECK_EQ(jstr_re_rmn_exec(&re, jstr_struct(&r), 0, 5), 4);
	check(&r, "", 0);

	/* No matches: unchanged regardless of n. */
	fill(&r, "xyz", 3);
	RCHECK_EQ(jstr_re_rmn_exec(&re, jstr_struct(&r), 0, 5), 0);
	check(&r, "xyz", 3);

	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rmn_exec(&re, jstr_struct(&r), 0, 5), 0);
	check(&r, "", 0);
	jstr_re_free(&re);

	/* Overlapping-looking multi-byte matches are consumed left to right. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "na", 0)));
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_exec(&re, jstr_struct(&r), 0, 2), 2);
	check(&r, "ba", 2);

	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_exec(&re, jstr_struct(&r), 0, 1), 1);
	check(&r, "bana", 4);
	jstr_re_free(&re);

	/* Extended class matches. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "[0-9]", JSTR_RE_CF_EXTENDED)));
	fill(&r, "a1b2c3", 6);
	RCHECK_EQ(jstr_re_rmn_exec(&re, jstr_struct(&r), 0, 2), 2);
	check(&r, "abc3", 4);

	fill(&r, "a1b2c3", 6);
	RCHECK_EQ(jstr_re_rmn_exec(&re, jstr_struct(&r), 0, 10), 3);
	check(&r, "abc", 3);
	jstr_re_free(&re);

	/* Epsilon pattern on an empty subject is processed once. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a*", 0)));
	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rmn_exec(&re, jstr_struct(&r), 0, 7), 1);
	check(&r, "", 0);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
