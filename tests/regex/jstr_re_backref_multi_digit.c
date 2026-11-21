/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: multi-digit backreference templates. jstr parses "\10" as backref
 * \1 followed by a literal '0' (REVIEW.md R5). */

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
	assert(!jstr_re_chkcomp(jstr_re_comp(&re, "\\(a\\)", 0)));

	jstr_ty r = JSTR_INIT;
	RCHECK(!jstr_chk(jstr_assign_len(jstr_struct(&r), "aa", 2)));
	jstr_re_off_ty ret = jstr_re_rplcall_backref_len_exec(&re, jstr_struct(&r), "\\10", 3, 0, 3);
	RCHECK_EQ(ret, 2);
	RCHECK_EQ(r.size, 4);
	RCHECK_MEM(r.data, "a0a0", 4);
	RCHECK_EQ(r.data[r.size], '\0');

	jstr_free_j(&r);
	jstr_re_free(&re);
	return 0;
}
