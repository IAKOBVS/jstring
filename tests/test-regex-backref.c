/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Fixes regex.h problems with tcc */
#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/regex.h"

#define T_ASSERT(func, result, expected)                                                                           \
	do {                                                                                                       \
		ASSERT_RESULT(func, (result).size == strlen(expected), (result).data, expected);                   \
		ASSERT_RESULT(func, memcmp((result).data, expected, (result).size) == 0, (result).data, expected); \
	} while (0)
#define FILL(result, str)                                                                     \
	do {                                                                                  \
		(void)jstr_shrink_to_fit(&result.data, &result.size, &result.capacity);             \
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&(result)), str, strlen(str)))); \
	} while (0)
#define T_RPLC_BREF(func, result, expected, expected_n)  \
	do {                                             \
		TESTING(func);                           \
		jstr_re_off_ty ret_n_ = func;            \
		T_ASSERT(func, result, expected);          \
		ASSERT_ERRFUNC(func, ret_n_ == expected_n); \
		(result).size = 0;                           \
		*(result).data = '\0';                       \
	} while (0)

int
main(int argc, char **argv)
{
	START();
	jstr_ty result = JSTR_INIT;
	{
		/* jstr_re_rplcn_backref_len_exec: replace with backref \1. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "hello_world");
		T_RPLC_BREF(jstr_re_rplcn_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1", 4, 0, 2, (size_t)-1),
			result, "hellohello_worldworld", 2);

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rplcn_backref_len_exec: with explicit n limit. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "hello_world_foo");
		T_RPLC_BREF(jstr_re_rplcn_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1", 4, 0, 2, 2),
			result, "hellohello_worldworld_foo", 2);

		FILL(result, "hello_world_foo");
		T_RPLC_BREF(jstr_re_rplcn_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1", 4, 0, 2, 0),
			result, "hello_world_foo", 0);

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rplcn_backref_len_exec (from, with start_idx). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "hello_world_foo");
		T_RPLC_BREF(jstr_re_rplcn_backref_len_from_exec(&preg, jstr_struct(&result), 6, "\\1\\1", 4, 0, 2, (size_t)-1),
			result, "hello_worldworld_foofoo", 2);

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rplc_backref_len_exec: replace first match with backref. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "hello_world");
		T_RPLC_BREF(jstr_re_rplc_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1", 4, 0, 2),
			result, "hellohello_world", 1);

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rplc_backref_len_from_exec: replace first match from
		 * start_idx with backref. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "hello_world_foo");
		T_RPLC_BREF(jstr_re_rplc_backref_len_from_exec(&preg, jstr_struct(&result), 6, "\\1\\1", 4, 0, 2),
			result, "hello_worldworld_foo", 1);

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rplcall_backref_len_exec: replace all matches with backref. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "hello_world");
		T_RPLC_BREF(jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1", 4, 0, 2),
			result, "hellohello_worldworld", 2);

		jstr_re_free(&preg);
	}
	{
		/* jstr_re_rplcall_backref_len_from_exec: replace all matches from
		 * start_idx with backref. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "hello_world_foo");
		T_RPLC_BREF(jstr_re_rplcall_backref_len_from_exec(&preg, jstr_struct(&result), 6, "\\1\\1", 4, 0, 2),
			result, "hello_worldworld_foofoo", 2);

		FILL(result, "hello_world_foo");
		T_RPLC_BREF(jstr_re_rplcall_backref_len_from_exec(&preg, jstr_struct(&result), 0, "\\1\\1", 4, 0, 2),
			result, "hellohello_worldworld_foofoo", 3);

		jstr_re_free(&preg);
	}
	{
		/* Backref with multiple capture groups: \1 and \2. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)_\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "hello_world_foo_bar");
		T_RPLC_BREF(jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "\\1\\2", 4, 0, 3),
			result, "helloworld_foobar", 2);

		jstr_re_free(&preg);
	}
	{
		/* Backref with prefix text before \1. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "hello_world");
		T_RPLC_BREF(jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "P\\1\\1", 5, 0, 2),
			result, "Phellohello_Pworldworld", 2);

		jstr_re_free(&preg);
	}
	{
		/* Backref with suffix text after \1. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "hello_world");
		T_RPLC_BREF(jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1Q", 5, 0, 2),
			result, "hellohelloQ_worldworldQ", 2);

		jstr_re_free(&preg);
	}
	{
		/* Backref with escaped backslash before digit. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "hello_world");
		T_RPLC_BREF(jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "\\1\\\\\\1", 6, 0, 2),
			result, "hello\\\\hello_world\\\\world", 2);

		jstr_re_free(&preg);
	}
	{
		/* rplc_backref_len_from_exec with _from where start_idx is past
		 * some matches. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "a_b_c_d");
		T_RPLC_BREF(jstr_re_rplcall_backref_len_from_exec(&preg, jstr_struct(&result), 4, "\\1\\1", 4, 0, 2),
			result, "a_b_cc_dd", 2);

		jstr_re_free(&preg);
	}
	{
		/* Backref with .* pattern: capture entire string. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\(.*\\)", 0)));

		FILL(result, "hello_world");
		T_RPLC_BREF(jstr_re_rplc_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1", 4, 0, 2),
			result, "hello_worldhello_world", 1);

		jstr_re_free(&preg);
	}
	{
		/* Backref with newline-sensitive compilation flag: anchored
		 * patterns should only be used with rm/rplc (single match), not
		 * rmn/rplcn/rplcall. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "^\\([a-z]\\{1,\\}\\)", JSTR_RE_CF_NEWLINE)));

		FILL(result, "\nhello_world");
		T_RPLC_BREF(jstr_re_rplc_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1", 4, 0, 2),
			result, "\nhellohello_world", 1);

		jstr_re_free(&preg);
	}
	{
		/* Backref with n=1 via rplc_backref_len_exec (single, not via
		 * rplcn). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "hello_world_foo");
		T_RPLC_BREF(jstr_re_rplc_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1", 4, 0, 2),
			result, "hellohello_world_foo", 1);

		jstr_re_free(&preg);
	}
	{
		/* Backref with rplcn_backref_len_exec from start_idx > 0. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "a_b_c_d_e");
		T_RPLC_BREF(jstr_re_rplcn_backref_len_from_exec(&preg, jstr_struct(&result), 4, "\\1\\1", 4, 0, 2, 2),
			result, "a_b_cc_dd_e", 2);

		jstr_re_free(&preg);
	}
	{
		/* Backref replacement shorter than match (rplcwbackref_len < find_len). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{4,\\}\\)", 0)));

		FILL(result, "xxxx_yyyy");
		T_RPLC_BREF(jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "\\1", 2, 0, 2),
			result, "xxxx_yyyy", 2);

		jstr_re_free(&preg);
	}
	{
		/* Backref replacement longer than match. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\)", 0)));

		FILL(result, "a1b");
		T_RPLC_BREF(jstr_re_rplc_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1\\1", 6, 0, 2),
			result, "aaa1b", 1);

		jstr_re_free(&preg);
	}
	{
		/* Backref with empty capture group (pattern can match empty string). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]*\\)", 0)));

		FILL(result, "abc");
		T_RPLC_BREF(jstr_re_rplc_backref_len_exec(&preg, jstr_struct(&result), "[\\1]", 4, 0, 2),
			result, "[abc]", 1);

		jstr_re_free(&preg);
	}
	{
		/* Backref with rplcn_backref_len_exec n=0 (no replacement). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "a_b_c");
		T_RPLC_BREF(jstr_re_rplcn_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1", 4, 0, 2, 0),
			result, "a_b_c", 0);

		jstr_re_free(&preg);
	}
	{
		/* Backref with rplcn_backref_len_exec n=1. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "a_b_c");
		T_RPLC_BREF(jstr_re_rplcn_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1", 4, 0, 2, 1),
			result, "aa_b_c", 1);

		jstr_re_free(&preg);
	}
	{
		/* Backref with _from_exec start_idx == sz (boundary, no match). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "abc");
		T_RPLC_BREF(jstr_re_rplc_backref_len_from_exec(&preg, jstr_struct(&result), 3, "\\1\\1", 4, 0, 2),
			result, "abc", 0);

		jstr_re_free(&preg);
	}
	{
		/* Backref with _from_exec start_idx == sz (rplcn variant). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "abc");
		T_RPLC_BREF(jstr_re_rplcn_backref_len_from_exec(&preg, jstr_struct(&result), 3, "\\1\\1", 4, 0, 2, (size_t)-1),
			result, "abc", 0);

		jstr_re_free(&preg);
	}
	{
		/* Backref with _from_exec start_idx > sz. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "abc");
		T_RPLC_BREF(jstr_re_rplc_backref_len_from_exec(&preg, jstr_struct(&result), 99, "\\1\\1", 4, 0, 2),
			result, "abc", 0);

		jstr_re_free(&preg);
	}
	{
		/* Backref with empty haystack. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "");
		T_RPLC_BREF(jstr_re_rplc_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1", 4, 0, 2),
			result, "", 0);

		jstr_re_free(&preg);
	}
	{
		/* Backref rplcall_from_exec where start_idx skips first match. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "a_b_c");
		T_RPLC_BREF(jstr_re_rplcall_backref_len_from_exec(&preg, jstr_struct(&result), 2, "\\1\\1", 4, 0, 2),
			result, "a_bb_cc", 2);

		jstr_re_free(&preg);
	}
	{
		/* Backref rplcn_backref_len_exec with start_idx past end. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));

		FILL(result, "a_b_c");
		T_RPLC_BREF(jstr_re_rplcn_backref_len_from_exec(&preg, jstr_struct(&result), 99, "\\1\\1", 4, 0, 2, (size_t)-1),
			result, "a_b_c", 0);

		jstr_re_free(&preg);
	}
	jstr_free_j(&result);
	SUCCESS();
	return 0;
	(void)argc;
}
