/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/regex.h"

int
main(int argc, char **argv)
{
	START();
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "world", 0)));

		regmatch_t pmatch[1];

		/* 1. Test jstr_re_exec */
		pmatch[0].rm_so = -1;
		pmatch[0].rm_eo = -1;
		assert(jstr_re_exec(&preg, "hello world", 1, pmatch, 0) == JSTR_RE_RET_NOERROR);
		assert(pmatch[0].rm_so == 6);
		assert(pmatch[0].rm_eo == 11);

		pmatch[0].rm_so = -2;
		pmatch[0].rm_eo = -2;
		/* Call again with identical arguments. If pure attribute is set,
		 * the compiler is allowed to assume no side effects occur and may
		 * optimize away this second call, failing to update pmatch. */
		assert(jstr_re_exec(&preg, "hello world", 1, pmatch, 0) == JSTR_RE_RET_NOERROR);
		assert(pmatch[0].rm_so == 6);
		assert(pmatch[0].rm_eo == 11);

		/* 2. Test jstr_re_exec_len */
		pmatch[0].rm_so = -1;
		pmatch[0].rm_eo = -1;
		assert(jstr_re_exec_len(&preg, "hello world", 11, 1, pmatch, 0) == JSTR_RE_RET_NOERROR);
		assert(pmatch[0].rm_so == 6);
		assert(pmatch[0].rm_eo == 11);

		pmatch[0].rm_so = -2;
		pmatch[0].rm_eo = -2;
		assert(jstr_re_exec_len(&preg, "hello world", 11, 1, pmatch, 0) == JSTR_RE_RET_NOERROR);
		assert(pmatch[0].rm_so == 6);
		assert(pmatch[0].rm_eo == 11);

		/* 3. Test jstr_re_search */
		pmatch[0].rm_so = -1;
		pmatch[0].rm_eo = -1;
		assert(jstr_re_search(&preg, "hello world", pmatch, 0) == JSTR_RE_RET_NOERROR);
		assert(pmatch[0].rm_so == 6);
		assert(pmatch[0].rm_eo == 11);

		pmatch[0].rm_so = -2;
		pmatch[0].rm_eo = -2;
		assert(jstr_re_search(&preg, "hello world", pmatch, 0) == JSTR_RE_RET_NOERROR);
		assert(pmatch[0].rm_so == 6);
		assert(pmatch[0].rm_eo == 11);

		jstr_re_free(&preg);
	}
	SUCCESS();
	(void)argc;
	(void)argv;
	return 0;
}
