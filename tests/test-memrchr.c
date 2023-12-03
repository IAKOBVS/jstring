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

static char buf[512];

static void *
aligned(void *p)
{
	return (void *)(((uintptr_t)p + 63) & (unsigned long)-64);
}

static void *
aligncpy(const void *p, size_t len, size_t a)
{
	return memcpy((char *)aligned(buf) + a, p, len);
}

static void *
simple_memrchr(const void *s,
               int c,
               size_t sz)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMRCHR
	return (void *)memrchr(s, c, sz);
#else
	const unsigned char *p = (const unsigned char *)s + sz;
	for (; sz--;)
		if (*--p == (unsigned char)c)
			return (void *)p;
	return NULL;
#endif
}

/*
   strchr() before s + N.
*/
static char *
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

#define N(s, c)                                                                                   \
	do {                                                                                      \
		int align;                                                                        \
		for (align = 0; align < 8; align++) {                                             \
			const char *result, *expected, *p = aligncpy(s, sizeof s, (size_t)align); \
			size_t p_len = strlen(p);                                                 \
			result = jstr_memrchr(p, c, p_len);                                       \
			expected = simple_memrchr(p, c, p_len);                                   \
			ASSERT_RESULT(jstr_memrchr, result == expected, result, expected);        \
			result = jstr_strnchr(p, c, p_len);                                       \
			expected = simple_strnchr(p, c, p_len);                                   \
			ASSERT_RESULT(jstr_strnchr, result == expected, result, expected);        \
		}                                                                                 \
	} while (0)

#define T(s, c, n)                                                                                \
	do {                                                                                      \
		int align;                                                                        \
		for (align = 0; align < 8; align++) {                                             \
			const char *result, *expected, *p = aligncpy(s, sizeof s, (size_t)align); \
			size_t p_len = strlen(p);                                                 \
			result = jstr_memrchr(p, c, p_len);                                       \
			expected = simple_memrchr(p, c, p_len);                                   \
			ASSERT_RESULT(jstr_memrchr, result == expected, result, expected);        \
			result = jstr_strnchr(p, c, p_len);                                       \
			expected = simple_strnchr(p, c, p_len);                                   \
			ASSERT_RESULT(jstr_strnchr, result == expected, result, expected);        \
		}                                                                                 \
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

	for (i = 0; i < 128; i++)
		a[i] = (i + 1) & 127;
	for (i = 0; i < 256; i++)
		*((unsigned char *)s + i) = i + 1;

	N("\0aaa", 'a');
	N("a\0bb", 'b');
	N("ab\0c", 'c');
	N("abc\0d", 'd');
	N("abc abc\0x", 'x');
	N(a, 128);
	N(a, 255);

	T("", 0, 0);
	T("a", 'a', 0);
	T("a", 'a' + 256, 0);
	T("a", 0, 1);
	T("abb", 'b', 1);
	T("aabb", 'b', 2);
	T("aaabb", 'b', 3);
	T("aaaabb", 'b', 4);
	T("aaaaabb", 'b', 5);
	T("aaaaaabb", 'b', 6);
	T("abc abc", 'c', 2);
	T(s, 1, 0);
	T(s, 2, 1);
	T(s, 10, 9);
	T(s, 11, 10);
	T(s, 127, 126);
	T(s, 128, 127);
	T(s, 255, 254);
	T(s, 0, 255);

	SUCCESS();
	return EXIT_SUCCESS;
}
