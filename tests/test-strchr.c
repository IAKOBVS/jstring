/* Modified version of functional/strchr.c taken from musl's libc-test

   libc-test is licensed under the following standard MIT license:

   Copyright © 2005-2013 libc-test AUTHORS

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "test.h"
#include "../src/jstr.h"

static JSTR_ATTR_MAYBE_UNUSED char buf[512];

static JSTR_ATTR_MAYBE_UNUSED void *
aligned(void *p)
{
	return (void *)(((uintptr_t)p + 63) & (unsigned long)-64);
}

static JSTR_ATTR_MAYBE_UNUSED void *
aligncpy(const void *p, size_t len, size_t a)
{
	return memcpy((char *)aligned(buf) + a, p, len);
}

static JSTR_ATTR_MAYBE_UNUSED void *
simple_memrchr(const void *s,
               int c,
               size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMRCHR && !JSTR_USE_SIMPLE
	return memrchr(s, c, n);
#else
	const unsigned char *p = (const unsigned char *)s + n;
	for (; n--;)
		if (*--p == (unsigned char)c)
			return (void *)p;
	return NULL;
#endif
}

/*
   strchr() before s + T.
*/
static JSTR_ATTR_MAYBE_UNUSED char *
simple_strnchr(const char *s,
               int c,
               size_t n)
JSTR_NOEXCEPT
{
	for (; n--; ++s) {
		if (*s == (char)c)
			return (char *)s;
		if (jstr_unlikely(*s == '\0'))
			break;
	}
	return NULL;
}

static JSTR_ATTR_MAYBE_UNUSED char *
simple_strchrnul(const char *s,
                 int c)
{
#if JSTR_HAVE_STRCHRNUL && !JSTR_USE_SIMPLE
	return strchrnul(s, c);
#else
	const char *const p = strchr(s, c);
	return (char *)(p ? p : s + strlen(s));
#endif
}

static JSTR_ATTR_MAYBE_UNUSED char *
simple_strcasechrnul(const char *s,
                     int c)
{
	c = TOLOWER(c);
	for (;; ++s)
		if (TOLOWER(*s) == c || *s == '\0')
			break;
	return (char *)s;
}

static JSTR_ATTR_MAYBE_UNUSED char *
simple_strcasechr(const char *s,
                  int c)
{
	s = simple_strcasechrnul(s, c);
	return TOLOWER(*s) == TOLOWER(c) ? (char *)s : NULL;
}

static JSTR_ATTR_MAYBE_UNUSED char *
simple_strncasechr(const char *s,
                   int c,
                   size_t n)
{
	const char *p = simple_strcasechrnul(s, c);
	return p < s + n && TOLOWER(*p) == TOLOWER(c) ? (char *)p : NULL;
}

static JSTR_ATTR_MAYBE_UNUSED char *
simple_memcasechr(const void *s,
                  int c,
                  size_t n)
{
	c = jstr_tolower(c);
	const unsigned char *p = (const unsigned char *)s;
	for (; n && jstr_tolower(*p) != c; --n, ++p) {}
	return n ? (void *)p : NULL;
}

static JSTR_ATTR_MAYBE_UNUSED size_t
simple_countchr_len(const char *s,
                    int c,
                    size_t sz)
{
	size_t cnt = 0;
	for (; sz--; cnt += *s++ == (char)c) {}
	return cnt;
}

static JSTR_ATTR_MAYBE_UNUSED size_t
simple_countchr(const char *s,
                int c)
{
	return simple_countchr_len(s, c, strlen(s));
}

#define T_ASSERT(func, expr, result, expected, str, c, n)                              \
	do {                                                                           \
		if (jstr_unlikely(!(expr))) {                                          \
			PRINTERR("String:%s\n", str);                                  \
			PRINTERR("C:%c\n", c);                                         \
			PRINTERR("C ascii:%d\n", c);                                   \
			if ((size_t)n != (size_t)-1)                                   \
				PRINTERR("N:%zu\n", (size_t)n);                        \
			PRINTERR("Strlen():%zu\n", strlen(str));                       \
			PRINTERR("Result_len: %zu\n", JSTR_PTR_DIFF(result, str));     \
			PRINTERR("Expected_len: %zu\n", JSTR_PTR_DIFF(expected, str)); \
			PRINTERR("Ptr_result: %p\n", result);                          \
			PRINTERR("Ptr_expected: %p\n", expected);                      \
			ASSERT_RESULT(func, expr, result, expected);                   \
		}                                                                      \
	} while (0)

#define TCOUNT_ASSERT(func, expr, str, c, n)                       \
	do {                                                       \
		if (jstr_unlikely(!(expr))) {                      \
			PRINTERR("String:%s\n", str);              \
			PRINTERR("C:%c\n", c);                     \
			PRINTERR("C ascii:%d\n", c);               \
			if ((size_t)n != (size_t)-1)               \
				PRINTERR("N:%zu\n", (size_t)n);    \
			PRINTERR("Result: %zu\n", result_len);     \
			PRINTERR("Expected: %zu\n", expected_len); \
			PRINTERR(JSTR_ASSERT_FUNC);                \
			assert(expr);                              \
		}                                                  \
	} while (0)

