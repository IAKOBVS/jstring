/* Based on musl's libc-bench
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

#define BUFLEN 500000

char *buf;

char *
simple_strchrnul(const char *s,
                 int c)
{
	char *const p = strchr(s, c);
	return *s == (char)c ? p : (char *)s + strlen(s);
}

void *
simple_memrchr(const void *s,
               int c,
               size_t n)
{
	const unsigned char *p = (const unsigned char *)s + n;
	while (n--)
		if (*--p == c)
			return (void *)p;
	return NULL;
}

char *
simple_strcasechrnul_strcspn(const char *s,
                             int c)
{
	const char a[] = { (char)jstr_tolower(c), (char)jstr_toupper(c), '\0' };
	return (char *)s + strcspn(s, a);
}

static size_t
simple_countchr_len(const char *s,
                    int c,
                    size_t sz)
{
	size_t cnt = 0;
	for (; sz--; cnt += *s++ == (char)c) {}
	return cnt;
}

static size_t
simple_countchr(const char *s,
                int c)
{
	return simple_countchr_len(s, c, strlen(s));
}

#define T_SETUP(buf, BUFLEN)              \
	do {                              \
		memset(buf, 'a', BUFLEN); \
		buf[BUFLEN - 1] = 0;      \
		buf[BUFLEN - 2] = 'b';    \
	} while (0)

#define T_DEFINE_STRCHR(impl_func, ...)                       \
	size_t b_##impl_func(void *dummy)                     \
	{                                                     \
		size_t i;                                     \
		size_t cs;                                    \
		for (i = 0; i < 100; i++) {                   \
			buf[i] = '0' + i % 8;                 \
			cs += (size_t)impl_func(__VA_ARGS__); \
		}                                             \
		return cs;                                    \
	}

#ifdef __AVX2__
T_DEFINE_STRCHR(jstr__simd_countchr, buf, 'b')
T_DEFINE_STRCHR(jstr__simd_countchr_len, buf, 'b', BUFLEN)
T_DEFINE_STRCHR(jstr__simd_strchrnul, buf, 'b')
T_DEFINE_STRCHR(jstr__simd_memrchr, buf, 'b', BUFLEN)
T_DEFINE_STRCHR(jstr__simd_strcasechrnul, buf, 'b')
T_DEFINE_STRCHR(jstr__simd_memcasechr, buf, 'b', BUFLEN)
T_DEFINE_STRCHR(jstr__simd_strnchr, buf, 'b', BUFLEN)
T_DEFINE_STRCHR(jstr__simd_strncasechr, buf, 'b', BUFLEN)
#endif

T_DEFINE_STRCHR(simple_countchr, buf, 'b')
T_DEFINE_STRCHR(simple_countchr_len, buf, 'b', BUFLEN)
T_DEFINE_STRCHR(simple_strcasechrnul_strcspn, buf, 'b')
T_DEFINE_STRCHR(jstr_strchrnul, buf, 'b')
T_DEFINE_STRCHR(simple_strchrnul, buf, 'b')
/* T_DEFINE_STRCHR(jstr__strchrnul_musl, buf, 'b') */
T_DEFINE_STRCHR(simple_memrchr, buf, 'b', BUFLEN)
T_DEFINE_STRCHR(jstr__memrchr_musl, buf, 'b', BUFLEN)
T_DEFINE_STRCHR(jstr_memrchr, buf, 'b', BUFLEN)
T_DEFINE_STRCHR(jstr__strcasechrnul_musl, buf, 'b')
T_DEFINE_STRCHR(jstr__memcasechr_musl, buf, 'b', BUFLEN)
#if JSTR_HAVE_STRCHRNUL
T_DEFINE_STRCHR(strchrnul, buf, 'b')
#endif
#if JSTR_HAVE_MEMRCHR
T_DEFINE_STRCHR(memrchr, buf, 'b', BUFLEN)
#endif

int
main()
{
	buf = malloc(BUFLEN + 1);
	assert(buf);
	T_SETUP(buf, BUFLEN);

#ifdef __AVX2__
	RUN(b_jstr__simd_countchr, 0);
	RUN(b_jstr__simd_countchr_len, 0);
	RUN(b_jstr__simd_strchrnul, 0);
	RUN(b_jstr__simd_memrchr, 0);
	RUN(b_jstr__simd_strcasechrnul, 0);
	RUN(b_jstr__simd_memcasechr, 0);
	RUN(b_jstr__simd_strnchr, 0);
	RUN(b_jstr__simd_strncasechr, 0);
#endif

	RUN(b_simple_countchr, 0);
	RUN(b_simple_countchr_len, 0);
	/* RUN(b_jstr__strchrnul_musl, 0); */
	RUN(b_jstr__strcasechrnul_musl, 0);
	RUN(b_jstr__memcasechr_musl, 0);
	RUN(b_jstr__memrchr_musl, 0);
	RUN(b_jstr_strchrnul, 0);
#if JSTR_HAVE_STRCHRNUL
	RUN(b_strchrnul, 0);
#endif
	RUN(b_simple_strchrnul, 0);
	RUN(b_simple_strcasechrnul_strcspn, 0);
#if JSTR_HAVE_MEMRCHR
	RUN(b_memrchr, 0);
#endif
	RUN(b_jstr_memrchr, 0);
	RUN(b_simple_memrchr, 0);

	free(buf);
}
