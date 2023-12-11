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

char *
simple_strchrnul(const char *s,
                 int c)
{
	char *const p = strchr(s, c);
	return *s == (char)c ? p : (char *)s + strlen(s);
}

#define T_DEFINE_STRCHR(impl_func, ...)                    \
	size_t b_##impl_func(void *dummy)                  \
	{                                                  \
		char *buf = malloc(BUFLEN);                \
		size_t i;                                  \
		size_t cs;                                 \
		memset(buf, 'a', BUFLEN);                  \
		buf[BUFLEN - 1] = 0;                       \
		buf[BUFLEN - 2] = 'b';                     \
		for (i = 0; i < 100; i++) {                \
			buf[i] = '0' + i % 8;              \
			cs += (int)impl_func(__VA_ARGS__); \
		}                                          \
		free(buf);                                 \
		return cs;                                 \
	}

T_DEFINE_STRCHR(pjstr_strchrnul_avx2, buf, 'b')
T_DEFINE_STRCHR(strchrnul, buf, 'b')
T_DEFINE_STRCHR(simple_strchrnul, buf, 'b')

int
main()
{
	RUN(b_pjstr_strchrnul_avx2, 0);
	RUN(b_strchrnul, 0);
	RUN(b_simple_strchrnul, 0);
}
