/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Fixes regex.h problems with tcc */
#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/regex.h"

static void
check_re_replace(jstr_ty *result, const char *str, const char *pattern, const char *rplc, size_t n, const char *expected)
{
	jstr_re_ty preg;
	assert(!jstr_re_chkcomp(jstr_re_comp(&preg, pattern, 0)));
	test_shrink_fill(result, str);
	jstr_re_off_ty ignored_re = jstr_re_rplcn_len_exec(&preg, jstr_struct(result), rplc, strlen(rplc), 0, n);
	(void)ignored_re;
	test_expect_buf("jstr_re_rplcn_len_exec", result, expected);
	test_reset(result);
	jstr_re_free(&preg);
}

int
main(void)
{
	jstr_ty result = JSTR_INIT;

	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", (size_t)-1, "___");
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", 4, "___");
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", 3, "___hello");
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", 2, "__hello_hello");
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", 1, "_hello_hello_hello");
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", 0, "hello_hello_hello_hello");
	/* rplc_len > find_len */
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", (size_t)-1, "world?_world?_world?_world?");
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", 4, "world?_world?_world?_world?");
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", 3, "world?_world?_world?_hello");
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", 2, "world?_world?_hello_hello");
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", 1, "world?_hello_hello_hello");
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", 0, "hello_hello_hello_hello");
	/* rplc_len < find_len */
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "worl", (size_t)-1, "worl_worl_worl_worl");
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "worl", 4, "worl_worl_worl_worl");
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "worl", 3, "worl_worl_worl_hello");
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "worl", 2, "worl_worl_hello_hello");
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "worl", 1, "worl_hello_hello_hello");
	check_re_replace(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "worl", 0, "hello_hello_hello_hello");
	/* .* */
	check_re_replace(&result, "hello_hello_hello_hello", ".*", "worl", (size_t)-1, "worl");
	check_re_replace(&result, "hello_hello_hello_hello", "(.*)", "worl", (size_t)-1, "hello_hello_hello_hello");
	check_re_replace(&result, "hello_hello_hello_hello", "\\(.*\\)", "worl", (size_t)-1, "worl");
	check_re_replace(&result, "hello_(hello)_hello_hello", "(.*)", "worl", (size_t)-1, "hello_worl_hello_hello");

	/* test REG_NEWLINE replace bug: .* b should result in b\nb rather than bb */
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, ".* b", REG_NEWLINE | REG_EXTENDED)));
		test_shrink_fill(&result, " b\n b");
		jstr_re_off_ty ignored_re = jstr_re_rplcn_len_exec(&preg, jstr_struct(&result), "b", 1, 0, (size_t)-1);
		(void)ignored_re;
		test_expect_buf("jstr_re_rplcn_len_exec", &result, "b\nb");
		test_reset(&result);
		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, ".* b", REG_NEWLINE | REG_EXTENDED)));
		test_shrink_fill(&result, " b\n b");
		jstr_re_off_ty ignored_re = jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, (size_t)-1);
		(void)ignored_re;
		test_expect_buf("jstr_re_rmn_exec", &result, "\n");
		test_reset(&result);
		jstr_re_free(&preg);
	}

	jstr_free_j(&result);
	return 0;
}
