/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: regfree wrapper lifecycle: compile/exec/free, reuse after free and
 * freeing an object whose compilation failed. */

#define __STDC_NO_VLA__ 1

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "../rigor.h"
#include "regex.h"

int
main(void)
{
	jstr_re_ty re;
	memset(&re, 0, sizeof(re));

	/* Compile, use, free. */
	RCHECK_EQ(jstr_re_comp(&re, "hello", 0), JSTR_RE_RET_NOERROR);
	regmatch_t rm;
	RCHECK_EQ(jstr_re_search(&re, "say hello", &rm, 0), JSTR_RE_RET_NOERROR);
	RCHECK_EQ(rm.rm_so, 4);
	RCHECK_EQ(rm.rm_eo, 9);
	jstr_re_free(&re);

	/* The object may be reused after recompilation. */
	RCHECK_EQ(jstr_re_comp(&re, "[0-9]+", JSTR_RE_CF_EXTENDED), JSTR_RE_RET_NOERROR);
	RCHECK_EQ(re.cflags, JSTR_RE_CF_EXTENDED);
	RCHECK_EQ(jstr_re_search(&re, "ab123", &rm, 0), JSTR_RE_RET_NOERROR);
	RCHECK_EQ(rm.rm_so, 2);
	RCHECK_EQ(rm.rm_eo, 5);
	jstr_re_free(&re);

	/* Reuse once more with a different pattern. */
	RCHECK_EQ(jstr_re_comp(&re, "x", 0), JSTR_RE_RET_NOERROR);
	RCHECK_EQ(jstr_re_match(&re, "axb", 0), JSTR_RE_RET_NOERROR);
	RCHECK_EQ(jstr_re_match(&re, "abc", 0), JSTR_RE_RET_NOMATCH);
	jstr_re_free(&re);

	/* Freeing after a failed compilation must not crash. */
	jstr_re_ty bad;
	memset(&bad, 0, sizeof(bad));
	RCHECK(jstr_re_chkcomp(jstr_re_comp(&bad, "[", JSTR_RE_CF_EXTENDED)));
	jstr_re_free(&bad);

	/* Freeing a zeroed object (as if compiled with an empty pattern) is
	 * also supported. */
	jstr_re_ty empty;
	memset(&empty, 0, sizeof(empty));
	RCHECK_EQ(jstr_re_comp(&empty, "", 0), JSTR_RE_RET_NOERROR);
	jstr_re_free(&empty);

	return EXIT_SUCCESS;
}
