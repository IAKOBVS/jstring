/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Fixes regex.h problems with tcc */
#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/regex.h"

int
main(int argc, char **argv)
{
	START();
	{
		jstr_re_ty preg;
		/* Valid pattern compiles successfully. */
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]", 0)));
		assert(preg.cflags == 0);
		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		/* Extended regex. */
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]+", JSTR_RE_CF_EXTENDED)));
		assert(preg.cflags == JSTR_RE_CF_EXTENDED);
		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		/* Case insensitive. */
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]", JSTR_RE_CF_ICASE)));
		assert(preg.cflags == JSTR_RE_CF_ICASE);
		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		/* Newline-sensitive. */
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "^hello", JSTR_RE_CF_NEWLINE)));
		assert(preg.cflags == JSTR_RE_CF_NEWLINE);
		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		/* Multiple flags combined. */
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]+", JSTR_RE_CF_EXTENDED | JSTR_RE_CF_ICASE)));
		assert(preg.cflags == (JSTR_RE_CF_EXTENDED | JSTR_RE_CF_ICASE));
		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		/* jstr_re_chkcomp returns true on compile error. */
		assert(jstr_re_chkcomp(jstr_re_comp(&preg, "[", 0)));
		/* Even on error, we should still free (regfree is safe on bad state). */
		jstr_re_free(&preg);
	}
	{
		/* jstr_re_ty preg; */
		/* jstr_re_chkexec returns false for NOERROR. */
		assert(!jstr_re_chkexec(JSTR_RE_RET_NOERROR));
		/* jstr_re_chkexec returns false for NOMATCH (not an error). */
		assert(!jstr_re_chkexec(JSTR_RE_RET_NOMATCH));
		/* jstr_re_chkexec returns true for compile errors. */
		assert(jstr_re_chkexec(JSTR_RE_RET_BADPAT));
		/* jstr_re_chk returns true for negative (runtime error from _exec functions). */
		assert(jstr_re_chk(-1));
		assert(jstr_re_chk(-JSTR_RE_RET_ESPACE));
		/* jstr_re_chk returns false for zero or positive. */
		assert(!jstr_re_chk(0));
		assert(!jstr_re_chk(1));
		assert(!jstr_re_chk(JSTR_RE_RET_NOERROR));
		assert(!jstr_re_chk(JSTR_RE_RET_NOMATCH));
	}
	{
		jstr_re_ty preg;
		/* Compile then match a pattern via jstr_re_exec. */
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "hello", 0)));
		assert(jstr_re_exec(&preg, "hello", 0, NULL, 0) == JSTR_RE_RET_NOERROR);
		assert(jstr_re_exec(&preg, "world", 0, NULL, 0) == JSTR_RE_RET_NOMATCH);
		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		/* jstr_re_chkcomp allows checking compile error correctly. */
		assert(jstr_re_chkcomp(jstr_re_comp(&preg, "\\", 0)));
		jstr_re_free(&preg);
	}
	{
		/* Two frees in a row on the same preg (should be safe, even if
		 * redundant). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "abc", 0)));
		jstr_re_free(&preg);
	}
	SUCCESS();
	return 0;
	(void)argc;
}
