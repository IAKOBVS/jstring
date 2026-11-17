/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rplcn_len_exec replaces at most n matches with literal
 * bytes; covers n == 0/1/partial/exhaustive, empty replacement and binary
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

	/* n == 0 is a no-op. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_exec(&re, jstr_struct(&r), "XY", 2, 0, 0), 0);
	check(&r, "banana", 6);

	/* n == 1 takes the first-match path. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_exec(&re, jstr_struct(&r), "XY", 2, 0, 1), 1);
	check(&r, "bXYnana", 7);

	/* n == 2 replaces the first two matches. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_exec(&re, jstr_struct(&r), "XY", 2, 0, 2), 2);
	check(&r, "bXYnXYna", 8);

	/* n larger than the match count replaces all of them. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_exec(&re, jstr_struct(&r), "XY", 2, 0, 100), 3);
	check(&r, "bXYnXYnXY", 9);

	fill(&r, "aaaa", 4);
	RCHECK_EQ(jstr_re_rplcn_len_exec(&re, jstr_struct(&r), "Z", 1, 0, 3), 3);
	check(&r, "ZZZa", 4);

	/* No match: zero, unchanged. */
	fill(&r, "xyz", 3);
	RCHECK_EQ(jstr_re_rplcn_len_exec(&re, jstr_struct(&r), "Q", 1, 0, 5), 0);
	check(&r, "xyz", 3);
	jstr_re_free(&re);

	/* Empty replacement is removal, still bounded by n. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a", 0)));
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_exec(&re, jstr_struct(&r), "", 0, 0, 2), 2);
	check(&r, "bnna", 4);

	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_exec(&re, jstr_struct(&r), "", 0, 0, 100), 3);
	check(&r, "bnn", 3);

	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_exec(&re, jstr_struct(&r), "", 0, 0, 0), 0);
	check(&r, "banana", 6);
	jstr_re_free(&re);

	/* Binary replacement bytes survive the in-place compaction. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a", 0)));
	static const char bin[] = {'Q', '\0'};
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplcn_len_exec(&re, jstr_struct(&r), bin, sizeof(bin), 0, 2), 2);
	static const char bin_out[] = {'b', 'Q', '\0', 'n', 'Q', '\0', 'n', 'a'};
	check(&r, bin_out, sizeof(bin_out));
	jstr_re_free(&re);

	/* Extended class with a bounded count. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "[0-9]", JSTR_RE_CF_EXTENDED)));
	fill(&r, "a1b2c3", 6);
	RCHECK_EQ(jstr_re_rplcn_len_exec(&re, jstr_struct(&r), "N", 1, 0, 2), 2);
	check(&r, "aNbNc3", 6);

	fill(&r, "a1b2c3", 6);
	RCHECK_EQ(jstr_re_rplcn_len_exec(&re, jstr_struct(&r), "N", 1, 0, 10), 3);
	check(&r, "aNbNcN", 6);
	jstr_re_free(&re);

	/* Epsilon pattern on an empty subject inserts once. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a*", 0)));
	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rplcn_len_exec(&re, jstr_struct(&r), "Z", 1, 0, 5), 1);
	check(&r, "Z", 1);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
