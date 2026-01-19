/* SPDX-License-Identifier: MIT */
/* This file is part of the jstring library, and is based on musl's
 * libc-bench
 *
 * libc-bench is licensed under the following standard MIT license:
 *
 * Copyright © 2011 Rich Felker
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "bench.h"

#include "../src/jstr-string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFLEN 500000U
#define CNT    10000U

size_t needle_len;
size_t buf_len;
char *buf;

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

static char *
simple_strnstr(const char *hs,
               const char *ne,
               size_t n)
{
	if (jstr_unlikely(*ne == 0))
		return (char *)hs;
	const char *p1, *p2;
	size_t k;
	for (; *hs && n; ++hs, --n) {
		for (p1 = hs + 1, p2 = ne + 1, k = n - 1; k && *p1 && *p1 == *p2; --k, ++p1, ++p2) {}
		if (jstr_unlikely(k == 0) || *p2 == '\0')
			return (char *)hs;
		if (jstr_unlikely(*p1 == '\0'))
			break;
	}
	return NULL;
}

static char *
simple_stpcpy(char *JSTR_RESTRICT dst,
              const char *JSTR_RESTRICT src)
{
	size_t n = strlen(src);
	*(char *)memcpy(dst, src, n) = '\0';
	return dst + n;
}

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
		T_STRSTR_ALL("efghabcde");                          \
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
		T_STRSTR_ALL("efgh4bcde");                          \
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

#define T_SETUP(buf, needle, needle_len)                                  \
	do {                                                              \
		size_t i;                                                 \
		size_t cnt = 10000;                                       \
		for (i = 0; i < cnt - 1; i++) {                           \
			memcpy(buf + needle_len * i, needle, needle_len); \
			buf[needle_len * i + needle_len - 1] ^= 1;        \
		}                                                         \
		memcpy(buf + needle_len * i, needle, needle_len + 1);     \
	} while (0)

#define T_DEFINE_STRSTR(impl_func, ...)                       \
	static JSTR_ATTR_MAYBE_UNUSED size_t                  \
	b_##impl_func(void *dummy)                            \
	{                                                     \
		const char *needle = dummy;                   \
		const size_t needle_len = needle_len;         \
		size_t cs = 0;                                \
		for (size_t i = 0; i < 50; i++) {             \
			buf[0] ^= 1;                          \
			cs += (size_t)impl_func(__VA_ARGS__); \
		}                                             \
		return cs;                                    \
	}

#define T_STRSTR_ALL(needle)                                          \
	JSTR_STATIC_ASSERT(sizeof(needle) - 1 <= (BUFLEN * CNT), ""); \
	needle_len = sizeof(needle) - 1;                              \
	buf_len = needle_len * CNT;                                   \
	buf = realloc(buf, buf_len + 1);                              \
	assert(buf);                                                  \
	T_SETUP(buf, needle, needle_len);                             \
	RUN(b_simple_strcasestr, needle);                             \
	RUN(b_simple_strstr, needle);                                 \
	RUN(b_simple_memmem, needle);                                 \
	RUN(b_simple_strstr, needle);                                 \
	RUN(b_strstr, needle);                                        \
	RUN(b_memmem, needle);                                        \
	RUN(b_jstr_memmem, needle);                                   \
	RUN(b_jstr_strstr_len, needle);                               \
	RUN(b_strcasestr, needle);                                    \
	RUN(b_jstr_strcasestr, needle);                               \
	RUN(b_jstr_strcasestr_len, needle);                           \
	RUN(b_simple_strnstr, needle);                                \
	RUN(b_simple_strrstr_len, needle);                            \
	RUN(b_jstr_strrstr_len, needle);                              \
	T_AVX(needle);

T_DEFINE_STRSTR(simple_memmem, buf, buf_len, needle, needle_len)
T_DEFINE_STRSTR(simple_strcasestr, buf, needle)
T_DEFINE_STRSTR(simple_strnstr, buf, needle, buf_len)
T_DEFINE_STRSTR(simple_strstr, buf, needle)
T_DEFINE_STRSTR(strstr, buf, needle)
T_DEFINE_STRSTR(memmem, buf, buf_len, needle, needle_len)
T_DEFINE_STRSTR(jstr_memmem, buf, buf_len, needle, needle_len)
T_DEFINE_STRSTR(jstr_strstr_len, buf, buf_len, needle, needle_len)
T_DEFINE_STRSTR(strcasestr, buf, needle)
T_DEFINE_STRSTR(jstr_strcasestr, buf, needle)
T_DEFINE_STRSTR(jstr_strcasestr_len, buf, buf_len, needle, needle_len)
T_DEFINE_STRSTR(simple_strrstr_len, buf, buf_len, needle, needle_len)
T_DEFINE_STRSTR(jstr_strrstr_len, buf, buf_len, needle, needle_len)
T_DEFINE_STRSTR(jstr_stpcpy, buf, buf + i)
T_DEFINE_STRSTR(simple_stpcpy, buf, buf + i)

#ifdef __AVX2__
T_DEFINE_STRSTR(jstr__simd_memmem, buf, buf_len, needle, needle_len)
T_DEFINE_STRSTR(jstr__simd_stpcpy, buf, buf + i)
#	define T_AVX(needle)                    \
		RUN(b_jstr__simd_memmem, needle); \

#else
#	define T_AVX(needle)
#endif

/* clang-format off */
#define DOUBLE(s) s#s
/* clang-format on */

int
main()
{
	T_STRSTR();
	RUN(b_jstr_stpcpy, 0);
	RUN(b_simple_stpcpy, 0);
	RUN(b_jstr__simd_stpcpy, 0);
	free(buf);
	return 0;
}
