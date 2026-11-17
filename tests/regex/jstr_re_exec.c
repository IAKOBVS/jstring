/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_exec over a feature/flag matrix with exact match spans,
 * group filling, nmatch truncation and a literal differential property. */

#define __STDC_NO_VLA__ 1

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "../rigor.h"
#include "regex.h"

struct span_case {
	const char *pat;
	int cflags;
	const char *subj;
	int eflags;
	int match;
	regoff_t so;
	regoff_t eo;
};

static void
run_case(const struct span_case *c)
{
	jstr_re_ty re;
	memset(&re, 0, sizeof(re));
	RCHECK_EQ(jstr_re_comp(&re, c->pat, c->cflags), JSTR_RE_RET_NOERROR);
	regmatch_t pm[1];
	pm[0].rm_so = -7;
	pm[0].rm_eo = -7;
	const jstr_re_ret_ty ret = jstr_re_exec(&re, c->subj, 1, pm, c->eflags);
	if (c->match) {
		if (ret != JSTR_RE_RET_NOERROR || pm[0].rm_so != c->so || pm[0].rm_eo != c->eo) {
			fprintf(stderr, "pat=\"%s\" subj=\"%s\": ret=%d span=%ld..%ld want %ld..%ld\n",
			        c->pat, c->subj, (int)ret, (long)pm[0].rm_so, (long)pm[0].rm_eo,
			        (long)c->so, (long)c->eo);
		}
		RCHECK_EQ(ret, JSTR_RE_RET_NOERROR);
		RCHECK_EQ(pm[0].rm_so, c->so);
		RCHECK_EQ(pm[0].rm_eo, c->eo);
	} else {
		RCHECK_EQ(ret, JSTR_RE_RET_NOMATCH);
	}
	jstr_re_free(&re);
}

static long
naive_find(const char *h, size_t hl, const char *n, size_t nl)
{
	if (nl == 0)
		return 0;
	if (nl > hl)
		return -1;
	for (size_t i = 0; i + nl <= hl; ++i)
		if (memcmp(h + i, n, nl) == 0)
			return (long)i;
	return -1;
}

