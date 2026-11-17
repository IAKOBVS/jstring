/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rmn_from_exec combines the start_idx window with an n
 * limit; covers 0/middle/end/out-of-range offsets, n == 0, partial and
 * complete exhaustion, and buffer invariants. */

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

	/* n == 0 never changes anything, even from a middle offset. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_from_exec(&re, jstr_struct(&r), 2, 0, 0), 0);
	check(&r, "banana", 6);

	/* n == 1 from offset 2 removes the match at index 3. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_from_exec(&re, jstr_struct(&r), 2, 0, 1), 1);
	check(&r, "banna", 5);

	/* n == 2 from offset 2 removes indices 3 and 5. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_from_exec(&re, jstr_struct(&r), 2, 0, 2), 2);
	check(&r, "bann", 4);

	/* n exceeding the remaining matches removes them all. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_from_exec(&re, jstr_struct(&r), 2, 0, 100), 2);
	check(&r, "bann", 4);

	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_from_exec(&re, jstr_struct(&r), 0, 0, 100), 3);
	check(&r, "bnn", 3);

	fill(&r, "aaaa", 4);
	RCHECK_EQ(jstr_re_rmn_from_exec(&re, jstr_struct(&r), 1, 0, 100), 3);
	check(&r, "a", 1);

	/* start_idx == size and start_idx > size are no-ops. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_from_exec(&re, jstr_struct(&r), 6, 0, 100), 0);
	check(&r, "banana", 6);

	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_from_exec(&re, jstr_struct(&r), 9, 0, 100), 0);
	check(&r, "banana", 6);

	/* No match after the offset. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_from_exec(&re, jstr_struct(&r), 0, 0, 0), 0);
	check(&r, "banana", 6);
	jstr_re_free(&re);

	/* Multi-byte matches from an offset. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "na", 0)));
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_from_exec(&re, jstr_struct(&r), 2, 0, 100), 2);
	check(&r, "ba", 2);

	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rmn_from_exec(&re, jstr_struct(&r), 3, 0, 1), 1);
	check(&r, "bana", 4);
	jstr_re_free(&re);

	/* Empty subject with epsilon pattern runs at most once. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a*", 0)));
	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rmn_from_exec(&re, jstr_struct(&r), 0, 0, 9), 1);
	check(&r, "", 0);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
