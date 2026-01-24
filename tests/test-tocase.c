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
#include "../include/string.h"
#include "../include/replace.h"

#define T_ASSERT(func, expr, expected)                                               \
	do {                                                                         \
		ASSERT_RESULT(func, strlen(buf) == strlen(expected), buf, expected); \
		ASSERT_RESULT(func, p - buf == strlen(expected), buf, expected);     \
		ASSERT_RESULT(func, !strcmp(buf, expected), buf, expected);          \
	} while (0)

#define T(func, string, expected)                                                                               \
	do {                                                                                                    \
		JSTR_STATIC_ASSERT(sizeof(buf) > 2 * strlen(string) && sizeof(buf) > 2 * strlen(expected), ""); \
		memcpy(buf, string, strlen(string) + 1);                                                        \
		const char *p = func(buf);                                                                      \
		T_ASSERT(func(buf), expr, expected);                                                                 \
	} while (0)

#define T_CPY(func, string, expected)                                                                           \
	do {                                                                                                    \
		JSTR_STATIC_ASSERT(sizeof(buf) > 2 * strlen(string) && sizeof(buf) > 2 * strlen(expected), ""); \
		const char *p = func(buf, string);                                                              \
		T_ASSERT(func(buf, string), expr, expected);                                                                 \
	} while (0)

static char buf[256];

int
main(int argc, char **argv)
{
	START();

	TESTING(jstr_toCamelCaseP);
	T(jstr_toCamelCaseP, "", "");
	T(jstr_toCamelCaseP, "_", "_");
	T(jstr_toCamelCaseP, "hello", "hello");
	T(jstr_toCamelCaseP, "hello_world", "helloWorld");
	T(jstr_toCamelCaseP, "_hello", "_hello");
	T(jstr_toCamelCaseP, "_hello_world", "_helloWorld");

	TESTING(jstr_tocamelCaseCpyP);
	T_CPY(jstr_toCamelCaseCpyP, "", "");
	T_CPY(jstr_toCamelCaseCpyP, "_", "_");
	T_CPY(jstr_toCamelCaseCpyP, "hello", "hello");
	T_CPY(jstr_toCamelCaseCpyP, "hello_world", "helloWorld");
	T_CPY(jstr_toCamelCaseCpyP, "__hello", "__hello");
	T_CPY(jstr_toCamelCaseCpyP, "__hello_world", "__helloWorld");

	TESTING(jstr_to_snake_case_p);
	T(jstr_to_snake_case_p, "", "");
	T(jstr_to_snake_case_p, "_", "_");
	T(jstr_to_snake_case_p, "hello", "hello");
	T(jstr_to_snake_case_p, "helloWorld", "hello_world");
	T(jstr_to_snake_case_p, "__hello", "__hello");
	T(jstr_to_snake_case_p, "__helloWorld", "__hello_world");

	TESTING(jstr_to_snake_case_cpy_p);
	T_CPY(jstr_to_snake_case_cpy_p, "", "");
	T_CPY(jstr_to_snake_case_cpy_p, "_", "_");
	T_CPY(jstr_to_snake_case_cpy_p, "hello", "hello");
	T_CPY(jstr_to_snake_case_cpy_p, "helloWorld", "hello_world");
	T_CPY(jstr_to_snake_case_cpy_p, "__hello", "__hello");
	T_CPY(jstr_to_snake_case_cpy_p, "__helloWorld", "__hello_world");

	SUCCESS();
	return EXIT_SUCCESS;
}
