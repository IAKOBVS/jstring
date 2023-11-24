/* Copyright (c) 2023 James Tirta Halim <tirtajames45 at gmail dot com>
   This file is part of the jstring library.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

   MIT License (Expat) */

#include "test.h"

#define T(func, string, expected)                                   \
	do {                                                        \
		char s[1024] = string;                              \
		JSTR_STATIC_ASSERT(sizeof(s) > 2 * strlen(string)          \
		            && sizeof(s) > 2 * strlen(expected),    \
		            "");                                    \
		const char *const p = func(s);                      \
		ASSERT(strlen(s) == strlen(expected), s, expected); \
		ASSERT(p - s == strlen(expected), s, expected);     \
		ASSERT(!strcmp(s, expected), s, expected);          \
	} while (0)

#define T_CPY(func, string, expected)                                   \
	do {                                                            \
		char dst[1024];                                         \
		JSTR_STATIC_ASSERT(sizeof(dst) > 2 * strlen(string)            \
		            && sizeof(dst) > 2 * strlen(expected),      \
		            "");                                        \
		const char *const p = func(dst, string);                \
		ASSERT(strlen(dst) == strlen(expected), dst, expected); \
		ASSERT(p - dst == strlen(expected), dst, expected);     \
		ASSERT(!strcmp(dst, expected), dst, expected);          \
		ASSERT(!strcmp(dst, expected), dst, expected);          \
	} while (0)

int
main(int argc, char **argv)
{
	TESTING(jstr_toCamelCaseP);
	T(jstr_toCamelCaseP, "", "");
	T(jstr_toCamelCaseP, "hello", "hello");
	T(jstr_toCamelCaseP, "hello_world", "helloWorld");
	T(jstr_toCamelCaseP, "_hello", "_hello");
	T(jstr_toCamelCaseP, "_hello_world", "_helloWorld");

	TESTING(jstr_tocamelCaseCpyP);
	T_CPY(jstr_toCamelCaseCpyP, "", "");
	T_CPY(jstr_toCamelCaseCpyP, "hello", "hello");
	T_CPY(jstr_toCamelCaseCpyP, "hello_world", "helloWorld");
	T_CPY(jstr_toCamelCaseCpyP, "__hello", "__hello");
	T_CPY(jstr_toCamelCaseCpyP, "__hello_world", "__helloWorld");

	TESTING(jstr_to_snake_case_p);
	T(jstr_to_snake_case_p, "", "");
	T(jstr_to_snake_case_p, "hello", "hello");
	T(jstr_to_snake_case_p, "helloWorld", "hello_world");
	T(jstr_to_snake_case_p, "__hello", "__hello");
	T(jstr_to_snake_case_p, "__helloWorld", "__hello_world");

	TESTING(jstr_to_snake_case_cpy_p);
	T_CPY(jstr_to_snake_case_cpy_p, "", "");
	T_CPY(jstr_to_snake_case_cpy_p, "hello", "hello");
	T_CPY(jstr_to_snake_case_cpy_p, "helloWorld", "hello_world");
	T_CPY(jstr_to_snake_case_cpy_p, "__hello", "__hello");
	T_CPY(jstr_to_snake_case_cpy_p, "__helloWorld", "__hello_world");

	SUCCESS();
	return 0;
}