int
main(void)
{
	static const struct span_case cases[] = {
		{"abc", 0, "xxabcyy", 0, 1, 2, 5},
		{"a.c", 0, "xxaXc", 0, 1, 2, 5},
		{"a*", 0, "bbb", 0, 1, 0, 0},
		{"^abc", 0, "abcdef", 0, 1, 0, 3},
		{"^abc", 0, "zabc", 0, 0, 0, 0},
		{"abc$", 0, "zabc", 0, 1, 1, 4},
		{"abc$", 0, "abcz", 0, 0, 0, 0},
		{"a\\|b", 0, "xxb", 0, 1, 2, 3},
		{"\\(ab\\)\\(cd\\)", 0, "zzabcd", 0, 1, 2, 6},
		{"[0-9]\\{2,3\\}", 0, "a12345", 0, 1, 1, 4},
		{"[^a-c]", 0, "abXc", 0, 1, 2, 3},
		{"colou\\?r", 0, "colour", 0, 1, 0, 6},
		{".", 0, "", 0, 0, 0, 0},
		{"", 0, "abc", 0, 1, 0, 0},
		{"", 0, "", 0, 1, 0, 0},
		{"b", 0, "abc", 0, 1, 1, 2},
		{"x\\|y\\|z", 0, "..y", 0, 1, 2, 3},
		{"a\\+", 0, "baaa", 0, 1, 1, 4},
		{"a\\{2,3\\}", 0, "ba", 0, 0, 0, 0},
		{"[[:digit:]]+", JSTR_RE_CF_EXTENDED, "x42y", 0, 1, 1, 3},
	};

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
		run_case(&cases[i]);

	/* Flag interactions. */
	static const struct span_case flag_cases[] = {
		{"ABC", JSTR_RE_CF_ICASE, "xxabc", 0, 1, 2, 5},
		{"ABC", 0, "xxabc", 0, 0, 0, 0},
		{"^b", JSTR_RE_CF_NEWLINE, "a\nb", 0, 1, 2, 3},
		{"^b", 0, "a\nb", 0, 0, 0, 0},
		{"b$", JSTR_RE_CF_NEWLINE, "b\na", 0, 1, 0, 1},
		{"^a", 0, "a", JSTR_RE_EF_NOTBOL, 0, 0, 0},
		{"a$", 0, "a", JSTR_RE_EF_NOTEOL, 0, 0, 0},
		{"^a", 0, "a", 0, 1, 0, 1},
		{"a$", 0, "a", 0, 1, 0, 1},
	};
	for (size_t i = 0; i < sizeof(flag_cases) / sizeof(flag_cases[0]); ++i)
		run_case(&flag_cases[i]);

	/* Group spans and non-participating groups. */
	{
		jstr_re_ty re;
		RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "(a)(b)", JSTR_RE_CF_EXTENDED)));
		regmatch_t pm[5];
		memset(pm, 0, sizeof(pm));
		RCHECK_EQ(jstr_re_exec(&re, "zab", 5, pm, 0), JSTR_RE_RET_NOERROR);
		RCHECK_EQ(pm[0].rm_so, 1);
		RCHECK_EQ(pm[0].rm_eo, 3);
		RCHECK_EQ(pm[1].rm_so, 1);
		RCHECK_EQ(pm[1].rm_eo, 2);
		RCHECK_EQ(pm[2].rm_so, 2);
		RCHECK_EQ(pm[2].rm_eo, 3);
		RCHECK(pm[3].rm_so < 0);
		RCHECK(pm[4].rm_so < 0);
		jstr_re_free(&re);
	}
	{
		/* nmatch smaller than group count keeps the first entries only. */
		jstr_re_ty re;
		RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "(a)(b)", JSTR_RE_CF_EXTENDED)));
		regmatch_t pm[2];
		memset(pm, 0, sizeof(pm));
		RCHECK_EQ(jstr_re_exec(&re, "zab", 2, pm, 0), JSTR_RE_RET_NOERROR);
		RCHECK_EQ(pm[0].rm_so, 1);
		RCHECK_EQ(pm[0].rm_eo, 3);
		RCHECK_EQ(pm[1].rm_so, 1);
		RCHECK_EQ(pm[1].rm_eo, 2);
		/* nmatch == 0 with pmatch == NULL reports match only. */
		RCHECK_EQ(jstr_re_exec(&re, "zab", 0, NULL, 0), JSTR_RE_RET_NOERROR);
		RCHECK_EQ(jstr_re_exec(&re, "zzz", 0, NULL, 0), JSTR_RE_RET_NOMATCH);
		jstr_re_free(&re);
	}
	{
		jstr_re_ty re;
		RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "(a)|(b)", JSTR_RE_CF_EXTENDED)));
		regmatch_t pm[3];
		memset(pm, 0, sizeof(pm));
		RCHECK_EQ(jstr_re_exec(&re, "zb", 3, pm, 0), JSTR_RE_RET_NOERROR);
		RCHECK_EQ(pm[0].rm_so, 1);
		RCHECK_EQ(pm[0].rm_eo, 2);
		RCHECK(pm[1].rm_so < 0); /* group 1 did not participate */
		RCHECK_EQ(pm[2].rm_so, 1);
		RCHECK_EQ(pm[2].rm_eo, 2);
		jstr_re_free(&re);
	}

	/* Differential: literal patterns must report the same first span as a
	 * naive substring scan. Exhaustive lengths only in slow mode. */
	rigor_srand(0x5eed1234u);
	char subj[1100];
	char needle[8];
	size_t iterations = 0;
	for (size_t i = 0; i < RIGOR_NLENS; ++i) {
		if (!JSTR_TEST_SLOW && (i % 4) != 0)
			continue;
		const size_t hl = rigor_lens[i];
		for (size_t k = 0; k < hl; ++k)
			subj[k] = (char)('a' + rigor_randn(26));
		subj[hl] = '\0';
		const size_t nl = 1 + rigor_randn(hl ? JSTR_MIN(hl, 6) : 1);
		for (size_t k = 0; k < nl; ++k)
			needle[k] = (char)('a' + rigor_randn(26));
		needle[nl] = '\0';
		jstr_re_ty re;
		RCHECK_EQ(jstr_re_comp(&re, needle, 0), JSTR_RE_RET_NOERROR);
		regmatch_t pm[1];
		const long want = naive_find(subj, hl, needle, nl);
		const jstr_re_ret_ty ret = jstr_re_exec(&re, subj, 1, pm, 0);
		if (want < 0) {
			RCHECK_EQ(ret, JSTR_RE_RET_NOMATCH);
		} else {
			RCHECK_EQ(ret, JSTR_RE_RET_NOERROR);
			RCHECK_EQ(pm[0].rm_so, (regoff_t)want);
			RCHECK_EQ(pm[0].rm_eo, (regoff_t)(want + (long)nl));
		}
		jstr_re_free(&re);
		++iterations;
	}
	RCHECK(iterations >= 8);

	return EXIT_SUCCESS;
}
