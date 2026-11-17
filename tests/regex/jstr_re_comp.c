/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: compilation across regex features, and documented error codes for
 * malformed patterns. */

#define __STDC_NO_VLA__ 1

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "../rigor.h"
#include "regex.h"

struct valid_case {
	const char *pat;
	int cflags;
};

struct invalid_case {
	const char *pat;
	int cflags;
	jstr_re_ret_ty code;
};

int
main(void)
{
	static const struct valid_case valid[] = {
		{"", 0},
		{"abc", 0},
		{".", 0},
		{"a*", 0},
		{"a\\+", 0},
		{"a\\?", 0},
		{"[a-z]", 0},
		{"[^a-c]", 0},
		{"^abc$", 0},
		{"a\\|b", 0},
		{"\\(ab\\)\\1", 0},
		{"a\\{2,4\\}", 0},
		{"[[:alpha:]]+", JSTR_RE_CF_EXTENDED},
		{"(a|b)*c?", JSTR_RE_CF_EXTENDED},
		{"a{1,3}", JSTR_RE_CF_EXTENDED},
		{"ABC", JSTR_RE_CF_ICASE},
		{"^b$", JSTR_RE_CF_NEWLINE},
		{"(a)(b)\\2\\1", JSTR_RE_CF_EXTENDED},
		{"x|y|z", JSTR_RE_CF_EXTENDED | JSTR_RE_CF_ICASE},
		{"[0-9]+", JSTR_RE_CF_EXTENDED},
	};

	for (size_t i = 0; i < sizeof(valid) / sizeof(valid[0]); ++i) {
		jstr_re_ty re;
		memset(&re, 0xAA, sizeof(re));
		const jstr_re_ret_ty ret = jstr_re_comp(&re, valid[i].pat, valid[i].cflags);
		RCHECK_EQ(ret, JSTR_RE_RET_NOERROR);
		RCHECK(!jstr_re_chkcomp(ret));
		RCHECK_EQ(re.cflags, valid[i].cflags);
		/* The object may be reused after being freed. */
		jstr_re_free(&re);
		RCHECK_EQ(jstr_re_comp(&re, "abc", 0), JSTR_RE_RET_NOERROR);
		jstr_re_free(&re);
	}

	static const struct invalid_case invalid[] = {
		{"[", JSTR_RE_CF_EXTENDED, JSTR_RE_RET_BADPAT},
		{"[a", JSTR_RE_CF_EXTENDED, JSTR_RE_RET_EBRACK},
		{"(", JSTR_RE_CF_EXTENDED, JSTR_RE_RET_EPAREN},
		{"(a", JSTR_RE_CF_EXTENDED, JSTR_RE_RET_EPAREN},
		{"a{2,1}", JSTR_RE_CF_EXTENDED, JSTR_RE_RET_BADBR},
		{"a{1", JSTR_RE_CF_EXTENDED, JSTR_RE_RET_EBRACE},
		{"*", JSTR_RE_CF_EXTENDED, JSTR_RE_RET_BADRPT},
		{"+a", JSTR_RE_CF_EXTENDED, JSTR_RE_RET_BADRPT},
		{"\\", JSTR_RE_CF_EXTENDED, JSTR_RE_RET_EESCAPE},
		{"\\", 0, JSTR_RE_RET_EESCAPE},
		{"[z-a]", JSTR_RE_CF_EXTENDED, JSTR_RE_RET_ERANGE},
		{"[[:foo:]]", JSTR_RE_CF_EXTENDED, JSTR_RE_RET_ECTYPE},
		{"\\1", JSTR_RE_CF_EXTENDED, JSTR_RE_RET_ESUBREG},
		{"(a)\\2", JSTR_RE_CF_EXTENDED, JSTR_RE_RET_ESUBREG},
		{"[", 0, JSTR_RE_RET_BADPAT},
		{"\\(a", 0, JSTR_RE_RET_EPAREN},
	};

	for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
		jstr_re_ty re;
		memset(&re, 0, sizeof(re));
		const jstr_re_ret_ty ret = jstr_re_comp(&re, invalid[i].pat, invalid[i].cflags);
		if (ret != invalid[i].code) {
			fprintf(stderr, "pattern #%zu \"%s\" cflags=%d: got %d want %d\n",
			        i, invalid[i].pat, invalid[i].cflags, (int)ret, (int)invalid[i].code);
		}
		RCHECK_EQ(ret, invalid[i].code);
		RCHECK(jstr_re_chkcomp(ret));
		RCHECK(ret != JSTR_RE_RET_NOERROR);
		/* cflags is recorded before regcomp runs. */
		RCHECK_EQ(re.cflags, invalid[i].cflags);
		jstr_re_free(&re);
	}

	/* chkexec distinguishes NOMATCH (not an error) from real errors. */
	RCHECK(!jstr_re_chkexec(JSTR_RE_RET_NOERROR));
	RCHECK(!jstr_re_chkexec(JSTR_RE_RET_NOMATCH));
	RCHECK(jstr_re_chkexec(JSTR_RE_RET_BADPAT));
	/* Regex return codes are positive errno-like values, so jstr_re_chk()
	 * (which tests for negative library return codes) is false here. */
	RCHECK(!jstr_re_chk(JSTR_RE_RET_EBRACK));
	RCHECK(jstr_re_chkcomp(JSTR_RE_RET_EBRACK));

	return EXIT_SUCCESS;
}
