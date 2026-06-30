/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Fixes regex.h problems with tcc */
#define __STDC_NO_VLA__ 1

#include "test.h"

#include <stdio.h>
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
/* Like T_APPEND_NORET but checks return count too. */
#define T_RPLC(func, result, expected, expected_n)       \
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
	jstr_ty result = JSTR_INIT;
	{ 
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "abc123def");

		T_RPLC(jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "X", 1, 0), result, "abcXdef", 1);

		FILL(result, "abc");

		T_RPLC(jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "X", 1, 0), result, "abc", 0);

		jstr_re_free(&preg);
	}
	{ 
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "abc123def456");

		T_RPLC(jstr_re_rplc_len_from_exec(&preg, jstr_struct(&result), 6, "X", 1, 0), result, "abc123defX", 1);

		FILL(result, "abc123def456");

		T_RPLC(jstr_re_rplc_len_from_exec(&preg, jstr_struct(&result), 12, "X", 1, 0), result, "abc123def456", 0);

		jstr_re_free(&preg);
	}
	{ 
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "1a2b3c");

		T_RPLC(jstr_re_rplcn_len_exec(&preg, jstr_struct(&result), "X", 1, 0, 2), result, "XaXb3c", 2);

		FILL(result, "1a2b3c");

		T_RPLC(jstr_re_rplcn_len_exec(&preg, jstr_struct(&result), "X", 1, 0, 0), result, "1a2b3c", 0);

		FILL(result, "1a2b3c");

		T_RPLC(jstr_re_rplcn_len_exec(&preg, jstr_struct(&result), "X", 1, 0, (size_t)-1), result, "XaXbXc", 3);

		jstr_re_free(&preg);
	}
	{ 
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "1a2b3c4d");

		T_RPLC(jstr_re_rplcn_len_from_exec(&preg, jstr_struct(&result), 2, "X", 1, 0, 2), result, "1aXbXc4d", 2);

		jstr_re_free(&preg);
	}
	{ 
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "1a2b3c");

		T_RPLC(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "NUM", 3, 0), result, "NUMaNUMbNUMc", 3);

		FILL(result, "abc");

		T_RPLC(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "NUM", 3, 0), result, "abc", 0);

		jstr_re_free(&preg);
	}
	{ 
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "1a2b3c4d");

		T_RPLC(jstr_re_rplcall_len_from_exec(&preg, jstr_struct(&result), 2, "X", 1, 0), result, "1aXbXcXd", 3);

		FILL(result, "1a2b3c4d");

		T_RPLC(jstr_re_rplcall_len_from_exec(&preg, jstr_struct(&result), 0, "X", 1, 0), result, "XaXbXcXd", 4);

		jstr_re_free(&preg);
	}
	{ 
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]\\{1,\\}", 0)));

		FILL(result, "hello world");

		T_RPLC(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "word", 4, 0), result, "word word", 2);

		jstr_re_free(&preg);
	}
	{ 
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "_\\{1,\\}", 0)));

		FILL(result, "a_b_c");

		T_RPLC(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), " ", 1, 0), result, "a b c", 2);

		jstr_re_free(&preg);
	}
	{ 
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "abc123def");

		T_RPLC(jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "", 0, 0), result, "abcdef", 1);

		jstr_re_free(&preg);
	}
	{ 
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]\\{1,\\}", 0)));

		FILL(result, "a b c");

		T_RPLC(jstr_re_rplcn_len_exec(&preg, jstr_struct(&result), "long", 4, 0, 2), result, "long long c", 2);

		jstr_re_free(&preg);
	}
	{ 
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]\\{4,\\}", 0)));

		FILL(result, "hello_world_foo");

		T_RPLC(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "x", 1, 0), result, "x_x_foo", 2);

		jstr_re_free(&preg);
	}
	{ 
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]+", JSTR_RE_CF_EXTENDED)));

		FILL(result, "a1b2c3");

		T_RPLC(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "X", 1, 0), result, "X1X2X3", 3);

		jstr_re_free(&preg);
	}
	{ 
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "colou?r", JSTR_RE_CF_EXTENDED)));

		FILL(result, "color colour");

		T_RPLC(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "X", 1, 0), result, "X X", 2);

		jstr_re_free(&preg);
	}
	{ 
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "hello", JSTR_RE_CF_ICASE)));

		FILL(result, "Hello HELLO hello");

		T_RPLC(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "hi", 2, 0), result, "hi hi hi", 3);

		jstr_re_free(&preg);
	}
	{
		/* rplc_len < find_len: replacement shorter than match. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]\\{4,\\}", 0)));

		FILL(result, "xxxx");
		T_RPLC(jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "y", 1, 0), result, "y", 1);

		FILL(result, "aaaa_bbbb");
		T_RPLC(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "x", 1, 0), result, "x_x", 2);

		jstr_re_free(&preg);
	}
	{
		/* rplc_len > find_len: replacement longer than match. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "a1b");
		T_RPLC(jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "long", 4, 0), result, "alongb", 1);

		jstr_re_free(&preg);
	}
	{
		/* rplc_len == find_len: same size replacement. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "a1b2c");
		T_RPLC(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "X", 1, 0), result, "aXbXc", 2);

		jstr_re_free(&preg);
	}
	{
		/* _from_exec with start_idx == sz (boundary). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "abc");
		T_RPLC(jstr_re_rplc_len_from_exec(&preg, jstr_struct(&result), 3, "X", 1, 0), result, "abc", 0);

		jstr_re_free(&preg);
	}
	{
		/* _from_exec with start_idx > sz. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "abc");
		T_RPLC(jstr_re_rplc_len_from_exec(&preg, jstr_struct(&result), 99, "X", 1, 0), result, "abc", 0);

		jstr_re_free(&preg);
	}
	{
		/* rplcn_from_exec with start_idx > sz. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "1a2b3c");
		T_RPLC(jstr_re_rplcn_len_from_exec(&preg, jstr_struct(&result), 99, "X", 1, 0, (size_t)-1), result, "1a2b3c", 0);

		jstr_re_free(&preg);
	}
	{
		/* rplcall_from_exec with start_idx > sz. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "1a2b3c");
		T_RPLC(jstr_re_rplcall_len_from_exec(&preg, jstr_struct(&result), 99, "X", 1, 0), result, "1a2b3c", 0);

		jstr_re_free(&preg);
	}
	{
		/* Empty haystack. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "");
		T_RPLC(jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "X", 1, 0), result, "", 0);
		T_RPLC(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "X", 1, 0), result, "", 0);

		jstr_re_free(&preg);
	}
	{
		/* rplcn with n=0 (no replacement). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "1a2b3c");
		T_RPLC(jstr_re_rplcn_len_exec(&preg, jstr_struct(&result), "X", 1, 0, 0), result, "1a2b3c", 0);

		jstr_re_free(&preg);
	}
	{
		/* rplcn with n=1 (single replacement via rplcn path). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "1a2b3c");
		T_RPLC(jstr_re_rplcn_len_exec(&preg, jstr_struct(&result), "X", 1, 0, 1), result, "Xa2b3c", 1);

		jstr_re_free(&preg);
	}
	{
		/* rplcn with n > matches (should replace all). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		FILL(result, "1a2");
		T_RPLC(jstr_re_rplcn_len_exec(&preg, jstr_struct(&result), "X", 1, 0, 99), result, "XaX", 2);

		jstr_re_free(&preg);
	}
	{
		/* Multiple digits vs single digit replacement (rplc_len < find_len). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{2,\\}", 0)));

		FILL(result, "ab12cd34e");
		T_RPLC(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "X", 1, 0), result, "abXcdXe", 2);

		jstr_re_free(&preg);
	}
	{
		/* Single char match replaced with multiple chars (rplc_len > find_len). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]", 0)));

		FILL(result, "a1b2c");
		T_RPLC(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "long", 4, 0), result, "long1long2long", 3);

		jstr_re_free(&preg);
	}
	{
		/* Pattern with boundary assertions and NOTBOL. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "^hello", 0)));

		FILL(result, "hello hello");
		T_RPLC(jstr_re_rplc_len_from_exec(&preg, jstr_struct(&result), 6, "X", 1, JSTR_RE_EF_NOTBOL), result, "hello hello", 0);

		jstr_re_free(&preg);
	}
	jstr_free_j(&result);
	SUCCESS();
	return 0;
	(void)argc;
}
