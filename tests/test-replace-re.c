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
#include "../src/jstr-regex.h"

#define T_ASSERT(func, result, expected)                                                                           \
	do {                                                                                                       \
		ASSERT_RESULT(func, (result).size == strlen(expected), (result).data, expected);                   \
		ASSERT_RESULT(func, memcmp((result).data, expected, (result).size) == 0, (result).data, expected); \
	} while (0)
#define FILL(result, str)                                                                     \
	do {                                                                                  \
		jstr_shrink_to_fit(&result.data, &result.size, &result.capacity);             \
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&(result)), str, strlen(str)))); \
	} while (0)
#define T_APPEND_NORET(func, ...)                              \
	do {                                                   \
		TESTING(func);                                 \
		func(__VA_ARGS__);                             \
		T_ASSERT(func(__VA_ARGS__), result, expected); \
		(result).size = 0;                             \
		*(result).data = '\0';                         \
	} while (0)
#define T_RE(_string, _find, _rplc, _expected, _n)                                                                \
	do {                                                                                                      \
		const char *rplc, *find, *expected;                                                               \
		FILL(result, _string);                                                                            \
		find = _find;                                                                                     \
		rplc = _rplc;                                                                                     \
		expected = _expected;                                                                             \
		jstr_re_ty preg;                                                                                  \
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, _find, 0)));                                          \
		T_APPEND_NORET(jstr_re_rplcn_len_exec, &preg, jstr_struct(&result), _rplc, strlen(_rplc), 0, _n); \
		jstr_re_free(&preg);                                                                              \
		(void)find;                                                                                       \
		(void)rplc;                                                                                       \
	} while (0)

int
main(int argc, char **argv)
{
	START();
	jstr_ty result = JSTR_INIT;
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___", (size_t)-1);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___", 4);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "___hello", 3);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "__hello_hello", 2);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "_hello_hello_hello", 1);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "", "hello_hello_hello_hello", 0);
	/* rplc_len > find_len */
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_world?", (size_t)-1);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_world?", 4);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_world?_hello", 3);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_world?_hello_hello", 2);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "world?_hello_hello_hello", 1);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "world?", "hello_hello_hello_hello", 0);
	/* rplc_len < find_len */
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "worl", "worl_worl_worl_worl", (size_t)-1);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "worl", "worl_worl_worl_worl", 4);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "worl", "worl_worl_worl_hello", 3);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "worl", "worl_worl_hello_hello", 2);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "worl", "worl_hello_hello_hello", 1);
	T_RE("hello_hello_hello_hello", "[0-9A-Za-z]\\{1,\\}", "worl", "hello_hello_hello_hello", 0);
	/* .* */
	T_RE("hello_hello_hello_hello", ".*", "worl", "worl", (size_t)-1);
	T_RE("hello_hello_hello_hello", "(.*)", "worl", "hello_hello_hello_hello", (size_t)-1);
	T_RE("hello_hello_hello_hello", "\\(.*\\)", "worl", "worl", (size_t)-1);
	T_RE("hello_(hello)_hello_hello", "(.*)", "worl", "hello_worl_hello_hello", (size_t)-1);
	jstr_free_j(&result);
	SUCCESS();
	return 0;
	(void)argc;
}
