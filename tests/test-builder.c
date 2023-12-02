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

#define T_ASSERT(func, result, expected)                                                                           \
	do {                                                                                                       \
		ASSERT_RESULT(func, strcmp((result).data, expected) == 0, (result).data, expected);                \
		ASSERT_RESULT(func, (result).size == strlen(expected), (result).data, expected);                   \
		ASSERT_RESULT(func, memcmp((result).data, expected, (result).size) == 0, (result).data, expected); \
	} while (0)

#define T_APPEND(ret, func, ...)                  \
	do {                                      \
		TESTING(func);                    \
		if (ret != func(__VA_ARGS__)) {   \
			jstr_debug(&(result));    \
			jstr_errdie("");          \
		}                                 \
		T_ASSERT(func, result, expected); \
		(result).size = 0;                \
		*(result).data = '\0';            \
	} while (0)

#define FILL(result, str) assert(!jstr_chk(jstr_assign_len(JSTR_STRUCT(&(result)), str, strlen(str))))

#define T_RPLC_INIT(buf, str, str_len)                                               \
	do {                                                                         \
		jstr_empty(buf.data, &buf.size);                                     \
		assert(!jstr_chk(jstr_assign_len(JSTR_STRUCT(&buf), str, str_len))); \
		assert(buf.size == str_len);                                         \
		assert(!memcmp(buf.data, str, str_len));                             \
	} while (0)

