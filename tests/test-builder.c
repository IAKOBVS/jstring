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
#include "../src/jstr-regex.h"
#include "test-array.h"

#define T_APPEND(ret, func, ...)                                                                    \
	do {                                                                                        \
		TESTING(func);                                                                      \
		if (ret != func(__VA_ARGS__)) {                                                     \
			jstr_debug(&(j));                                                           \
			jstr_errdie("");                                                            \
		}                                                                                   \
		ASSERT_RESULT(func, strcmp((j).data, expected) == 0, (j).data, expected);           \
		ASSERT_RESULT(func, (j).size == strlen(expected), (j).data, expected);              \
		ASSERT_RESULT(func, memcmp((j).data, expected, (j).size) == 0, (j).data, expected); \
		ASSERT_RESULT(func, (j).capacity > (j).size, (j).data, expected);                   \
		(j).size = 0;                                                                       \
		*(j).data = '\0';                                                                   \
	} while (0)

#define FILL(j, str) assert(JSTR_RET_SUCC == jstr_assign_len(JSTR_STRUCT(&(j)), str, strlen(str)))

#define T_RPLC_INIT(buf, find, find_len)                                               \
	do {                                                                           \
		jstr_empty(buf.data, &buf.size);                                       \
		assert(!jstr_chk(jstr_assign_len(JSTR_STRUCT(&buf), find, find_len))); \
		assert(buf.size == find_len);                                          \
		assert(!memcmp(buf.data, find, find_len));                             \
	} while (0)

#define ASSERT_RESULT_CHR(func, j_result, j_expected, find, rplc, expr)    \
	do {                                                               \
		if (jstr_unlikely(!(expr))) {                              \
			PRINTERR("assertion failure: %s().\n", #func);     \
			PRINTERR("string:%s.\nreplace:%c.\n", find, rplc); \
			PRINTERR("result:\n");                             \
			jstr_debug(&(j_result));                           \
			PRINTERR("expected:\n");                           \
			jstr_debug(&(j_expected));                         \
			assert(expr);                                      \
		}                                                          \
	} while (0)

#define T_CHR(func, simple_func)                                                                                                                        \
	do {                                                                                                                                            \
		jstr_ty result = JSTR_INIT;                                                                                                             \
		jstr_ty expected = JSTR_INIT;                                                                                                           \
		T_FOREACHI(test_array_memcmp, i)                                                                                                        \
		{                                                                                                                                       \
			const char *find = test_array_memcmp[i].s1;                                                                                     \
			const char rplc = *test_array_memcmp[i].s2;                                                                                     \
			size_t find_len = strlen(find);                                                                                                 \
			T_RPLC_INIT(result, find, find_len);                                                                                            \
			T_RPLC_INIT(expected, find, find_len);                                                                                          \
			result.size = func(result.data, result.size, rplc, i) - result.data;                                                            \
			expected.size = simple_func(expected.data, expected.size, rplc, i) - expected.data;                                             \
			fprintf(stderr, "i:%zu\n", i);                                                                                                          \
			ASSERT_RESULT_CHR(func, result, expected, find, rplc, strlen(result.data) == strlen(expected.data));                            \
			ASSERT_RESULT_CHR(func, result, expected, find, rplc, result.size == expected.size);                                            \
			ASSERT_RESULT_CHR(func, result, expected, find, rplc, !memcmp(result.data, expected.data, result.size * sizeof(*result.data))); \
		}                                                                                                                                       \
		jstr_free_j(&result);                                                                                                                   \
		jstr_free_j(&expected);                                                                                                                 \
	} while (0)

static char *
simple_rplcnchr_p(char *s,
                  size_t sz,
                  char remove,
                  size_t n)
{
	if (n == 0)
		return s + sz;
	for (; *s; ++s)
		if (*s == remove) {
			if (n-- == 0)
				break;
			*s = remove;
		}
	return s + sz;
}

static char *
simple_rmnchr_p(char *s,
                size_t sz,
                char remove,
                size_t n)
{
	if (n == 0)
		return s + sz;
	char *dst = s;
	for (; n--;) {
		if (*s == remove) {
			++s;
		} else {
			*dst++ = *s++;
		}
	}
	*dst = '\0';
	return dst;
}

int
main(int argc, char **argv)
{
	jstr_ty j = JSTR_INIT;
	START();

	T_CHR(jstr_rmnchr_len_p, simple_rmnchr_p);

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
