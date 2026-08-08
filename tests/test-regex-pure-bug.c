/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/regex.h"

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	START();
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "a", 0)));
		regmatch_t pmatch;
		const char *s = "aaa";
		size_t sz = 3;

		/* Under -O2 with LTO, GCC could assume that pmatch is unchanged by a pure-attributed call. */
		pmatch.rm_so = -1;
		pmatch.rm_eo = -1;
		jstr_re_ret_ty ret1 = jstr_re_exec_len(&preg, s, sz, 1, &pmatch, 0);
		assert(ret1 == JSTR_RE_RET_NOERROR);
		assert(pmatch.rm_so == 0);

		pmatch.rm_so = -5;
		pmatch.rm_eo = -5;
		jstr_re_ret_ty ret2 = jstr_re_exec_len(&preg, s, sz, 1, &pmatch, 0);
		assert(ret2 == JSTR_RE_RET_NOERROR);
		assert(pmatch.rm_so == 0);

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "a", 0)));
		regmatch_t pmatch;
		const char *s = "aaa";

		pmatch.rm_so = -1;
		pmatch.rm_eo = -1;
		jstr_re_ret_ty ret1 = jstr_re_exec(&preg, s, 1, &pmatch, 0);
		assert(ret1 == JSTR_RE_RET_NOERROR);
		assert(pmatch.rm_so == 0);

		pmatch.rm_so = -5;
		pmatch.rm_eo = -5;
		jstr_re_ret_ty ret2 = jstr_re_exec(&preg, s, 1, &pmatch, 0);
		assert(ret2 == JSTR_RE_RET_NOERROR);
		assert(pmatch.rm_so == 0);

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "a", 0)));
		regmatch_t pmatch;
		const char *s = "aaa";

		pmatch.rm_so = -1;
		pmatch.rm_eo = -1;
		jstr_re_ret_ty ret1 = jstr_re_search(&preg, s, &pmatch, 0);
		assert(ret1 == JSTR_RE_RET_NOERROR);
		assert(pmatch.rm_so == 0);

		pmatch.rm_so = -5;
		pmatch.rm_eo = -5;
		jstr_re_ret_ty ret2 = jstr_re_search(&preg, s, &pmatch, 0);
		assert(ret2 == JSTR_RE_RET_NOERROR);
		assert(pmatch.rm_so == 0);

		jstr_re_free(&preg);
	}
	SUCCESS();
	return 0;
}
