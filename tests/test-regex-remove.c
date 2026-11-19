/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Fixes regex.h problems with tcc */
#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/regex.h"

/* Helper: assign a C string to result, trimming capacity to fit. */
/* Assert that result equals expected string. */

/* Test a removal function.
 * Calls func on the current result, asserts it returns expected_n,
 * then asserts result string matches expected. */

int
main(void)
{
	jstr_ty result = JSTR_INIT;
	{
		/* jstr_re_rm_exec: remove first match of digit sequence. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "abc123def456");
		test_expect_call("jstr_re_rm_exec", (long long)(jstr_re_rm_exec(&preg, jstr_struct(&result), 0)), (long long)(1), &result, "abcdef456");

		test_shrink_fill(&result, "no digits here");
		test_expect_call("jstr_re_rm_exec", (long long)(jstr_re_rm_exec(&preg, jstr_struct(&result), 0)), (long long)(0), &result, "no digits here");

		test_shrink_fill(&result, "123");
		test_expect_call("jstr_re_rm_exec", (long long)(jstr_re_rm_exec(&preg, jstr_struct(&result), 0)), (long long)(1), &result, "");

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rm_from_exec: remove first match from start_idx. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "abc123def456");
		test_expect_call("jstr_re_rm_from_exec", (long long)(jstr_re_rm_from_exec(&preg, jstr_struct(&result), 3, 0)), (long long)(1), &result, "abcdef456");

		/* start_idx past first match, finds the second. */
		test_shrink_fill(&result, "abc123def456");
		test_expect_call("jstr_re_rm_from_exec", (long long)(jstr_re_rm_from_exec(&preg, jstr_struct(&result), 7, 0)), (long long)(1), &result, "abc123def");

		/* start_idx past all matches. */
		test_shrink_fill(&result, "abc123def456");
		test_expect_call("jstr_re_rm_from_exec", (long long)(jstr_re_rm_from_exec(&preg, jstr_struct(&result), 15, 0)), (long long)(0), &result, "abc123def456");

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rmn_exec: remove up to n matches. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "1a2b3c4d");
		test_expect_call("jstr_re_rmn_exec", (long long)(jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, 2)), (long long)(2), &result, "ab3c4d");

		test_shrink_fill(&result, "1a2b3c4d");
		test_expect_call("jstr_re_rmn_exec", (long long)(jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, 0)), (long long)(0), &result, "1a2b3c4d");

		test_shrink_fill(&result, "1a2b3c4d");
		test_expect_call("jstr_re_rmn_exec", (long long)(jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, (size_t)-1)), (long long)(4), &result, "abcd");

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rmn_from_exec: remove up to n matches from start_idx. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "1a2b3c4d");
		test_expect_call("jstr_re_rmn_from_exec", (long long)(jstr_re_rmn_from_exec(&preg, jstr_struct(&result), 2, 0, 2)), (long long)(2), &result, "1abc4d");

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rmall_exec: remove all matches. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "1a2b3c4d");
		test_expect_call("jstr_re_rmall_exec", (long long)(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0)), (long long)(4), &result, "abcd");

		test_shrink_fill(&result, "no_digits");
		test_expect_call("jstr_re_rmall_exec", (long long)(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0)), (long long)(0), &result, "no_digits");

		test_shrink_fill(&result, "123");
		test_expect_call("jstr_re_rmall_exec", (long long)(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0)), (long long)(1), &result, "");

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rmall_from_exec: remove all matches from start_idx. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "1a2b3c4d");
		test_expect_call("jstr_re_rmall_from_exec", (long long)(jstr_re_rmall_from_exec(&preg, jstr_struct(&result), 2, 0)), (long long)(3), &result, "1abcd");

		test_shrink_fill(&result, "1a2b3c4d");
		test_expect_call("jstr_re_rmall_from_exec", (long long)(jstr_re_rmall_from_exec(&preg, jstr_struct(&result), 0, 0)), (long long)(4), &result, "abcd");

		jstr_re_free(&preg);
	}
	{
		/* Remove letters, keep digits. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-zA-Z]\\{1,\\}", 0)));

		test_shrink_fill(&result, "abc123def456ghi");
		test_expect_call("jstr_re_rmall_exec", (long long)(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0)), (long long)(3), &result, "123456");

		jstr_re_free(&preg);
	}
	{
		/* Remove underscores and keep rest. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "_\\{1,\\}", 0)));

		test_shrink_fill(&result, "a_b_c_d");
		test_expect_call("jstr_re_rmall_exec", (long long)(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0)), (long long)(3), &result, "abcd");

		jstr_re_free(&preg);
	}
	{
		/* Anchored pattern with jstr_re_rm_exec (single remove). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "^hello", 0)));

		test_shrink_fill(&result, "hello world hello");
		test_expect_call("jstr_re_rm_exec", (long long)(jstr_re_rm_exec(&preg, jstr_struct(&result), 0)), (long long)(1), &result, " world hello");

		test_shrink_fill(&result, "say hello");
		test_expect_call("jstr_re_rm_exec", (long long)(jstr_re_rm_exec(&preg, jstr_struct(&result), 0)), (long long)(0), &result, "say hello");

		jstr_re_free(&preg);
	}
	{
		/* Anchored at end with jstr_re_rm_exec. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "world$", 0)));

		test_shrink_fill(&result, "hello world");
		test_expect_call("jstr_re_rm_exec", (long long)(jstr_re_rm_exec(&preg, jstr_struct(&result), 0)), (long long)(1), &result, "hello ");

		jstr_re_free(&preg);
	}
	{
		/* Remove with NOTBOL via eflags. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "^hello", 0)));

		test_shrink_fill(&result, "hello world");
		test_expect_call("jstr_re_rm_exec", (long long)(jstr_re_rm_exec(&preg, jstr_struct(&result), JSTR_RE_EF_NOTBOL)), (long long)(0), &result, "hello world");

		jstr_re_free(&preg);
	}
	{
		/* Newline-sensitive remove. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "^[a-z]\\{1,\\}", JSTR_RE_CF_NEWLINE)));

		test_shrink_fill(&result, "\nhello_world");
		test_expect_call("jstr_re_rm_exec", (long long)(jstr_re_rm_exec(&preg, jstr_struct(&result), 0)), (long long)(1), &result, "\n_world");

		jstr_re_free(&preg);
	}
	{
		/* Remove all alphabetic runs. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]\\{1,\\}", 0)));

		test_shrink_fill(&result, "hello world foo bar");
		test_expect_call("jstr_re_rmall_exec", (long long)(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0)), (long long)(4), &result, "   ");

		jstr_re_free(&preg);
	}
	{
		/* _from_exec with start_idx == sz (boundary, no match possible). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "abc");
		test_expect_call("jstr_re_rm_from_exec", (long long)(jstr_re_rm_from_exec(&preg, jstr_struct(&result), 3, 0)), (long long)(0), &result, "abc");

		test_shrink_fill(&result, "");
		test_expect_call("jstr_re_rm_from_exec", (long long)(jstr_re_rm_from_exec(&preg, jstr_struct(&result), 0, 0)), (long long)(0), &result, "");

		jstr_re_free(&preg);
	}
	{
		/* _from_exec on empty string. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]\\{1,\\}", 0)));

		test_shrink_fill(&result, "");
		test_expect_call("jstr_re_rm_exec", (long long)(jstr_re_rm_exec(&preg, jstr_struct(&result), 0)), (long long)(0), &result, "");
		test_expect_call("jstr_re_rmall_exec", (long long)(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0)), (long long)(0), &result, "");
		test_expect_call("jstr_re_rmn_exec", (long long)(jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, (size_t)-1)), (long long)(0), &result, "");

		jstr_re_free(&preg);
	}
	{
		/* rmn_from_exec with n=1 (falls through to rm_from_exec). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "a1b2c3");
		test_expect_call("jstr_re_rmn_from_exec", (long long)(jstr_re_rmn_from_exec(&preg, jstr_struct(&result), 0, 0, 1)), (long long)(1), &result, "ab2c3");

		jstr_re_free(&preg);
	}
	{
		/* rmn_from_exec with start_idx > sz. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "a1b2c3");
		test_expect_call("jstr_re_rmn_from_exec", (long long)(jstr_re_rmn_from_exec(&preg, jstr_struct(&result), 99, 0, (size_t)-1)), (long long)(0), &result, "a1b2c3");

		jstr_re_free(&preg);
	}
	{
		/* rmall_from_exec with start_idx past end. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "a1b2c3");
		test_expect_call("jstr_re_rmall_from_exec", (long long)(jstr_re_rmall_from_exec(&preg, jstr_struct(&result), 20, 0)), (long long)(0), &result, "a1b2c3");

		jstr_re_free(&preg);
	}
	{
		/* Remove first char only (pattern matches single char). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, ".", 0)));

		test_shrink_fill(&result, "abc");
		test_expect_call("jstr_re_rm_exec", (long long)(jstr_re_rm_exec(&preg, jstr_struct(&result), 0)), (long long)(1), &result, "bc");

		test_shrink_fill(&result, "a");
		test_expect_call("jstr_re_rm_exec", (long long)(jstr_re_rm_exec(&preg, jstr_struct(&result), 0)), (long long)(1), &result, "");

		jstr_re_free(&preg);
	}
	{
		/* Remove all individual characters (dot pattern). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, ".", 0)));

		test_shrink_fill(&result, "abc");
		test_expect_call("jstr_re_rmall_exec", (long long)(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0)), (long long)(3), &result, "");

		jstr_re_free(&preg);
	}
	{
		/* NOTBOL with remove from mid-string. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "^hello", 0)));

		test_shrink_fill(&result, "hello hello");
		test_expect_call("jstr_re_rm_from_exec", (long long)(jstr_re_rm_from_exec(&preg, jstr_struct(&result), 1, JSTR_RE_EF_NOTBOL)), (long long)(0), &result, "hello hello");

		/* Without NOTBOL, skips bol-only match. */
		test_expect_call("jstr_re_rm_from_exec", (long long)(jstr_re_rm_from_exec(&preg, jstr_struct(&result), 6, JSTR_RE_EF_NOTBOL)), (long long)(0), &result, "hello hello");

		jstr_re_free(&preg);
	}
	{
		/* Double free is safe (already tested). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "abc", 0)));
		jstr_re_free(&preg);
	}
	jstr_free_j(&result);
	return 0;
}
