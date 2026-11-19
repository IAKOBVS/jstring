/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Fixes regex.h problems with tcc */
#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/regex.h"

static void
check_re_backref(jstr_ty *result, const char *str, const char *pattern, const char *rplc, const char *expected, size_t nmatch, size_t n, size_t start_idx)
{
	jstr_re_ty preg;
	assert(!jstr_re_chkcomp(jstr_re_comp(&preg, pattern, 0)));
	test_fill(result, str);
	jstr_re_off_ty ignored_re = jstr_re_rplcn_backref_len_from_exec(&preg, jstr_struct(result), start_idx, rplc, strlen(rplc), 0, nmatch, n);
	(void)ignored_re;
	test_expect_buf("jstr_re_rplcn_backref_len_from_exec", result, expected);
	test_reset(result);
	jstr_re_free(&preg);
}

int
main(void)
{
	jstr_ty result = JSTR_INIT;
	check_re_backref(&result, "hello_hello_hello_hello", "^[0-9A-Za-z]\\{1,\\}", "", "_hello_hello_hello", 1, (size_t)-1, 0);

	check_re_backref(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___", 1, (size_t)-1, 0);
	check_re_backref(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___", 1, 4, 0);
	check_re_backref(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___hello", 1, 3, 0);
	check_re_backref(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "__hello_hello", 1, 2, 0);
	check_re_backref(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "_hello_hello_hello", 1, 1, 0);
	check_re_backref(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "hello_hello_hello_hello", 1, 0, 0);
	check_re_backref(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_world?", 1, (size_t)-1, 0);
	check_re_backref(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_world?", 1, 4, 0);
	check_re_backref(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_hello", 1, 3, 0);
	check_re_backref(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_hello_hello", 1, 2, 0);
	check_re_backref(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_hello_hello_hello", 1, 1, 0);
	check_re_backref(&result, "hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "hello_hello_hello_hello", 1, 0, 0);

	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hellohello_worldworld_hellohello_worldworld", 2, (size_t)-1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hellohello_worldworld_hellohello_worldworld", 2, 4, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hellohello_worldworld_hellohello_world", 2, 3, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hellohello_worldworld_hello_world", 2, 2, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hellohello_world_hello_world", 2, 1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1", "hello_world_hello_world", 2, 0, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2", "helloworld_helloworld", 3, (size_t)-1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2", "helloworld_helloworld", 3, 2, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2", "helloworld_hello_world", 3, 1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2", "hello_world_hello_world", 3, 0, 0);

	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\1", "Phellohello_Pworldworld_Phellohello_Pworldworld", 2, (size_t)-1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\1", "Phellohello_Pworldworld_Phellohello_Pworldworld", 2, 4, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\1", "Phellohello_Pworldworld_Phellohello_world", 2, 3, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\1", "Phellohello_Pworldworld_hello_world", 2, 2, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\1", "Phellohello_world_hello_world", 2, 1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\1", "hello_world_hello_world", 2, 0, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\2", "Phelloworld_Phelloworld", 3, (size_t)-1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\2", "Phelloworld_Phelloworld", 3, 2, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\2", "Phelloworld_hello_world", 3, 1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "P\\1\\2", "hello_world_hello_world", 3, 0, 0);

	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1Q", "hellohelloQ_worldworldQ_hellohelloQ_worldworldQ", 2, (size_t)-1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1Q", "hellohelloQ_worldworldQ_hellohelloQ_worldworldQ", 2, 4, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1Q", "hellohelloQ_worldworldQ_hellohelloQ_world", 2, 3, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1Q", "hellohelloQ_worldworldQ_hello_world", 2, 2, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1Q", "hellohelloQ_world_hello_world", 2, 1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\1Q", "hello_world_hello_world", 2, 0, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2Q", "helloworldQ_helloworldQ", 3, (size_t)-1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2Q", "helloworldQ_helloworldQ", 3, 2, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2Q", "helloworldQ_hello_world", 3, 1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\2Q", "hello_world_hello_world", 3, 0, 0);

	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\1Q", "hello\\\\helloQ_world\\\\worldQ_hello\\\\helloQ_world\\\\worldQ", 2, (size_t)-1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\1Q", "hello\\\\helloQ_world\\\\worldQ_hello\\\\helloQ_world\\\\worldQ", 2, 4, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\1Q", "hello\\\\helloQ_world\\\\worldQ_hello\\\\helloQ_world", 2, 3, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\1Q", "hello\\\\helloQ_world\\\\worldQ_hello_world", 2, 2, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\1Q", "hello\\\\helloQ_world_hello_world", 2, 1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\1Q", "hello_world_hello_world", 2, 0, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\2Q", "hello\\\\worldQ_hello\\\\worldQ", 3, (size_t)-1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\2Q", "hello\\\\worldQ_hello\\\\worldQ", 3, 2, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\2Q", "hello\\\\worldQ_hello_world", 3, 1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\([0-9A-Za-z]\\{1,\\}\\)_\\([0-9A-Za-z]\\{1,\\}\\)", "\\1\\\\\\2Q", "hello_world_hello_world", 3, 0, 0);

	check_re_backref(&result, "hello_world_hello_world", "\\(.*\\)", "", "", 1, (size_t)-1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\(.*\\)", "p", "p", 1, (size_t)-1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\(.*\\)", "\\1", "hello_world_hello_world", 2, (size_t)-1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\(.*\\)", "\\1\\1", "hello_world_hello_worldhello_world_hello_world", 2, (size_t)-1, 0);
	check_re_backref(&result, "hello_world_hello_world", "\\(.*\\)", "\\1\\1", "hello_world_hello_worldhello_world_hello_world", 2, (size_t)-1, 0);

	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)", "worl", "hello_worl_hello_hello", 1, (size_t)-1, 0);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)", "worl", "hello_worl_hello_hello", 1, (size_t)-1, 1);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)", "worl", "hello_worl_hello_hello", 1, (size_t)-1, 2);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)", "worl", "hello_worl_hello_hello", 1, (size_t)-1, 3);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)", "worl", "hello_worl_hello_hello", 1, (size_t)-1, 4);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)", "worl", "hello_worl_hello_hello", 1, (size_t)-1, 5);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)", "worl", "hello_worl_hello_hello", 1, (size_t)-1, 6);

	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)", "worl", "hello_(hello)_hello_hello", 1, (size_t)-1, 7);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)", "worl", "hello_(hello)_hello_hello", 1, (size_t)-1, 8);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)", "worl", "hello_(hello)_hello_hello", 1, (size_t)-1, 9);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)", "worl", "hello_(hello)_hello_hello", 1, (size_t)-1, 10);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)", "worl", "hello_(hello)_hello_hello", 1, (size_t)-1, 11);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)", "worl", "hello_(hello)_hello_hello", 1, (size_t)-1, 12);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)", "worl", "hello_(hello)_hello_hello", 1, (size_t)-1, 13);

	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)\\(.\\)", "worl\\1", "hello_worl_hello_hello", 2, (size_t)-1, 0);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)\\(.\\)", "worl\\1", "hello_worl_hello_hello", 2, (size_t)-1, 1);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)\\(.\\)", "worl\\1", "hello_worl_hello_hello", 2, (size_t)-1, 2);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)\\(.\\)", "worl\\1", "hello_worl_hello_hello", 2, (size_t)-1, 3);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)\\(.\\)", "worl\\1", "hello_worl_hello_hello", 2, (size_t)-1, 4);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)\\(.\\)", "worl\\1", "hello_worl_hello_hello", 2, (size_t)-1, 5);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)\\(.\\)", "worl\\1", "hello_worl_hello_hello", 2, (size_t)-1, 6);

	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)\\(.\\)", "worl\\1", "hello_(hello)_hello_hello", 2, (size_t)-1, 7);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)\\(.\\)", "worl\\1", "hello_(hello)_hello_hello", 2, (size_t)-1, 8);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)\\(.\\)", "worl\\1", "hello_(hello)_hello_hello", 2, (size_t)-1, 9);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)\\(.\\)", "worl\\1", "hello_(hello)_hello_hello", 2, (size_t)-1, 10);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)\\(.\\)", "worl\\1", "hello_(hello)_hello_hello", 2, (size_t)-1, 11);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)\\(.\\)", "worl\\1", "hello_(hello)_hello_hello", 2, (size_t)-1, 12);
	check_re_backref(&result, "hello_(hello)_hello_hello", "(.*)\\(.\\)", "worl\\1", "hello_(hello)_hello_hello", 2, (size_t)-1, 13);

	jstr_free_j(&result);
	return 0;
}
