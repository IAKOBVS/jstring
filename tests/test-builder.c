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

#define T_RPLC_INIT(buf, str, s_len)                                               \
	do {                                                                       \
		jstr_empty(buf.data, &buf.size);                                   \
		assert(!jstr_chk(jstr_assign_len(JSTR_STRUCT(&buf), str, s_len))); \
		assert(buf.size == s_len);                                         \
		assert(!memcmp(buf.data, s, s_len));                               \
	} while (0)

#define ASSERT_RESULT_CHR(func, j_result, j_expected, s, rplc, expr)   \
	do {                                                           \
		if (jstr_unlikely(!(expr))) {                          \
			PRINTERR("assertion failure: %s().\n", #func); \
			PRINTERR("string:%s\nreplace:%c\n", s, rplc);  \
			PRINTERR("result:\n");                         \
			jstr_debug(&(j_result));                       \
			PRINTERR("Expected:\n");                       \
			jstr_debug(&(j_expected));                     \
			assert(expr);                                  \
		}                                                      \
	} while (0)

#define T_CHR(func, simple_func)                                                                                                           \
	do {                                                                                                                               \
		jstr_ty buf = JSTR_INIT;                                                                                                   \
		jstr_ty s_buf = JSTR_INIT;                                                                                                 \
		T_FOREACHI(test_array_memcmp, i)                                                                                           \
		{                                                                                                                          \
			const char rplc = *test_array_memcmp[i].s2;                                                                        \
			if (rplc) {                                                                                                        \
				const char *s = test_array_memcmp[i].s1;                                                                   \
				size_t s_len = strlen(s);                                                                                  \
				fprintf(stderr, "string:%s.\n", s);                                                                        \
				fprintf(stderr, "replace:%c.\n", rplc);                                                                    \
				fprintf(stderr, "i:%zu\n", i);                                                                             \
				T_RPLC_INIT(buf, s, s_len);                                                                                \
				T_RPLC_INIT(s_buf, s, s_len);                                                                              \
				buf.size = func(buf.data, buf.size, rplc, i) - buf.data;                                                   \
				s_buf.size = simple_func(s_buf.data, s_buf.size, rplc, i) - s_buf.data;                                    \
				ASSERT_RESULT_CHR(func, buf, s_buf, s, rplc, strlen(buf.data) == strlen(s_buf.data));                      \
				ASSERT_RESULT_CHR(func, buf, s_buf, s, rplc, buf.size == s_buf.size);                                      \
				ASSERT_RESULT_CHR(func, buf, s_buf, s, rplc, !memcmp(buf.data, s_buf.data, buf.size * sizeof(*buf.data))); \
			}                                                                                                                  \
		}                                                                                                                          \
		jstr_free_j(&buf);                                                                                                         \
		jstr_free_j(&s_buf);                                                                                                       \
	} while (0)

static char *
simple_rplcnchr_p(char *s,
                  char remove,
                  size_t sz,
                  size_t n)
{
	for (; *s; ++s)
		if (*s == remove) {
			if (n--)
				break;
			*s = remove;
		}
	return (char *)s + strlen(s);
	(void)sz;
}

static char *
simple_rmnchr(char *s,
              char remove,
              size_t sz,
              size_t n)
{
	char *p = s;
	char *dst = p;
	for (; *p; ++p) {
		if (*p == remove) {
			if (n--)
				break;
			++p;
			continue;
		}
		*dst++ = *p;
	}
	return jstr_stpcpy_len(dst, p, strlen(p));
	(void)sz;
}

int
main(int argc, char **argv)
{
	jstr_ty j = JSTR_INIT;
	START();

	T_CHR(jstr_rmnchr_len_p, simple_rmnchr);

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
