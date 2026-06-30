/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Fixes regex.h problems with tcc */
#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/regex.h"

/* Helper: assign a C string to result, trimming capacity to fit. */
#define FILL(result, str)                                                                     \
	do {                                                                                  \
		(void)jstr_shrink_to_fit(&result.data, &result.size, &result.capacity);             \
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&(result)), str, strlen(str)))); \
	} while (0)
/* Assert that result equals expected string. */
#define T_ASSERT(func, result, expected)                                                                           \
	do {                                                                                                       \
		ASSERT_RESULT(func, (result).size == strlen(expected), (result).data, expected);                   \
		ASSERT_RESULT(func, memcmp((result).data, expected, (result).size) == 0, (result).data, expected); \
	} while (0)

/* Test a removal function.
 * Calls func on the current result, asserts it returns expected_n,
 * then asserts result string matches expected. */
#define T_RM(func, result, expected, expected_n)           \
	do {                                               \
		TESTING(func);                             \
		jstr_re_off_ty ret_n_ = func;              \
		ASSERT_ERRFUNC(func, ret_n_ == expected_n); \
		T_ASSERT(func, result, expected);          \
	} while (0)

int
main(int argc, char **argv)
{
	START();
	jstr_ty result = JSTR_INIT;
	{
		/* jstr_re_rm_exec: remove first match of digit sequence. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "abc123def456");
		T_RM(jstr_re_rm_exec(&preg, jstr_struct(&result), 0), result, "abcdef456", 1);

		FILL(result, "no digits here");
		T_RM(jstr_re_rm_exec(&preg, jstr_struct(&result), 0), result, "no digits here", 0);

		FILL(result, "123");
		T_RM(jstr_re_rm_exec(&preg, jstr_struct(&result), 0), result, "", 1);

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rm_from_exec: remove first match from start_idx. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "abc123def456");
		T_RM(jstr_re_rm_from_exec(&preg, jstr_struct(&result), 3, 0), result, "abcdef456", 1);

		/* start_idx past first match, finds the second. */
		FILL(result, "abc123def456");
		T_RM(jstr_re_rm_from_exec(&preg, jstr_struct(&result), 7, 0), result, "abc123def", 1);

		/* start_idx past all matches. */
		FILL(result, "abc123def456");
		T_RM(jstr_re_rm_from_exec(&preg, jstr_struct(&result), 15, 0), result, "abc123def456", 0);

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rmn_exec: remove up to n matches. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "1a2b3c4d");
		T_RM(jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, 2), result, "ab3c4d", 2);

		FILL(result, "1a2b3c4d");
		T_RM(jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, 0), result, "1a2b3c4d", 0);

		FILL(result, "1a2b3c4d");
		T_RM(jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, (size_t)-1), result, "abcd", 4);

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rmn_from_exec: remove up to n matches from start_idx. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "1a2b3c4d");
		T_RM(jstr_re_rmn_from_exec(&preg, jstr_struct(&result), 2, 0, 2), result, "1abc4d", 2);

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rmall_exec: remove all matches. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "1a2b3c4d");
		T_RM(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0), result, "abcd", 4);

		FILL(result, "no_digits");
		T_RM(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0), result, "no_digits", 0);

		FILL(result, "123");
		T_RM(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0), result, "", 1);

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rmall_from_exec: remove all matches from start_idx. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "1a2b3c4d");
		T_RM(jstr_re_rmall_from_exec(&preg, jstr_struct(&result), 2, 0), result, "1abcd", 3);

		FILL(result, "1a2b3c4d");
		T_RM(jstr_re_rmall_from_exec(&preg, jstr_struct(&result), 0, 0), result, "abcd", 4);

		jstr_re_free(&preg);
	}
	{
		/* Remove letters, keep digits. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-zA-Z]\\{1,\\}", 0)));

		FILL(result, "abc123def456ghi");
		T_RM(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0), result, "123456", 3);

		jstr_re_free(&preg);
	}
	{
		/* Remove underscores and keep rest. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "_\\{1,\\}", 0)));

		FILL(result, "a_b_c_d");
		T_RM(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0), result, "abcd", 3);

		jstr_re_free(&preg);
	}
	{
		/* Anchored pattern with jstr_re_rm_exec (single remove). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "^hello", 0)));

		FILL(result, "hello world hello");
		T_RM(jstr_re_rm_exec(&preg, jstr_struct(&result), 0), result, " world hello", 1);

		FILL(result, "say hello");
		T_RM(jstr_re_rm_exec(&preg, jstr_struct(&result), 0), result, "say hello", 0);

		jstr_re_free(&preg);
	}
	{
		/* Anchored at end with jstr_re_rm_exec. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "world$", 0)));

		FILL(result, "hello world");
		T_RM(jstr_re_rm_exec(&preg, jstr_struct(&result), 0), result, "hello ", 1);

		jstr_re_free(&preg);
	}
	{
		/* Remove with NOTBOL via eflags. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "^hello", 0)));

		FILL(result, "hello world");
		T_RM(jstr_re_rm_exec(&preg, jstr_struct(&result), JSTR_RE_EF_NOTBOL), result, "hello world", 0);

		jstr_re_free(&preg);
	}
	{
		/* Newline-sensitive remove. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "^[a-z]\\{1,\\}", JSTR_RE_CF_NEWLINE)));

		FILL(result, "\nhello_world");
		T_RM(jstr_re_rm_exec(&preg, jstr_struct(&result), 0), result, "\n_world", 1);

		jstr_re_free(&preg);
	}
	{
		/* Remove all alphabetic runs. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]\\{1,\\}", 0)));

		FILL(result, "hello world foo bar");
		T_RM(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0), result, "   ", 4);

		jstr_re_free(&preg);
	}
	{
		/* _from_exec with start_idx == sz (boundary, no match possible). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "abc");
		T_RM(jstr_re_rm_from_exec(&preg, jstr_struct(&result), 3, 0), result, "abc", 0);

		FILL(result, "");
		T_RM(jstr_re_rm_from_exec(&preg, jstr_struct(&result), 0, 0), result, "", 0);

		jstr_re_free(&preg);
	}
	{
		/* _from_exec on empty string. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]\\{1,\\}", 0)));

		FILL(result, "");
		T_RM(jstr_re_rm_exec(&preg, jstr_struct(&result), 0), result, "", 0);
		T_RM(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0), result, "", 0);
		T_RM(jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, (size_t)-1), result, "", 0);

		jstr_re_free(&preg);
	}
	{
		/* rmn_from_exec with n=1 (falls through to rm_from_exec). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "a1b2c3");
		T_RM(jstr_re_rmn_from_exec(&preg, jstr_struct(&result), 0, 0, 1), result, "ab2c3", 1);

		jstr_re_free(&preg);
	}
	{
		/* rmn_from_exec with start_idx > sz. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "a1b2c3");
		T_RM(jstr_re_rmn_from_exec(&preg, jstr_struct(&result), 99, 0, (size_t)-1), result, "a1b2c3", 0);

		jstr_re_free(&preg);
	}
	{
		/* rmall_from_exec with start_idx past end. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "a1b2c3");
		T_RM(jstr_re_rmall_from_exec(&preg, jstr_struct(&result), 20, 0), result, "a1b2c3", 0);

		jstr_re_free(&preg);
	}
	{
		/* Remove first char only (pattern matches single char). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, ".", 0)));

		FILL(result, "abc");
		T_RM(jstr_re_rm_exec(&preg, jstr_struct(&result), 0), result, "bc", 1);

		FILL(result, "a");
		T_RM(jstr_re_rm_exec(&preg, jstr_struct(&result), 0), result, "", 1);

		jstr_re_free(&preg);
	}
	{
		/* Remove all individual characters (dot pattern). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, ".", 0)));

		FILL(result, "abc");
		T_RM(jstr_re_rmall_exec(&preg, jstr_struct(&result), 0), result, "", 3);

		jstr_re_free(&preg);
	}
	{
		/* NOTBOL with remove from mid-string. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "^hello", 0)));

		FILL(result, "hello hello");
		T_RM(jstr_re_rm_from_exec(&preg, jstr_struct(&result), 1, JSTR_RE_EF_NOTBOL), result, "hello hello", 0);

		/* Without NOTBOL, skips bol-only match. */
		T_RM(jstr_re_rm_from_exec(&preg, jstr_struct(&result), 6, JSTR_RE_EF_NOTBOL), result, "hello hello", 0);

		jstr_re_free(&preg);
	}
	{
		/* Double free is safe (already tested). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "abc", 0)));
		jstr_re_free(&preg);
	}
	jstr_free_j(&result);
	SUCCESS();
	return 0;
	(void)argc;
}