#define ASSERT_RESULT_RMCHR(func, str, rplc, expr)                       \
	do {                                                             \
		if (jstr_unlikely(!(expr))) {                            \
			PRINTERR("\nassertion failure: %s().\n", #func); \
			PRINTERR("string:%s.\nfind:%c.\n", str, rplc);   \
			PRINTERR("result:\n");                           \
			jstr_debug(&(result));                           \
			PRINTERR("expected:\n");                         \
			jstr_debug(&(expected));                         \
			assert(expr);                                    \
		}                                                        \
	} while (0)

#define ASSERT_RESULT_RPLCCHR(func, str, find, rplc, expr)                             \
	do {                                                                           \
		if (jstr_unlikely(!(expr))) {                                          \
			PRINTERR("\nassertion failure: %s().\n", #func);               \
			PRINTERR("string:%s.\nfind:%c.\nrplc:%c.\n", str, find, rplc); \
			PRINTERR("result:\n");                                         \
			jstr_debug(&(result));                                         \
			PRINTERR("expected:\n");                                       \
			jstr_debug(&(expected));                                       \
			assert(expr);                                                  \
		}                                                                      \
	} while (0)

#define ASSERT_RESULT_RPLC(func, str, find, rplc, expr)                                \
	do {                                                                           \
		if (jstr_unlikely(!(expr))) {                                          \
			PRINTERR("\nassertion failure: %s().\n", #func);               \
			PRINTERR("string:%s.\nfind:%s.\nrplc:%s.\n", str, find, rplc); \
			PRINTERR("result:\n");                                         \
			jstr_debug(&(result));                                         \
			PRINTERR("expected:\n");                                       \
			jstr_debug(&(expected));                                       \
			assert(expr);                                                  \
		}                                                                      \
	} while (0)

#define T_RMCHR(func, simple_func)                                                                                                             \
	do {                                                                                                                                   \
		TESTING(func);                                                                                                                 \
		T_FOREACHI(test_array_memcmp, i)                                                                                               \
		{                                                                                                                              \
			const char *str = test_array_memcmp[i].s1;                                                                             \
			const char find = *test_array_memcmp[i].s2;                                                                            \
			size_t str_len = strlen(str);                                                                                          \
			T_RPLC_INIT((result), str, str_len);                                                                                   \
			T_RPLC_INIT((expected), str, str_len);                                                                                 \
			(result).size = JSTR_PTR_DIFF(func((result).data, (result).size, find, i), (result).data);                             \
			(expected).size = JSTR_PTR_DIFF(simple_func((expected).data, (expected).size, find, i), (expected).data);              \
			ASSERT_RESULT_RMCHR(func, str, find, strlen((result).data) == strlen((expected).data));                                \
			ASSERT_RESULT_RMCHR(func, str, find, (result).size == (expected).size);                                                \
			ASSERT_RESULT_RMCHR(func, str, find, !memcmp((result).data, (expected).data, (result).size * sizeof(*(result).data))); \
		}                                                                                                                              \
	} while (0)

#define T_RPLCCHR(func, simple_func)                                                                                                                   \
	do {                                                                                                                                           \
		TESTING(func);                                                                                                                         \
		T_FOREACHI(test_array_memcmp, i)                                                                                                       \
		{                                                                                                                                      \
			const char *str = test_array_memcmp[i].s1;                                                                                     \
			const char find = *test_array_memcmp[i].s2;                                                                                    \
			const char rplc = *test_array_memcmp[i].s1;                                                                                    \
			size_t str_len = strlen(str);                                                                                                  \
			T_RPLC_INIT((result), str, str_len);                                                                                           \
			T_RPLC_INIT((expected), str, str_len);                                                                                         \
			func((result).data, (result).size, find, rplc, i);                                                                             \
			simple_func((expected).data, (expected).size, find, rplc, i);                                                                  \
			ASSERT_RESULT_RPLCCHR(func, str, find, rplc, !memcmp((result).data, (expected).data, (result).size * sizeof(*(result).data))); \
		}                                                                                                                                      \
	} while (0)

#define T_RM(func, simple_func)                                                                                                                   \
	do {                                                                                                                                      \
		TESTING(func);                                                                                                                    \
		T_FOREACHI(test_array_memcmp, i)                                                                                                  \
		{                                                                                                                                 \
			const char *str = test_array_memcmp[i].s1;                                                                                \
			const char *find = test_array_memcmp[i].s2;                                                                               \
			size_t str_len = strlen(str);                                                                                             \
			size_t find_len = strlen(find);                                                                                           \
			T_RPLC_INIT((result), str, str_len);                                                                                      \
			T_RPLC_INIT((expected), str, str_len);                                                                                    \
			(result).size = JSTR_PTR_DIFF(func((result).data, (result).size, find, find_len, i), (result).data);                      \
			(expected).size = JSTR_PTR_DIFF(simple_func((expected).data, (expected).size, find, find_len, i), (expected).data);       \
			ASSERT_RESULT_RPLC(func, str, find, "", strlen((result).data) == strlen((expected).data));                                \
			ASSERT_RESULT_RPLC(func, str, find, "", (result).size == (expected).size);                                                \
			ASSERT_RESULT_RPLC(func, str, find, "", !memcmp((result).data, (expected).data, (result).size * sizeof(*(result).data))); \
		}                                                                                                                                 \
	} while (0)

#define T_RPLC(func, simple_func)                                                                                                                                           \
	do {                                                                                                                                                                \
		TESTING(func);                                                                                                                                              \
		T_FOREACHI(test_array_memcmp, i)                                                                                                                            \
		{                                                                                                                                                           \
			const char *str = test_array_memcmp[i].s1;                                                                                                          \
			const char *find = test_array_memcmp[i].s2;                                                                                                         \
			size_t find_len = strlen(find);                                                                                                                     \
			const char *rplc = find + find_len / 2;                                                                                                             \
			size_t str_len = strlen(str);                                                                                                                       \
			size_t rplc_len = strlen(rplc);                                                                                                                     \
			T_RPLC_INIT((result), str, str_len);                                                                                                                \
			T_RPLC_INIT((expected), str, str_len);                                                                                                              \
			assert(!jstr_chk(func(&(result).data, &(result).size, &(result).capacity, (i < str_len) ? i : 0, find, find_len, rplc, rplc_len, i)));              \
			assert(!jstr_chk(simple_func(&(expected).data, &(expected).size, &(expected).capacity, (i < str_len) ? i : 0, find, find_len, rplc, rplc_len, i))); \
			ASSERT_RESULT_RPLC(func, str, find, rplc, strlen((result).data) == strlen((expected).data));                                                        \
			ASSERT_RESULT_RPLC(func, str, find, rplc, (result).size == (expected).size);                                                                        \
			ASSERT_RESULT_RPLC(func, str, find, rplc, !memcmp((result).data, (expected).data, (result).size * sizeof(*(result).data)));                         \
		}                                                                                                                                                           \
	} while (0)

static void
simple_rplcnchr_len(char *s,
                    size_t sz,
                    char remove,
                    char replace,
                    size_t n)
{
	if (n == 0)
		return;
	for (; sz--; ++s)
		if (*s == remove) {
			if (n-- == 0)
				break;
			*s = replace;
		}
}

static char *
simple_rmnchr_len_p(char *s,
                    size_t sz,
                    char remove,
                    size_t n)
{
	if (n == 0)
		return s + sz;
	char *dst = s;
	for (; sz--;) {
		if (*s == remove && n) {
			--n;
			++s;
		} else {
			*dst++ = *s++;
		}
	}
	*dst = '\0';
	return dst;
}

static char *
simple_memmem(const void *hs,
              const size_t hl,
              const void *ne,
              const size_t nl)
{
	const unsigned char *h = (const unsigned char *)hs;
	const unsigned char *n = (const unsigned char *)ne;
	if (nl == 0)
		return (char *)h;
	if (hl == 0)
		return NULL;
	if (hl < nl)
		return NULL;
	const unsigned char *const end = h + hl - nl;
	for (; h <= end; ++h)
		if (*h == *n && !memcmp(h, n, nl))
			return (char *)h;
	return NULL;
}

JSTR_ATTR_MAYBE_UNUSED
static char *
simple_rmn_len_p(char *s,
                 size_t sz,
                 const char *find,
                 size_t find_len,
                 size_t n)
{
	char *p = s;
	for (; n-- && (p = simple_memmem(p, JSTR_PTR_DIFF(s + sz, p), find, find_len));) {
		*(char *)jstr_mempmove(p, p + find_len, JSTR_PTR_DIFF(s + sz, p + find_len)) = '\0';
		sz -= find_len;
		++p;
	}
	return s + sz;
}

static int
simple_rplcn_len_from(char **s,
                      size_t *sz,
                      size_t *cap,
                      size_t start_idx,
                      const char *find,
                      size_t find_len,
                      const char *rplc,
                      size_t rplc_len,
                      size_t n)
{
	char *p = *s + start_idx;
	for (; n-- && (p = simple_memmem(p, JSTR_PTR_DIFF(*s + *sz, p), find, find_len));) {
		p = pjstr_rplcat_len_higher(s, sz, cap, JSTR_PTR_DIFF(p, *s), rplc, rplc_len, find_len);
		if (jstr_unlikely(p == NULL))
			return JSTR_RET_ERR;
	}
	return JSTR_RET_SUCC;
}

int
main(int argc, char **argv)
{
	START();
	jstr_ty result = JSTR_INIT;
	{
		jstr_ty expected = JSTR_INIT;
		T_RMCHR(jstr_rmnchr_len_p, simple_rmnchr_len_p);
		T_RPLCCHR(jstr_rplcnchr_len, simple_rplcnchr_len);
		T_RM(jstr_rmn_len_p, simple_rmn_len_p);
		T_RPLC(jstr_rplcn_len_from, simple_rplcn_len_from);
		jstr_free_j(&expected);
	}
	jstr_empty(result.data, &result.size);
	const char *expected, *find, *rplc;
	regex_t preg;
	/* jstr-builder tests. */
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_cat, JSTR_STRUCT(&result), "hello", " ", "world", NULL);
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_append_len, JSTR_STRUCT(&result), expected, strlen(expected));
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_prepend_len, JSTR_STRUCT(&result), expected, strlen(expected));
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_assign_len, JSTR_STRUCT(&result), expected, strlen(expected));
	/* jstr-replace tests. */
	FILL(result, "hello hello hello hello");
	find = "hello";
	rplc = "world";
	expected = "world hello hello hello";
	T_APPEND(JSTR_RET_SUCC, jstr_rplc_len, JSTR_STRUCT(&result), find, strlen(find), rplc, strlen(rplc));
	FILL(result, "hello hello hello hello");
	find = "hello";
	rplc = "world";
	expected = "world world world world";
	T_APPEND(JSTR_RET_SUCC, jstr_rplcall_len, JSTR_STRUCT(&result), find, strlen(find), rplc, strlen(rplc));
	FILL(result, "hello hello hello hello");
	find = "hello";
	rplc = "";
	expected = "   ";
	T_APPEND(JSTR_RET_SUCC, jstr_rplcall_len, JSTR_STRUCT(&result), find, strlen(find), rplc, strlen(rplc));
	FILL(result, "hello hello hello hello");
	find = "hello";
	rplc = "";
	expected = "   ";
	T_APPEND(JSTR_RET_SUCC, jstr_rplcall_len, JSTR_STRUCT(&result), find, strlen(find), rplc, strlen(rplc));
	FILL(result, "hello hello hello hello");
	find = "[0-9A-Za-z]*";
	rplc = "";
	expected = "   ";
	/* jstr-regex tests. */
	assert(!jstrre_chkcomp(jstrre_comp(&preg, find, 0)));
	T_APPEND(JSTRRE_RET_NOERROR, jstrre_rplcall_len, &preg, JSTR_STRUCT(&result), rplc, strlen(rplc), 0);
	jstrre_free(&preg);
	FILL(result, "hello hello hello hello");
	find = "\\([0-9A-Za-z]*\\)";
	rplc = "\\1\\1";
	expected = "hellohello hellohello hellohello hellohello";
	assert(!jstrre_chkcomp(jstrre_comp(&preg, find, 0)));
	T_APPEND(JSTRRE_RET_NOERROR, jstrre_rplcall_bref_len, &preg, JSTR_STRUCT(&result), rplc, strlen(rplc), 0, 2);
	jstrre_free(&preg);
	jstr_free_j(&result);
	SUCCESS();
	return EXIT_SUCCESS;
	(void)simple_rmn_len_p;
	(void)argc;
}
