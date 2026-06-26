/* SPDX-License-Identifier: MIT */
#define __STDC_NO_VLA__ 1
#include "test.h"
#include "../include/regex.h"

#define T_ASSERT_BREF(func, result, expected)                                                                            \
	do {                                                                                                           \
		ASSERT_RESULT(func, (result).size == strlen(expected), (result).data, expected);                       \
		ASSERT_RESULT(func, memcmp((result).data, expected, (result).size) == 0, (result).data, expected);     \
	} while (0)

int
main(int argc, char **argv)
{
	START();
	jstr_ty result = JSTR_INIT;

	/* colou?r ERE on "color colour" — rplcall */
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "colou?r", JSTR_RE_CF_EXTENDED)));
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "color colour", 12)));
		jstr_re_off_ty ret = jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "X", 1, 0);
		assert(ret == 2);
		T_ASSERT_BREF(jstr_re_rplcall_len_exec, result, "X X");
		jstr_re_free(&preg);
	}
	/* colou?r — single replace from start_idx (first match only). */
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "colou?r", JSTR_RE_CF_EXTENDED)));
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "color colour", 12)));
		jstr_re_off_ty ret = jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "X", 1, 0);
		assert(ret == 1);
		T_ASSERT_BREF(jstr_re_rplc_len_exec, result, "X colour");
		jstr_re_free(&preg);
	}
	/* colou?r — _from_exec skipping the first match. */
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "colou?r", JSTR_RE_CF_EXTENDED)));
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "color colour", 12)));
		jstr_re_off_ty ret = jstr_re_rplc_len_from_exec(&preg, jstr_struct(&result), 6, "X", 1, 0);
		assert(ret == 1);
		T_ASSERT_BREF(jstr_re_rplc_len_from_exec, result, "color X");
		jstr_re_free(&preg);
	}
	jstr_free_j(&result);
	SUCCESS();
	return 0;
	(void)argc;
#undef T_ASSERT_BREF
}
