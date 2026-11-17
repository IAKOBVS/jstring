/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_rplc_len_exec replaces exactly the first match with literal
 * replacement bytes (including embedded NULs and empty replacements). */

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
	RCHECK_EQ(jstr_re_rplc_len_exec(&re, jstr_struct(&r), "XY", 2, 0), 1);
	check(&r, "bXYnana", 7);

	/* Same-length replacement. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplc_len_exec(&re, jstr_struct(&r), "Z", 1, 0), 1);
	check(&r, "bZnana", 6);

	/* Empty replacement removes the match. */
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplc_len_exec(&re, jstr_struct(&r), "", 0, 0), 1);
	check(&r, "bnana", 5);

	/* No match: untouched, zero reported. */
	fill(&r, "xyz", 3);
	RCHECK_EQ(jstr_re_rplc_len_exec(&re, jstr_struct(&r), "Q", 1, 0), 0);
	check(&r, "xyz", 3);

	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rplc_len_exec(&re, jstr_struct(&r), "Q", 1, 0), 0);
	check(&r, "", 0);

	/* Match at the end. */
	fill(&r, "xyza", 4);
	RCHECK_EQ(jstr_re_rplc_len_exec(&re, jstr_struct(&r), "BCD", 3, 0), 1);
	check(&r, "xyzBCD", 6);
	jstr_re_free(&re);

	/* Embedded NUL byte in the replacement is preserved verbatim. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a", 0)));
	static const char bin[] = {'\0', 'X'};
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplc_len_exec(&re, jstr_struct(&r), bin, sizeof(bin), 0), 1);
	static const char bin_out[] = {'b', '\0', 'X', 'n', 'a', 'n', 'a'};
	check(&r, bin_out, sizeof(bin_out));
	jstr_re_free(&re);

	/* Case-insensitive match. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "A", JSTR_RE_CF_ICASE)));
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplc_len_exec(&re, jstr_struct(&r), "Q", 1, 0), 1);
	check(&r, "bQnana", 6);
	jstr_re_free(&re);

	/* Multi-byte match. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "na", 0)));
	fill(&r, "banana", 6);
	RCHECK_EQ(jstr_re_rplc_len_exec(&re, jstr_struct(&r), "XY", 2, 0), 1);
	check(&r, "baXYna", 6);
	jstr_re_free(&re);

	/* Epsilon pattern on an empty subject inserts once. */
	RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "a*", 0)));
	fill(&r, "", 0);
	RCHECK_EQ(jstr_re_rplc_len_exec(&re, jstr_struct(&r), "Z", 1, 0), 1);
	check(&r, "Z", 1);
	jstr_re_free(&re);

	jstr_free_j(&r);
	return EXIT_SUCCESS;
}
