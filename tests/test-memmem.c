/* Copyright (c) 2023 James Tirta Halim <tirtajames45 at gmail dot com>
   This file is part of the jstring library.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:\n

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

#define JSTR_DISABLE_NONSTANDARD 1

#include "test.h"

#define TOLOWER(c) (unsigned char)(((c) >= 'A' && (c) <= 'Z') ? ((c) - 'A' + 'a') : (c))
#define TOUPPER(c) (unsigned char)(((c) >= 'a' && (c) <= 'z') ? ((c) - 'a' + 'A') : (c))

char *
simple_strrstr_len(const char *h,
                   const size_t hl,
                   const char *n,
                   const size_t nl)
{
	if (hl < nl)
		return NULL;
	const char *p = h + hl - nl;
	for (; p >= h; --p)
		if (!memcmp(p, n, nl))
			return (char *)p;
	return NULL;
}

char *
simple_strrstr(const char *h,
               const char *n)
{
	return simple_strrstr_len(h, strlen(h), n, strlen(n));
}

char *
simple_memmem(const char *h,
              const size_t hl,
              const char *n,
              const size_t nl)
{
	if (nl == 0)
		return (char *)h;
	if (hl == 0)
		return NULL;
	if (hl < nl)
		return NULL;
	const char *const end = h + hl - nl;
	for (; h <= end; ++h)
		if (!memcmp(h, n, nl))
			return (char *)h;
	return NULL;
}

char *
simple_strnstr(const char *hs,
               const char *ne,
               size_t n)
{
	return (char *)memmem(hs, jstr_strnlen(hs, n), ne, strlen(ne));
}

char *
simple_strstr(const char *h,
              const char *n)
{
	return simple_memmem(h, strlen(h), n, strlen(n));
}

int
simple_strncasecmp(const char *s1,
                   const char *s2,
                   size_t n)
{
	if (n == 0)
		return 0;
	int ret;
	for (; (ret = (TOLOWER(*s1) - TOLOWER(*s2))) == 0 && *s1 && --n; ++s1, ++s2)
		;
	return ret;
}

char *
simple_strcasestr_len(const char *h,
                      const size_t hl,
                      const char *n,
                      const size_t nl)
{
	if (nl == 0)
		return (char *)h;
	if (hl == 0)
		return NULL;
	if (hl < nl)
		return NULL;
	const char *end = h + hl - nl;
	for (; h <= end; ++h)
		if (!simple_strncasecmp(h, n, nl))
			return (char *)h;
	return NULL;
}

char *
simple_strcasestr(const char *h,
                  const char *n)
{
	return simple_strcasestr_len(h, strlen(h), n, strlen(n));
}

char *
jstr_strrstr(const char *h, const char *n)
{
	return jstr_strrstr_len(h, strlen(h), n, strlen(n));
}

#define T_STRSTR(fn, simple_fn)                                                           \
	do {                                                                              \
		TESTING(fn);                                                              \
		for (size_t i = 0; i < JSTR_ARRAY_SIZE(hs_ne); ++i) {                     \
			const char *const result = fn(hs_ne[i].hs, hs_ne[i].ne);          \
			const char *const expected = simple_fn(hs_ne[i].hs, hs_ne[i].ne); \
			ASSERT(result == expected, result, expected);                     \
		}                                                                         \
	} while (0)

#define T_STRSTR_LEN(fn, simple_fn)                                    \
	do {                                                           \
		TESTING(fn);                                           \
		const char *hs, *ne, *result, *expected;               \
		size_t hs_len, ne_len;                                 \
		for (size_t i = 0; i < JSTR_ARRAY_SIZE(hs_ne); ++i) {  \
			hs = hs_ne[i].hs;                              \
			ne = hs_ne[i].ne;                              \
			hs_len = strlen(hs);                           \
			ne_len = strlen(ne);                           \
			result = fn(hs, hs_len, ne, ne_len);           \
			expected = simple_fn(hs, hs_len, ne, ne_len);  \
			if (jstr_unlikely(result != expected)) {       \
				PRINTERR("hs:\n%s\n", hs);             \
				PRINTERR("ne:\n%s\n", ne);             \
				PRINTERR("hs_len:\n%zu\n", hs_len);    \
				PRINTERR("ne_len:\n%zu\n", ne_len);    \
				PRINTERR("expected:\n%s\n", expected); \
				PRINTERR("result:\n%s\n", result);     \
				PRINTERR("result_len:\n%zu\n", strlen(result)); \
				assert(result == expected);            \
			}                                              \
		}                                                      \
	} while (0)

#define T_STRNSTR(fn, simple_fn)                                                \
	do {                                                                    \
		TESTING(fn);                                                    \
		size_t n;                                                       \
		const char *hs, *ne, *result, *expected;                        \
		size_t hs_len, ne_len;                                          \
		for (size_t i = 0; i < JSTR_ARRAY_SIZE(hs_ne); ++i) {           \
			n = strlen(hs_ne[i].hs);                                \
			n = JSTR_MIN(n, i);                                     \
			hs = hs_ne[i].hs;                                       \
			ne = hs_ne[i].ne;                                       \
			hs_len = strlen(hs);                                    \
			ne_len = strlen(ne);                                    \
			result = fn(hs, ne, n);                                 \
			expected = simple_fn(hs, ne, n);                        \
			if (jstr_unlikely(result != expected)) {                \
				PRINTERR("hs:\n%s\n", hs);                      \
				PRINTERR("hsn:\n");                             \
				fwrite(hs, 1, jstr_strnlen(hs, n), stderr);     \
				PRINTERR("\n");                                 \
				PRINTERR("ne:\n%s\n", ne);                      \
				PRINTERR("hs_len:\n%zu\n", hs_len);             \
				PRINTERR("ne_len:\n%zu\n", ne_len);             \
				PRINTERR("n:\n%zu\n", n);                       \
				PRINTERR("expected:\n%s\n", expected);          \
				PRINTERR("result:\n%s\n", result);              \
				PRINTERR("result_len:\n%zu\n", strlen(result)); \
				assert(result == expected);                     \
			}                                                       \
		}                                                               \
	} while (0)

int
main(int argc, char **argv)
{
	struct hs_ne_ty {
		const char *hs;
		const char *ne;
	} hs_ne[] = {
		{"yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy",
                 "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"},
		{ "xxyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy",
                 "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"},
		{ "xxyyyy,yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy",
                 "yyyyyyyyyyyyyyyyy,yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"},
		{ "xxyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyxx",
                 "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"},
		{ "yxxyyyyxyxxxxy",
                 "yyyyy"		                                                                                                                                                                                                                                                    },
		{ ",yyy,xxxyxxxxy",
                 ",yyy,"		                                                                                                                                                                                                                                                    },
		{ "yxyyyxxxyxxxxyyyy",
                 "yyyyy"		                                                                                                                                                                                                                                                    },
		{ "yxxyyyyxyxxxxy",
                 "xxxxx"		                                                                                                                                                                                                                                                    },
		{ "yxxyyyyxyxxxxy",
                 "xyyyx"		                                                                                                                                                                                                                                                    },
		{ ",xxyyyyxyxxxxy",
                 "xyy,y"		                                                                                                                                                                                                                                                    },
		{ "yxheL,yyyyxyxxxxy",
                 "yyheL,"		                                                                                                                                                                                                                                                   },
		{ "yxxyyyyxyxxxxy",
                 "yyyyy"		                                                                                                                                                                                                                                                    },
		{ "yyyyyxxxyxxxxy",
                 "yyyyy"		                                                                                                                                                                                                                                                    },
		{ "hello world",
                 "hello"		                                                                                                                                                                                                                                                    },
		{ "hello world",
                 "world"		                                                                                                                                                                                                                                                    },
		{ "hello world",
                 "o w"		                                                                                                                                                                                                                                                      },
		{ "hello world",
                 "hel"		                                                                                                                                                                                                                                                      },
		{ "yxxx,xxxxy",
                 "xxx,"		                                                                                                                                                                                                                                                     },
		{ "yxxxyxxxxy",
                 "xxxx"		                                                                                                                                                                                                                                                     },
		{ "yxxxyxxxxy",
                 "xxx"		                                                                                                                                                                                                                                                      },
		{ "yxxxyxxy",
                 "xxx"		                                                                                                                                                                                                                                                      },
		{ "xxx",
                 "xxx"		                                                                                                                                                                                                                                                      },
		{ "xxx",
                 "x"		                                                                                                                                                                                                                                                        },
		{ "xxx",
                 "yyy"		                                                                                                                                                                                                                                                      },
		{ ",",
                 "x,x"		                                                                                                                                                                                                                                                      },
		{ "x",
                 "xxx"		                                                                                                                                                                                                                                                      },
		{ "xxx",
                 ""		                                                                                                                                                                                                                                                         },
		{ "",
                 "xxx"		                                                                                                                                                                                                                                                      },
		{ "",
                 ""		                                                                                                                                                                                                                                                         }
	};
	T_STRSTR(jstr_strcasestr, simple_strcasestr);
	T_STRSTR_LEN(jstr_strcasestr_len, simple_strcasestr_len);
	T_STRSTR_LEN(jstr_memmem, simple_memmem);
	T_STRSTR_LEN(jstr_strrstr_len, simple_strrstr_len);
	T_STRNSTR(jstr_strnstr, simple_strnstr);
	SUCCESS();
	return EXIT_SUCCESS;
}