#if defined __AVX2__
#	define T_AVX2(s, c, n)                                                                             \
		do {                                                                                        \
			result = pjstr_strchrnul_simd(s, c);                                                \
			expected = simple_strchrnul(s, c);                                                  \
			T_ASSERT(pjstr_strchrnul_simd, result == expected, result, expected, s, c, -1);     \
			result = pjstr_memrchr_simd(s, c, n);                                               \
			expected = simple_memrchr(s, c, n);                                                 \
			T_ASSERT(pjstr_memrchr_simd, result == expected, result, expected, s, c, n);        \
			result = pjstr_strcasechrnul_simd(s, c);                                            \
			expected = simple_strcasechrnul(s, c);                                              \
			T_ASSERT(pjstr_strcasechrnul_simd, result == expected, result, expected, s, c, -1); \
			result = pjstr_memcasechr_simd(s, c, n);                                            \
			expected = simple_memcasechr(s, c, n);                                              \
			T_ASSERT(pjstr_memcasechr_simd, result == expected, result, expected, s, c, n);     \
			result = pjstr_strnchr_simd(s, c, n);                                               \
			expected = simple_strnchr(s, c, n);                                                 \
			T_ASSERT(pjstr_strnchr_simd, result == expected, result, expected, s, c, n);        \
			result = pjstr_strncasechr_simd(s, c, n);                                           \
			expected = simple_strncasechr(s, c, n);                                             \
			T_ASSERT(pjstr_strncasechr_simd, result == expected, result, expected, s, c, n);    \
			result_len = pjstr_countchr_simd(s, c);                                             \
			expected_len = simple_countchr(s, c);                                               \
			TCOUNT_ASSERT(pjstr_countchr_simd, result_len == expected_len, s, c, -1);           \
			result_len = pjstr_countchr_len_simd(s, c, n);                                      \
			expected_len = simple_countchr_len(s, c, n);                                        \
			TCOUNT_ASSERT(pjstr_countchr_len_simd, result_len == expected_len, s, c, n);        \
		} while (0)
#else
#	define T_AVX2(s, c, n)
#endif

#define T(s, c)                                                                                             \
	do {                                                                                                \
		size_t align;                                                                               \
		for (align = 0; align < 8; align++) {                                                       \
			size_t result_len, expected_len;                                                    \
			const char *result, *expected, *p = aligncpy(s, sizeof(s), (size_t)align);          \
			(void)result_len, (void)expected_len;                                               \
			size_t p_len = strlen(p);                                                           \
			result = jstr_strncasechr(p, c, p_len);                                             \
			expected = simple_strncasechr(p, c, p_len);                                         \
			T_ASSERT(jstr_strncasechr, result == expected, result, expected, p, c, -1);         \
			result = jstr_strnchr(p, c, p_len);                                                 \
			expected = simple_strnchr(p, c, p_len);                                             \
			T_ASSERT(jstr_strnchr, result == expected, result, expected, p, c, -1);             \
			result = jstr_memrchr(p, c, p_len);                                                 \
			expected = simple_memrchr(p, c, p_len);                                             \
			T_ASSERT(jstr_memrchr, result == expected, result, expected, p, c, p_len);          \
			result = jstr_strchrnul(p, c);                                                      \
			expected = simple_strchrnul(p, c);                                                  \
			T_ASSERT(jstr_strchrnul, result == expected, result, expected, p, c, -1);           \
			result = jstr_strcasechr(p, c);                                                     \
			expected = simple_strcasechr(p, c);                                                 \
			T_ASSERT(jstr_strcasechr, result == expected, result, expected, p, c, -1);          \
			result = jstr_strcasechrnul(p, c);                                                  \
			expected = simple_strcasechrnul(p, c);                                              \
			T_ASSERT(jstr_strcasechrnul, result == expected, result, expected, p, c, -1);       \
			result = pjstr_memcasechr_musl(s, c, align);                                        \
			expected = simple_memcasechr(s, c, align);                                          \
			T_ASSERT(pjstr_memcasechr_musl, result == expected, result, expected, s, c, align); \
			T_AVX2(p, c, p_len);                                                                \
		}                                                                                           \
	} while (0)

int
main(int argc, char **argv)
{
	START();
	int i;
	char a[128];
	char s[256];
	TESTING(jstr_strnchr);
	TESTING(jstr_memrchr);
	TESTING(jstr_strnchr);
	TESTING(jstr_strchrnul);
	TESTING(jstr_strcasechr);
	TESTING(jstr_strcasechrnul);

	for (i = 0; i < 128; i++)
		a[i] = (i + 1) & 127;
	for (i = 0; i < 256; i++)
		*((unsigned char *)s + i) = i + 1;

	T("\0aaa", 'a');
	T("a\0bb", 'b');
	T("ab\0c", 'c');
	T("abc\0d", 'd');
	T("abc abc\0x", 'x');

	T(a, 128);
	T(a, 255);

	T("", 0);
	T("a", 'a');
	T("a", 'a' + 256);
	T("a", 0);
	T("abb", 'b');
	T("aabb", 'b');
	T("aaabb", 'b');
	T("aaaabb", 'b');
	T("aaaaabb", 'b');
	T("aaaaaabb", 'b');
	T("abc abc", 'c');

	T(s, 1);
	T(s, 2);
	T(s, 10);
	T(s, 11);
	T(s, 127);
	T(s, 128);
	T(s, 255);
	T(s, 0);

	SUCCESS();
	return EXIT_SUCCESS;
}
