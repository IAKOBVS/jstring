/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Empty-haystack regex handling: a zero-width REG_STARTEND window makes
 * glibc regexec return NOMATCH even for epsilon-matching patterns ("(.?)*",
 * ".*"), so replacing on an empty string silently did nothing. */

#define __STDC_NO_VLA__ 1

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../include/regex.h"

int
main(void)
{
	jstr_ty result = JSTR_INIT;

	{
		/* rplc(".*", "hello") on "" must insert "hello". */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, ".*", 0)));

		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "", 0)));
		jstr_re_off_ty rn = jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "hello", 5, 0);
		assert(rn == 1);
		assert(result.size == strlen("hello"));
		assert(memcmp(result.data, "hello", result.size + 1) == 0);

		jstr_re_free(&preg);
	}

	{
		/* rplcall of an epsilon pattern on "". NOTE: glibc itself returns
		 * NOMATCH for some nullable patterns like "(.?)*" on "", so use
		 * one it matches ("a*"). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "a*", 0)));

		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "", 0)));
		jstr_re_off_ty rn = jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "x", 1, 0);
		assert(rn == 1);
		assert(result.size == strlen("x"));
		assert(memcmp(result.data, "x", result.size + 1) == 0);

		jstr_re_free(&preg);
	}

	{
		/* Non-epsilon pattern still does not match "". */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "abc", 0)));

		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "", 0)));
		jstr_re_off_ty rn = jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "hello", 5, 0);
		assert(rn == 0);
		assert(result.size == 0);

		jstr_re_free(&preg);
	}

	{
		/* Normal non-empty replacement unaffected. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "b", 0)));

		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "abc", 3)));
		jstr_re_off_ty rn = jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "X", 1, 0);
		assert(rn == 1);
		assert(result.size == 3);
		assert(memcmp(result.data, "aXc", 4) == 0);

		jstr_re_free(&preg);
	}

	jstr_free_j(&result);
	return EXIT_SUCCESS;
}
