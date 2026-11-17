/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_search_len with REG_STARTEND windows. Verifies that sz,
 * not the NUL terminator, bounds the search, including anchors and flags,
 * and cross-checks literal patterns against a naive bounded scanner. */

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
	size_t sz;
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
	pm[0].rm_so = pm[0].rm_eo = -7;
	const jstr_re_ret_ty ret = jstr_re_search_len(&re, c->subj, c->sz, pm, c->eflags);
	if (c->match) {
		if (ret != JSTR_RE_RET_NOERROR || pm[0].rm_so != c->so || pm[0].rm_eo != c->eo) {
			fprintf(stderr,
			        "pat=\"%s\" subj=\"%s\" sz=%zu: ret=%d span=%ld..%ld want %ld..%ld\n",
			        c->pat, c->subj, c->sz, (int)ret, (long)pm[0].rm_so,
			        (long)pm[0].rm_eo, (long)c->so, (long)c->eo);
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
		{"abc", 0, "abcdef", 6, 0, 1, 0, 3},
		{"abc", 0, "abcdef", 3, 0, 1, 0, 3},
		{"abc", 0, "abcdef", 2, 0, 0, 0, 0},
		{"bc", 0, "abcdef", 2, 0, 0, 0, 0},
		{"bc", 0, "abcdef", 3, 0, 1, 1, 3},
		{"c$", 0, "abcdef", 3, 0, 1, 2, 3},
		{"c$", 0, "abcdef", 6, 0, 0, 0, 0},
		{"f$", 0, "abcdef", 6, 0, 1, 5, 6},
		{"^abc", 0, "abcdef", 6, 0, 1, 0, 3},
		{"^bc", 0, "abcdef", 6, 0, 0, 0, 0},
		{"a.c", 0, "xxaXc", 5, 0, 1, 2, 5},
		{"a*", 0, "bbb", 3, 0, 1, 0, 0},
		{"a*", 0, "", 0, 0, 1, 0, 0},
		{".", 0, "", 0, 0, 0, 0, 0},
		{"", 0, "", 0, 0, 1, 0, 0},
		{"", 0, "abc", 3, 0, 1, 0, 0},
		{"[0-9]+", JSTR_RE_CF_EXTENDED, "ab123", 5, 0, 1, 2, 5},
		{"[0-9]+", JSTR_RE_CF_EXTENDED, "ab123", 4, 0, 1, 2, 4},
		{"a\\|b", 0, "xxb", 3, 0, 1, 2, 3},
		{"[^a-c]", 0, "abXc", 4, 0, 1, 2, 3},
		{"(ab)(cd)", JSTR_RE_CF_EXTENDED, "zzabcd", 6, 0, 1, 2, 6},
	};
	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
		run_case(&cases[i]);

	static const struct span_case flag_cases[] = {
		{"ABC", JSTR_RE_CF_ICASE, "xxabc", 5, 0, 1, 2, 5},
		{"ABC", 0, "xxabc", 5, 0, 0, 0, 0},
		{"^b", JSTR_RE_CF_NEWLINE, "a\nb", 3, 0, 1, 2, 3},
		{"^b", 0, "a\nb", 3, 0, 0, 0, 0},
		{"b$", JSTR_RE_CF_NEWLINE, "b\na", 3, 0, 1, 0, 1},
		{"a$", JSTR_RE_CF_NEWLINE, "b\na", 3, 0, 1, 2, 3},
		{"^a", 0, "a", 1, JSTR_RE_EF_NOTBOL, 0, 0, 0},
		{"a$", 0, "a", 1, JSTR_RE_EF_NOTEOL, 0, 0, 0},
		{"^a", 0, "a", 1, 0, 1, 0, 1},
		{"a$", 0, "a", 1, 0, 1, 0, 1},
	};
	for (size_t i = 0; i < sizeof(flag_cases) / sizeof(flag_cases[0]); ++i)
		run_case(&flag_cases[i]);

	/* A window ending mid-match must not report the partial match. */
	{
		jstr_re_ty re;
		RCHECK(!jstr_re_chkcomp(jstr_re_comp(&re, "abcabc", 0)));
		regmatch_t pm[1];
		RCHECK_EQ(jstr_re_search_len(&re, "abcabc", 6, pm, 0), JSTR_RE_RET_NOERROR);
		RCHECK_EQ(pm[0].rm_so, 0);
		RCHECK_EQ(pm[0].rm_eo, 6);
		RCHECK_EQ(jstr_re_search_len(&re, "abcabc", 5, pm, 0), JSTR_RE_RET_NOMATCH);
		RCHECK_EQ(jstr_re_search_len(&re, "abcabc", 2, pm, 0), JSTR_RE_RET_NOMATCH);
		jstr_re_free(&re);
	}

	/* Differential: literal patterns bounded by a window agree with a naive
	 * scan of exactly that many bytes. Exhaustive lengths only when slow. */
	rigor_srand(0x0badf00dU);
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
		const size_t windows[2] = {hl, hl / 2};
		for (size_t w = 0; w < 2; ++w) {
			const size_t win = windows[w];
			regmatch_t pm[1];
			pm[0].rm_so = pm[0].rm_eo = -1;
			const long want = naive_find(subj, win, needle, nl);
			const jstr_re_ret_ty ret = jstr_re_search_len(&re, subj, win, pm, 0);
			if (want < 0) {
				RCHECK_EQ(ret, JSTR_RE_RET_NOMATCH);
			} else {
				RCHECK_EQ(ret, JSTR_RE_RET_NOERROR);
				RCHECK_EQ(pm[0].rm_so, (regoff_t)want);
				RCHECK_EQ(pm[0].rm_eo, (regoff_t)(want + (long)nl));
			}
		}
		jstr_re_free(&re);
		++iterations;
	}
	RCHECK(iterations >= 8);

	return EXIT_SUCCESS;
}
