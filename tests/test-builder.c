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

#define JSTR_DEBUG 1
#define JSTR_PANIC 1

#include "test.h"
#include "../src/jstr-regex.h"

#define T_APPEND(ret, fn, ...)                                                                         \
	do {                                                                                           \
		TESTING(fn);                                                                           \
		if (ret != fn(__VA_ARGS__)) {                                                          \
			fprintf(stderr, "%s\n", JSTR_STRING(fn) "()");                                 \
			jstr_debug(&j);                                                                \
			jstr_errdie("");                                                               \
		}                                                                                      \
		ASSERT(strcmp(j.data, expected) == 0, j.data, expected);                               \
		ASSERT(j.size == strlen(expected), j.data, expected);                                  \
		ASSERT(memcmp(j.data, expected, j.size) == 0, j.data, expected);                       \
		ASSERT(strlen(expected) != 0 && j.size != 0 && j.capacity > j.size, j.data, expected); \
		j.size = 0;                                                                            \
		*j.data = '\0';                                                                        \
	} while (0)

#define FILL(j, str) assert(JSTR_RET_SUCC == jstr_assign_len(JSTR_STRUCT(&(j)), str, strlen(str)))

int
main(int argc, char **argv)
{
	jstr_ty j = JSTR_INIT;
	START();
	const char *expected;
	const char *find;
	const char *rplc;
	regex_t preg;
	/* jstr-builder tests. */
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_cat, JSTR_STRUCT(&j), "hello", " ", "world", NULL);
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_append_len, JSTR_STRUCT(&j), expected, strlen(expected));
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_prepend_len, JSTR_STRUCT(&j), expected, strlen(expected));
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_assign_len, JSTR_STRUCT(&j), expected, strlen(expected));
	/* jstr-replace tests. */
	FILL(j, "hello hello hello hello");
	find = "hello";
	rplc = "world";
	expected = "world hello hello hello";
	T_APPEND(JSTR_RET_SUCC, jstr_rplc_len, JSTR_STRUCT(&j), find, strlen(find), rplc, strlen(rplc));
	FILL(j, "hello hello hello hello");
	find = "hello";
	rplc = "world";
	expected = "world world world world";
	T_APPEND(JSTR_RET_SUCC, jstr_rplcall_len, JSTR_STRUCT(&j), find, strlen(find), rplc, strlen(rplc));
	FILL(j, "hello hello hello hello");
	find = "hello";
	rplc = "";
	expected = "   ";
	T_APPEND(JSTR_RET_SUCC, jstr_rplcall_len, JSTR_STRUCT(&j), find, strlen(find), rplc, strlen(rplc));
	FILL(j, "hello hello hello hello");
	find = "hello";
	rplc = "";
	expected = "   ";
	T_APPEND(JSTR_RET_SUCC, jstr_rplcall_len, JSTR_STRUCT(&j), find, strlen(find), rplc, strlen(rplc));
	FILL(j, "hello hello hello hello");
	find = "[0-9A-Za-z]*";
	rplc = "";
	expected = "   ";
	/* jstr-regex tests. */
	assert(!jstrre_chkcomp(jstrre_comp(&preg, find, 0)));
	T_APPEND(JSTRRE_RET_NOERROR, jstrre_rplcall_len, &preg, JSTR_STRUCT(&j), rplc, strlen(rplc), 0);
	jstrre_free(&preg);
	FILL(j, "hello hello hello hello");
	find = "\\([0-9A-Za-z]*\\)";
	rplc = "\\1\\1";
	expected = "hellohello hellohello hellohello hellohello";
	assert(!jstrre_chkcomp(jstrre_comp(&preg, find, 0)));
	T_APPEND(JSTRRE_RET_NOERROR, jstrre_rplcall_bref_len, &preg, JSTR_STRUCT(&j), rplc, strlen(rplc), 0, 3);
	jstrre_free(&preg);
	jstr_free_j(&j);
	SUCCESS();
	return 0;
}
