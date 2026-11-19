/* SPDX-License-Identifier: MIT */
#define __STDC_NO_VLA__ 1
#include "test.h"
#include "../include/regex.h"

int
main(void)
{
	jstr_ty result = JSTR_INIT;

	/* colou?r ERE on "color colour" — rplcall */
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "colou?r", JSTR_RE_CF_EXTENDED)));
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "color colour", 12)));
		jstr_re_off_ty ret = jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "X", 1, 0);
		assert(ret == 2);
		test_expect_buf("jstr_re_rplcall_len_exec", &result, "X X");
		jstr_re_free(&preg);
	}
	/* colou?r — single replace from start_idx (first match only). */
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "colou?r", JSTR_RE_CF_EXTENDED)));
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "color colour", 12)));
		jstr_re_off_ty ret = jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "X", 1, 0);
		assert(ret == 1);
		test_expect_buf("jstr_re_rplc_len_exec", &result, "X colour");
		jstr_re_free(&preg);
	}
	/* colou?r — _from_exec skipping the first match. */
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "colou?r", JSTR_RE_CF_EXTENDED)));
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "color colour", 12)));
		jstr_re_off_ty ret = jstr_re_rplc_len_from_exec(&preg, jstr_struct(&result), 6, "X", 1, 0);
		assert(ret == 1);
		test_expect_buf("jstr_re_rplc_len_from_exec", &result, "color X");
		jstr_re_free(&preg);
	}
	jstr_free_j(&result);
	return 0;
}
