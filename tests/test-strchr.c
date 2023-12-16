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

#define T(s, c)                                                                 \
	do {                                                                    \
		size_t align;                                                   \
		for (align = 0; align < 8; align++) {                           \
			const char *p = aligncpy(s, sizeof(s), (size_t)align);  \
			t_init();                                               \
			t.n = strlen(p);                                        \
			t.result = jstr_strncasechr(p, c, t.n);                 \
			t.expected = simple_strncasechr(p, c, t.n);             \
			T_DEBUG(jstr_strncasechr, t.result == t.expected);      \
			t.result = jstr_strnchr(p, c, t.n);                     \
			t.expected = simple_strnchr(p, c, t.n);                 \
			T_DEBUG(jstr_strnchr, t.result == t.expected);          \
			t.result = jstr_memrchr(p, c, t.n);                     \
			t.expected = simple_memrchr(p, c, t.n);                 \
			T_DEBUG(jstr_memrchr, t.result == t.expected);          \
			t.result = jstr_strchrnul(p, c);                        \
			t.expected = simple_strchrnul(p, c);                    \
			T_DEBUG(jstr_strchrnul, t.result == t.expected);        \
			t.result = jstr_strcasechr(p, c);                       \
			t.expected = simple_strcasechr(p, c);                   \
			T_DEBUG(jstr_strcasechr, t.result == t.expected);       \
			t.result = jstr_strcasechrnul(p, c);                    \
			t.expected = simple_strcasechrnul(p, c);                \
			T_DEBUG(jstr_strcasechrnul, t.result == t.expected);    \
			t.result = pjstr_memcasechr_musl(s, c, t.n);            \
			t.expected = simple_memcasechr(s, c, t.n);              \
			T_DEBUG(pjstr_memcasechr_musl, t.result == t.expected); \
			t.result_n = jstr_countchr_len(s, c, t.n);              \
			t.expected_n = simple_countchr_len(s, c, t.n);          \
			T_DEBUG(jstr_countchr_len, t.result_n == t.expected_n); \
			t.result_n = jstr_countchr(s, c);                       \
			t.expected_n = simple_countchr(s, c);                   \
			T_DEBUG(jstr_countchr, t.result_n == t.expected_n);     \
		}                                                               \
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
	TESTING(jstr_countchr);
	TESTING(jstr_countchr_len);

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
