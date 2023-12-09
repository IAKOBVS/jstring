/* Based on musl's libc-bench

   Copyright © 2011 Rich Felker

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

#include "bench.h"

#include "../src/jstr-string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFLEN 500000

static char *
simple_memmem(const char *h,
              size_t hl,
              const char *n,
              size_t nl)
{
	if (jstr_unlikely(nl == 0))
		return (char *)h;
	if (jstr_unlikely(hl < nl))
		return NULL;
	for (; hl; --hl, ++h)
		if (*h == *n && !memcmp(h, n, nl))
			return (char *)h;
	return NULL;
}

static char *
simple_strrstr_len(const char *h,
                   size_t hl,
                   const char *n,
                   size_t nl)
{
	if (jstr_unlikely(nl == 0))
		return (char *)h;
	if (jstr_unlikely(hl < nl))
		return NULL;
	h -= nl;
	hl -= nl;
	for (; hl; --hl, --h)
		if (*h == *n && !memcmp(h, n, nl))
			return (char *)h;
	return NULL;
}

static char *
simple_strcasestr(const char *h,
                  const char *n)
{
	if (jstr_unlikely(*n == 0))
		return (char *)h;
	const char *p1, *p2;
	for (; *h; ++h) {
		for (p1 = h + 1, p2 = n + 1; *p1 && jstr_tolower(*p1) == jstr_tolower(*p2); ++p1, ++p2) {}
		if (*p2 == '\0')
			return (char *)h;
		if (jstr_unlikely(*p1 == '\0'))
			break;
	}
	return NULL;
}

static char *
simple_strstr(const char *h,
              const char *n)
{
	if (jstr_unlikely(*n == 0))
		return (char *)h;
	const char *p1, *p2;
	for (; *h; ++h) {
		for (p1 = h + 1, p2 = n + 1; *p1 && *p1 == *p2; ++p1, ++p2) {}
		if (*p2 == '\0')
			return (char *)h;
		if (jstr_unlikely(*p1 == '\0'))
			break;
	}
	return NULL;
}

#define T_DEFINE_STRSTR(impl_func, ...)                                        \
	static JSTR_ATTR_MAYBE_UNUSED size_t                                   \
	b_##impl_func(void *dummy)                                             \
	{                                                                      \
		const char *needle = dummy;                                    \
		const size_t needle_len = strlen(needle);                      \
		size_t i;                                                      \
		const size_t cnt = 10000;                                      \
		size_t cs = 0;                                                 \
		char *haystack = xmalloc(needle_len * cnt + 1);                \
		for (i = 0; i < cnt - 1; i++) {                                \
			memcpy(haystack + needle_len * i, needle, needle_len); \
			haystack[needle_len * i + needle_len - 1] ^= 1;        \
		}                                                              \
		memcpy(haystack + needle_len * i, needle, needle_len + 1);     \
		for (i = 0; i < 50; i++) {                                     \
			haystack[0] ^= 1;                                      \
			cs += (int)impl_func(__VA_ARGS__);                     \
		}                                                              \
		free(haystack);                                                \
		return cs;                                                     \
	}

T_DEFINE_STRSTR(simple_strrstr_len, haystack, strlen(haystack), needle, needle_len)
T_DEFINE_STRSTR(simple_memmem, haystack, strlen(haystack), needle, needle_len)
T_DEFINE_STRSTR(simple_strcasestr, haystack, needle)
T_DEFINE_STRSTR(simple_strstr, haystack, needle)
T_DEFINE_STRSTR(strstr, haystack, needle)
T_DEFINE_STRSTR(memmem, haystack, strlen(haystack), needle, needle_len)
T_DEFINE_STRSTR(jstr_memmem, haystack, strlen(haystack), needle, needle_len)
T_DEFINE_STRSTR(jstr_strstr_len, haystack, strlen(haystack), needle, needle_len)
T_DEFINE_STRSTR(strcasestr, haystack, needle)
T_DEFINE_STRSTR(jstr_strcasestr, haystack, needle)
T_DEFINE_STRSTR(jstr_strcasestr_len, haystack, strlen(haystack), needle, needle_len)
T_DEFINE_STRSTR(jstr_strrstr_len, haystack, strlen(haystack), needle, needle_len)

#define T_STRSTR_ALL(needle)                \
	RUN(b_simple_strcasestr, needle);   \
	RUN(b_simple_strstr, needle);       \
	RUN(b_simple_memmem, needle);       \
	RUN(b_simple_strstr, needle);       \
	RUN(b_strstr, needle);              \
	RUN(b_memmem, needle);              \
	RUN(b_jstr_memmem, needle);         \
	RUN(b_jstr_strstr_len, needle);     \
	RUN(b_strcasestr, needle);          \
	RUN(b_jstr_strcasestr, needle);     \
	RUN(b_jstr_strcasestr_len, needle); \
/* RUN(b_simple_strrstr_len, needle);  \ */
/* RUN(b_jstr_strrstr_len, needle); */

/* clang-format off */
#define DOUBLE(s) s#s
/* clang-format on */

#define T_STRSTR()                                                  \
	do {                                                        \
		T_STRSTR_ALL("a");                                  \
		T_STRSTR_ALL("ab");                                 \
		T_STRSTR_ALL("abc");                                \
		T_STRSTR_ALL("abcd");                               \
		T_STRSTR_ALL("efgha");                              \
		T_STRSTR_ALL("efghab");                             \
		T_STRSTR_ALL("efghabc");                            \
		T_STRSTR_ALL("efghabcd");                           \
		T_STRSTR_ALL("abcdefghijklmnopqrstuvwxyz");         \
		T_STRSTR_ALL("azbycxdwevfugthsirjqkplomn");         \
		T_STRSTR_ALL(DOUBLE("abcdefghijklmnopqrstuvwxyz")); \
		T_STRSTR_ALL(DOUBLE("azbycxdwevfugthsirjqkplomn")); \
		T_STRSTR_ALL("4");                                  \
		T_STRSTR_ALL("4b");                                 \
		T_STRSTR_ALL("4bc");                                \
		T_STRSTR_ALL("4bcd");                               \
		T_STRSTR_ALL("efgh4");                              \
		T_STRSTR_ALL("efgh4b");                             \
		T_STRSTR_ALL("efgh4bc");                            \
		T_STRSTR_ALL("efgh4bcd");                           \
		T_STRSTR_ALL("4bcdefghijklmnopqrstuvwxyz");         \
		T_STRSTR_ALL("4zbycxdwevfugthsirjqkplomn");         \
		T_STRSTR_ALL("A");                                  \
		T_STRSTR_ALL("Ab");                                 \
		T_STRSTR_ALL("Abc");                                \
		T_STRSTR_ALL("Abcd");                               \
		T_STRSTR_ALL("efghA");                              \
		T_STRSTR_ALL("efghAb");                             \
		T_STRSTR_ALL("efghAbc");                            \
		T_STRSTR_ALL("efghAbcd");                           \
		T_STRSTR_ALL("Abcdefghijklmnopqrstuvwxyz");         \
		T_STRSTR_ALL("Azbycxdwevfugthsirjqkplomn");         \
		T_STRSTR_ALL(DOUBLE("Abcdefghijklmnopqrstuvwxyz")); \
		T_STRSTR_ALL(DOUBLE("Azbycxdwevfugthsirjqkplomn")); \
	} while (0)

int
main()
{
	T_STRSTR();
	return 0;
}
