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

#include "test.h"
#include "../include/ctype.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define T(func)                       \
	for (int i = 0; i < 128; ++i) \
	ASSERT_ERRFUNC(#func, !jstr_##func(i) == !func(i))
#define T_STR(func, str, result)                                                                                             \
	do {                                                                                                                 \
		size_t expected_len, result_len;                                                                             \
		*(char *)memcpy(buffer_expected, str, strlen(str)) = '\0';                                                   \
		*(char *)memcpy(buffer_result, str, strlen(str)) = '\0';                                                     \
		(void)jstr_##func(buffer_result);                                                                            \
		(void)simple_##func(buffer_expected);                                                                        \
		expected_len = strlen(buffer_expected);                                                                      \
		result_len = strlen(buffer_result);                                                                          \
		ASSERT_RESULT(#func, !strcmp(buffer_result, buffer_expected), buffer_result, buffer_expected);               \
		ASSERT_RESULT(#func, result_len == expected_len, buffer_result, buffer_expected);                            \
		ASSERT_RESULT(#func, !memcmp(buffer_result, buffer_expected, result_len), buffer_expected, buffer_expected); \
	} while (0)
#define T_TO(str, result)                         \
	do {                                      \
		T_STR(tolowerstr_p, str, result); \
		T_STR(toupperstr_p, str, result); \
	} while (0)

static char buffer_result[256];
static char buffer_expected[256];

static char *
simple_toupperstr_p(char *s)
{
	for (; *s; ++s)
		*s = toupper(*s);
	return s;
}

static char *
simple_tolowerstr_p(char *s)
{
	for (; *s; ++s)
		*s = tolower(*s);
	return s;
}

int
main(int argc, char **argv)
{
	START();
	T(isalpha);
	T(islower);
	T(isupper);
	T(isspace);
	T(isblank);
	T(isdigit);
	T(isxdigit);
	T(ispunct);
	T(isgraph);
	T(isprint);
	T(isprint);
	T(iscntrl);
	T(tolower);
	T(toupper);
	T_TO("", "");
	T_TO(";", ";");
	T_STR(toupperstr_p, "a", "A");
	T_STR(toupperstr_p, "aA", "AA");
	T_STR(tolowerstr_p, "A", "a");
	T_STR(tolowerstr_p, "aA", "aa");
	SUCCESS();
	return EXIT_SUCCESS;
}
