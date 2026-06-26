/* SPDX-License-Identifier: MIT */
#define __STDC_NO_VLA__ 1
#include "test.h"
#include "../include/regex.h"

int
main(int argc, char **argv)
{
	START();
	jstr_ty result = JSTR_INIT;
	/* Test 19: colou?r ERE on "color colour" */
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "colou?r", JSTR_RE_CF_EXTENDED)));
		result.data = strdup("color colour");
		result.size = 12;
		result.capacity = 13;
		/* Reserve enough for the replacement */
		size_t sz = 12, cap = 13;
		char *s = result.data;
		jstr_re_off_ty ret = jstr_re_rplcall_len_exec(&preg, &s, &sz, &cap, "X", 1, 0);
		/* Adopt returned pointer into result so jstr_free_j() frees it once. */
		result.data = s;
		result.size = sz;
		result.capacity = cap;
		jstr_re_free(&preg);
	}
	jstr_free_j(&result);
	SUCCESS();
	return 0;
	(void)argc;
}
