/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: REG_NOSUB compile flag. POSIX still requires match/no-match results;
 * match slots are unspecified, so no span values are asserted here. */

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
	assert(!jstr_re_chkcomp(jstr_re_comp(&re, "\\(a\\)\\(b\\)", REG_NOSUB)));

	RCHECK_EQ(jstr_re_exec(&re, "ab", 0, NULL, 0), JSTR_RE_RET_NOERROR);
	RCHECK_EQ(jstr_re_exec(&re, "zz", 0, NULL, 0), JSTR_RE_RET_NOMATCH);
	RCHECK_EQ(jstr_re_match(&re, "xxabyy", 0), JSTR_RE_RET_NOERROR);
	RCHECK_EQ(jstr_re_search(&re, "zzab", NULL, 0), JSTR_RE_RET_NOERROR);

	jstr_re_free(&re);
	return 0;
}
