/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rplcall_len_exec replaces every match with literal bytes,
 * reports the number of replacements, and preserves buffer invariants. */

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
	RCHECK_EQ(jstr_re_rplcall_len_exec(&re, jstr_struct(&r), "XY", 2, 0), 3);
	check(&r, "bXYnXYnXY", 9);

	/* Same-length replacement preserves the size. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcall_len_exec(&re, jstr_struct(&r), "Z", 1, 0), 3);
	check(&r, "bZnZnZ", 6);

	/* Empty replacement is full removal. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcall_len_exec(&re, jstr_struct(&r), "", 0, 0), 3);
	check(&r, "bnn", 3);

	fill(&r, "aaaa", 4);
	RCHECK_EQ(jstr_re_rplcall_len_exec(&re, jstr_struct(&r), "", 0, 0), 4);
	check(&r, "", 0);

	/* No match: zero, unchanged. */
	fill(&r, "xyz", 3);
	RCHECK_EQ(jstr_re_rplcall_len_exec(&re, jstr_struct(&r), "Q", 1, 0), 0);
	check(&r, "xyz", 3);

	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rplcall_len_exec(&re, jstr_struct(&r), "Q", 1, 0), 0);
	check(&r, "", 0);
	jstr_re_free(&re);

	/* Binary replacement data repeated per match. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a", 0)));
	static const char bin[] = {'Q', '\0'};
	fill(&r, "aaa", 3);
	RCHECK_EQ(jstr_re_rplcall_len_exec(&re, jstr_struct(&r), bin, sizeof(bin), 0), 3);
	static const char bin_out[] = {'Q', '\0', 'Q', '\0', 'Q', '\0'};
	check(&r, bin_out, sizeof(bin_out));
	jstr_re_free(&re);

	/* Multi-byte matches are consumed left to right. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "na", 0)));
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcall_len_exec(&re, jstr_struct(&r), "XY", 2, 0), 2);
	check(&r, "baXYXY", 6);
	jstr_re_free(&re);

	/* Extended class across the whole subject. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "[0-9]", JSTR_RE_CF_EXTENDED)));
	fill(&r, "a1b2c3", 6);
	RCHECK_EQ(jstr_re_rplcall_len_exec(&re, jstr_struct(&r), "N", 1, 0), 3);
	check(&r, "aNbNcN", 6);

	fill(&r, "a1b2c3", 6);
	RCHECK_EQ(jstr_re_rplcall_len_exec(&re, jstr_struct(&r), "", 0, 0), 3);
	check(&r, "abc", 3);
	jstr_re_free(&re);

	/* Case-insensitive replacement. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "A", JSTR_RE_CF_ICASE)));
	fill(&r, "Banana", 6);
	RCHECK_EQ(jstr_re_rplcall_len_exec(&re, jstr_struct(&r), "a", 1, 0), 3);
	check(&r, "Banana", 6);

	fill(&r, "Banana", 6);
	RCHECK_EQ(jstr_re_rplcall_len_exec(&re, jstr_struct(&r), "Z", 1, 0), 3);
	check(&r, "BZnZnZ", 6);
	jstr_re_free(&re);

	/* Epsilon pattern on an empty subject inserts once. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a*", 0)));
	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rplcall_len_exec(&re, jstr_struct(&r), "Z", 1, 0), 1);
	check(&r, "Z", 1);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
