/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_match (nmatch == 0) across literals, quantifiers, classes,
 * anchors, alternation, groups and execution flags, plus a literal
 * differential property. */

#define __STDC_NO_VLA__ 1

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "../rigor.h"
#include "regex.h"

struct match_case {
	const char *pat;
	int cflags;
	const char *subj;
	int eflags;
	int match;
};

static void
run_case(const struct match_case *c)
{
	jstr_re_ty re;
	memset(&re, 0, sizeof(re));
	RCHECK_EQ(jstr_re_comp(&re, c->pat, c->cflags), JSTR_RE_RET_NOERROR);
	const jstr_re_ret_ty ret = jstr_re_match(&re, c->subj, c->eflags);
	if (c->match)
		RCHECK_EQ(ret, JSTR_RE_RET_NOERROR);
	else
		RCHECK_EQ(ret, JSTR_RE_RET_NOMATCH);
	/* chkexec treats NOMATCH specially; neither outcome is an error. */
	RCHECK(!jstr_re_chkexec(ret));
	jstr_re_free(&re);
}

static int
naive_contains(const char *h, size_t hl, const char *n, size_t nl)
{
	if (nl == 0)
		return 1;
	if (nl > hl)
		return 0;
	for (size_t i = 0; i + nl <= hl; ++i)
		if (memcmp(h + i, n, nl) == 0)
			return 1;
	return 0;
}

int
main(void)
{
	static const struct match_case cases[] = {
		{"abc", 0, "xxabcyy", 0, 1},
		{"abc", 0, "xxabxy", 0, 0},
		{"a.c", 0, "xxaXc", 0, 1},
		{"a.c", 0, "xxac", 0, 0},
		{"a*", 0, "bbb", 0, 1},
		{"^abc", 0, "abcdef", 0, 1},
		{"^abc", 0, "zabc", 0, 0},
		{"abc$", 0, "zabc", 0, 1},
		{"abc$", 0, "abcz", 0, 0},
		{"a\\|b", 0, "xxb", 0, 1},
		{"\\(ab\\)\\(cd\\)", 0, "zzabcd", 0, 1},
		{"[0-9]\\{2,3\\}", 0, "a12345", 0, 1},
		{"[0-9]\\{4,5\\}", 0, "a123", 0, 0},
		{"[^a-c]", 0, "abc", 0, 0},
		{"[^a-c]", 0, "abXc", 0, 1},
		{"colou\\?r", 0, "colour", 0, 1},
		{"colou\\?r", 0, "color", 0, 1},
		{".", 0, "", 0, 0},
		{"", 0, "", 0, 1},
		{"", 0, "abc", 0, 1},
		{"b", 0, "abc", 0, 1},
		{"x\\|y\\|z", 0, "..y", 0, 1},
		{"a\\+", 0, "baaa", 0, 1},
		{"a\\+", 0, "bbb", 0, 0},
		{"[[:digit:]]+", JSTR_RE_CF_EXTENDED, "x42y", 0, 1},
		{"(foo|bar)baz", JSTR_RE_CF_EXTENDED, "..barbaz", 0, 1},
		{"(foo|bar)baz", JSTR_RE_CF_EXTENDED, "bazbar", 0, 0},
	};
	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
		run_case(&cases[i]);

	static const struct match_case flag_cases[] = {
		{"ABC", JSTR_RE_CF_ICASE, "xxabc", 0, 1},
		{"ABC", 0, "xxabc", 0, 0},
		{"^b", JSTR_RE_CF_NEWLINE, "a\nb", 0, 1},
		{"^b", 0, "a\nb", 0, 0},
		{"b$", JSTR_RE_CF_NEWLINE, "b\na", 0, 1},
		{"^a", 0, "a", JSTR_RE_EF_NOTBOL, 0, 0},
		{"a$", 0, "a", JSTR_RE_EF_NOTEOL, 0, 0},
		{"^a", 0, "a", 0, 1},
		{"a$", 0, "a", 0, 1},
	};
	for (size_t i = 0; i < sizeof(flag_cases) / sizeof(flag_cases[0]); ++i)
		run_case(&flag_cases[i]);

	/* Differential: literal patterns must agree with a naive containment
	 * scan. Exhaustive lengths only in slow mode. */
	rigor_srand(0xfeedfaceU);
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
		const int want = naive_contains(subj, hl, needle, nl);
		const jstr_re_ret_ty ret = jstr_re_match(&re, subj, 0);
		if (want)
			RCHECK_EQ(ret, JSTR_RE_RET_NOERROR);
		else
			RCHECK_EQ(ret, JSTR_RE_RET_NOMATCH);
		jstr_re_free(&re);
		++iterations;
	}
	RCHECK(iterations >= 8);

	return EXIT_SUCCESS;
}
