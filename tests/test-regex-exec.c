/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 * This file is part of the jstring library.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

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
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "hello", 0)));

		/* jstr_re_exec: exact match. */
		assert(jstr_re_exec(&preg, "hello", 0, NULL, 0) == JSTR_RE_RET_NOERROR);
		assert(jstr_re_exec(&preg, "hello!", 0, NULL, 0) == JSTR_RE_RET_NOERROR);
		assert(jstr_re_exec(&preg, "world", 0, NULL, 0) == JSTR_RE_RET_NOMATCH);

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "^hello", 0)));

		/* Anchored at start. */
		assert(jstr_re_exec(&preg, "hello", 0, NULL, 0) == JSTR_RE_RET_NOERROR);
		assert(jstr_re_exec(&preg, "hello world", 0, NULL, 0) == JSTR_RE_RET_NOERROR);
		assert(jstr_re_exec(&preg, "say hello", 0, NULL, 0) == JSTR_RE_RET_NOMATCH);
		/* NOTBOL prevents ^ from matching at start. */
		assert(jstr_re_exec(&preg, "hello", 0, NULL, JSTR_RE_EF_NOTBOL) == JSTR_RE_RET_NOMATCH);

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "world$", 0)));

		/* Anchored at end. */
		assert(jstr_re_exec(&preg, "world", 0, NULL, 0) == JSTR_RE_RET_NOERROR);
		assert(jstr_re_exec(&preg, "hello world", 0, NULL, 0) == JSTR_RE_RET_NOERROR);
		assert(jstr_re_exec(&preg, "world! hello", 0, NULL, 0) == JSTR_RE_RET_NOMATCH);
		/* NOTEOL prevents $ from matching at end. */
		assert(jstr_re_exec(&preg, "world", 0, NULL, JSTR_RE_EF_NOTEOL) == JSTR_RE_RET_NOMATCH);

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "hello", JSTR_RE_CF_ICASE)));

		/* Case insensitive. */
		assert(jstr_re_exec(&preg, "HELLO", 0, NULL, 0) == JSTR_RE_RET_NOERROR);
		assert(jstr_re_exec(&preg, "Hello", 0, NULL, 0) == JSTR_RE_RET_NOERROR);
		assert(jstr_re_exec(&preg, "hElLo", 0, NULL, 0) == JSTR_RE_RET_NOERROR);
		assert(jstr_re_exec(&preg, "world", 0, NULL, 0) == JSTR_RE_RET_NOMATCH);

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		/* jstr_re_exec with nmatch and pmatch. */
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\) \\([a-z]\\{1,\\}\\)", 0)));

		regmatch_t pmatch[3];
		assert(jstr_re_exec(&preg, "hello world", 3, pmatch, 0) == JSTR_RE_RET_NOERROR);
		assert(pmatch[0].rm_so == 0);
		assert(pmatch[0].rm_eo == 11);
		assert(pmatch[1].rm_so == 0);
		assert(pmatch[1].rm_eo == 5);
		assert(pmatch[2].rm_so == 6);
		assert(pmatch[2].rm_eo == 11);

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		/* jstr_re_exec_len with explicit length. */
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "hello", 0)));

		regmatch_t rm;
		assert(jstr_re_exec_len(&preg, "helloXXX", 5, 1, &rm, 0) == JSTR_RE_RET_NOERROR);
		assert(rm.rm_so == 0);
		assert(rm.rm_eo == 5);
		/* Matching beyond length should fail. */
		assert(jstr_re_exec_len(&preg, "HELLO", 5, 1, &rm, 0) == JSTR_RE_RET_NOMATCH);

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		/* jstr_re_exec_len with a shorter length should not match
		 * beyond it. */
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "hello", 0)));

		regmatch_t rm;
		assert(jstr_re_exec_len(&preg, "hello", 4, 1, &rm, 0) == JSTR_RE_RET_NOMATCH);

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		/* jstr_re_match returns NOERROR if pattern matches, NOMATCH
		 * otherwise (no submatches). */
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		assert(jstr_re_match(&preg, "123", 0) == JSTR_RE_RET_NOERROR);
		assert(jstr_re_match(&preg, "abc123", 0) == JSTR_RE_RET_NOERROR);
		assert(jstr_re_match(&preg, "abc", 0) == JSTR_RE_RET_NOMATCH);
		assert(jstr_re_match(&preg, "", 0) == JSTR_RE_RET_NOMATCH);

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		/* jstr_re_match_len with explicit length. */
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "abc", 0)));

		assert(jstr_re_match_len(&preg, "abc", 3, 0) == JSTR_RE_RET_NOERROR);
		assert(jstr_re_match_len(&preg, "abcXXX", 3, 0) == JSTR_RE_RET_NOERROR);
		assert(jstr_re_match_len(&preg, "ABC", 3, 0) == JSTR_RE_RET_NOMATCH);
		/* Shorter than pattern should not match. */
		assert(jstr_re_match_len(&preg, "ab", 2, 0) == JSTR_RE_RET_NOMATCH);

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		/* jstr_re_search returns match position. */
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "world", 0)));

		regmatch_t rm;
		assert(jstr_re_search(&preg, "hello world", &rm, 0) == JSTR_RE_RET_NOERROR);
		assert(rm.rm_so == 6);
		assert(rm.rm_eo == 11);

		/* No match. */
		assert(jstr_re_search(&preg, "hello there", &rm, 0) == JSTR_RE_RET_NOMATCH);

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		/* jstr_re_search with case insensitive flag. */
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "hello", JSTR_RE_CF_ICASE)));

		regmatch_t rm;
		assert(jstr_re_search(&preg, "say HELLO world", &rm, 0) == JSTR_RE_RET_NOERROR);
		assert(rm.rm_so == 4);
		assert(rm.rm_eo == 9);

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		/* jstr_re_search_len with explicit length. */
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "world", 0)));

		regmatch_t rm;
		assert(jstr_re_search_len(&preg, "hello world and more", 11, &rm, 0) == JSTR_RE_RET_NOERROR);
		assert(rm.rm_so == 6);
		assert(rm.rm_eo == 11);
		/* Searching beyond length should not find it. */
		assert(jstr_re_search_len(&preg, "hello world and more", 10, &rm, 0) == JSTR_RE_RET_NOMATCH);

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		/* Extended regex with jstr_re_exec. */
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]+[0-9]+", JSTR_RE_CF_EXTENDED)));

		regmatch_t rm;
		assert(jstr_re_exec(&preg, "abc123", 1, &rm, 0) == JSTR_RE_RET_NOERROR);
		assert(rm.rm_so == 0);
		assert(rm.rm_eo == 6);
		assert(jstr_re_exec(&preg, "abc", 1, &rm, 0) == JSTR_RE_RET_NOMATCH);

		jstr_re_free(&preg);
	}
	SUCCESS();
	return 0;
	(void)argc;
}
