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

#define T_SETUP(buf, BUFLEN)              \
	do {                              \
		memset(buf, 'a', BUFLEN); \
		buf[BUFLEN - 1] = 0;      \
		buf[BUFLEN - 2] = 'b';    \
	} while (0)

#define T_DEFINE_STRCHR(impl_func, ...)                    \
	size_t b_##impl_func(void *dummy)                  \
	{                                                  \
		size_t i;                                  \
		size_t cs;                                 \
		for (i = 0; i < 100; i++) {                \
			buf[i] = '0' + i % 8;              \
			cs += (int)impl_func(__VA_ARGS__); \
		}                                          \
		return cs;                                 \
	}

#ifdef __AVX2__
T_DEFINE_STRCHR(pjstr_strchrnul_avx2, buf, 'b')
T_DEFINE_STRCHR(pjstr_memrchr_avx2, buf, 'b', strlen(buf))
#endif
T_DEFINE_STRCHR(jstr_strchrnul, buf, 'b')
T_DEFINE_STRCHR(strchrnul, buf, 'b')
T_DEFINE_STRCHR(simple_strchrnul, buf, 'b')
T_DEFINE_STRCHR(memrchr, buf, 'b', strlen(buf))
T_DEFINE_STRCHR(jstr_memrchr, buf, 'b', strlen(buf))
T_DEFINE_STRCHR(simple_memrchr, buf, 'b', strlen(buf))

int
main()
{
	buf = malloc(BUFLEN);
	assert(buf);

	T_SETUP(buf, BUFLEN);

#ifdef __AVX2__
	RUN(b_pjstr_strchrnul_avx2, 0);
#endif
	RUN(b_jstr_strchrnul, 0);
	RUN(b_strchrnul, 0);
	RUN(b_simple_strchrnul, 0);
	RUN(b_memrchr, 0);
	RUN(b_jstr_memrchr, 0);
	RUN(b_pjstr_memrchr_avx2, 0);
	RUN(b_simple_memrchr, 0);

	free(buf);
}