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

#include "test.h"

#define TOLOWER(c) (unsigned char)(((c) >= 'A' && (c) <= 'Z') ? ((c) - 'A' + 'a') : (c))
#define TOUPPER(c) (unsigned char)(((c) >= 'a' && (c) <= 'z') ? ((c) - 'a' + 'A') : (c))

char *
simple_strrstr_len(const char *h,
                   const size_t hl,
                   const char *n,
                   const size_t nl)
{
	if (nl == 0)
		return (char *)h + hl;
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

#define T_DEBUG(hs, ne, hs_len, ne_len, n, result, expected)        \
	if (jstr_unlikely(result != expected)) {                    \
		PRINTERR("hsn:\n");                                 \
		PRINTERR("hs:\n%s\n", hs);                          \
		PRINTERR("ne:\n%s\n", ne);                          \
		fwrite(hs, 1, jstr_strnlen(hs, n), stderr);         \
		PRINTERR("\n");                                     \
		PRINTERR("hs_len:\n%zu\n", hs_len);                 \
		PRINTERR("ne_len:\n%zu\n", ne_len);                 \
		PRINTERR("n:\n%zu\n", (size_t)n);                   \
		PRINTERR("expected:\n%s\n", N(expected));           \
		PRINTERR("expected_len:\n%zu\n", strlen(expected)); \
		PRINTERR("result:\n%s\n", N(result));               \
		PRINTERR("result_len:\n%zu\n", strlen(result));     \
		assert(result == expected);                         \
	}

#define T_FOREACHI(array, i) for (size_t i = 0; i < JSTR_ARRAY_SIZE(array); ++i)
#define T_HS(test, i)        ((test)[i].hs)
#define T_NE(test, i)        ((test)[i].ne)

#define T_STRSTR(fn, simple_fn)                         \
	do {                                            \
		TESTING(fn);                            \
		T_FOREACHI(test, i)                     \
		{                                       \
			const char *hs = T_HS(test, i); \
			const char *ne = T_NE(test, i); \
			T_DEBUG(hs,                     \
			        ne,                     \
			        strlen(hs),             \
			        strlen(ne),             \
			        0,                      \
			        N(fn(hs, ne)),          \
			        N(simple_fn(hs, ne)));  \
		}                                       \
	} while (0)

#define T_STRSTR_LEN(fn, simple_fn)                                                  \
	do {                                                                         \
		TESTING(fn);                                                         \
		T_FOREACHI(test, i)                                                  \
		{                                                                    \
			const char *hs = T_HS(test, i);                              \
			const char *ne = T_NE(test, i);                              \
			const size_t hs_len = strlen(hs);                            \
			const size_t ne_len = strlen(ne);                            \
			T_DEBUG(hs,                                                  \
			        ne,                                                  \
			        hs_len,                                              \
			        ne_len,                                              \
			        0,                                                   \
			        N((const char *)fn(hs, hs_len, ne, ne_len)),         \
			        N((const char *)simple_fn(hs, hs_len, ne, ne_len))); \
		}                                                                    \
	} while (0)

#define T_STRNSTR(fn, simple_fn)                                        \
	do {                                                            \
		TESTING(fn);                                            \
		T_FOREACHI(test, i)                                     \
		{                                                       \
			size_t n = strlen(test[i].hs);                  \
			n = JSTR_MIN(n, i);                             \
			const char *hs = T_HS(test, i);                 \
			const char *ne = T_NE(test, i);                 \
			const size_t hs_len = strlen(hs);               \
			const size_t ne_len = strlen(ne);               \
			T_DEBUG(hs,                                     \
			        ne,                                     \
			        hs_len,                                 \
			        ne_len,                                 \
			        n,                                      \
			        N((const char *)fn(hs, ne, n)),         \
			        N((const char *)simple_fn(hs, ne, n))); \
		}                                                       \
	} while (0)

int
main(int argc, char **argv)
{
	START();
	struct test_ty {
		const char *hs;
		const char *ne;
	} test[] = {
  /* clang-format off */
		{"yxxyyyyxyxxxxy","yyyyy"},
		{",yyy,xxxyxxxxy",",yyy,"},
		{"yxyyyxxxyxxxxyyyy","yyyyy"},
		{"yxxyyyyxyxxxxy","xxxxx"},
		{"yxxyyyyxyxxxxy","xyyyx"},
		{",xxyyyyxyxxxxy","xyy,y"},
		{"yxheL,yyyyxyxxxxy","yyheL,"},
		{"yxxyyyyxyxxxxy","yyyyy"},
		{"yyyyyxxxyxxxxy","yyyyy"},
		{"yxxx,xxxxy","xxx,"},
		{"yxxxyxxxxy","xxxx"},
		{"yxxxyxxxxy","xxx"},
		{"yxxxyxxy","xxx"},
		{"xxx","xxx"},
		{"xxx","x"},
		{"xxx","yyy"},
		{",","x,x"},
		{"x","xxx"},
		{"xxx",""},
		{"","xxx"},
		{"",""},
		{" he11o wor1d","he11o wor1d"},
		{"he11o wor1","he11o wor1d"},
		{"he11o wor1d","he11o wor1d"},
		{"he11o wor1d","he11o wor1"},
		{"he11o wor1d","he11o wor"},
		{"he11o wor1d","he11o wo"},
		{"he11o wor1d","he11o w"},
		{"he11o wor1d","he11o "},
		{"he11o wor1d","he11o"},
		{"he11o wor1d","wor1d"},
		{"he11o wor1d","he11o "},
		{"he11o wor1d"," he11o"},
		{"he11o wor1d","  he11o"},
		{"he11o wor1d","o w"},
		{"he11o wor1d","he1"},
		{"he11o","he11o"},
		{" he11o","he11o "},
		{" he11o","he11o"},
		{"he11o ","he11o"},
		{" he11o ","he11o"},
		{"  he11o ","he11o"},
		{"he11o  ","he11o"},
		{"he11o  ","he11o     "},
		{"yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy","yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"},
		{"xxyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy","yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"},
		{"xxyyyy,yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy","yyyyyyyyyyyyyyyyy,yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"},
		{"xxyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyxx","yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"},

		/* Some of these haystacks and needles were taken from musl's libc-test.

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

#define y256 "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"
#define NE(hs, ne) \
		{hs, ne}, \
		{y256 hs, y256 ne}, \
		{hs y256, ne y256}, \

		NE("","a")
		NE("a","a")
		NE("a","b")
		NE("aa","b")
		NE("aa","a")
		NE("aba","b")
		NE("abba","b")
		NE("abba","ba")
		NE("abc abc","d")
		NE("0-1-2-3-a-5-6-7-8-9","")
		NE("0-1-2-3-a-5-6-7-8-9","")
		NE("_ _ _\xff_ _ _","\x7f_")
		NE("_ _ _\x7f_ _ _","\xff_")
		NE("","")
		NE("abcd","")
		NE("abcd","a")
		NE("abcd","b")
		NE("abcd","c")
		NE("abcd","d")
		NE("abcd","ab")
		NE("abcd","bc")
		NE("abcd","cd")
		NE("ababa","baba")
		NE("ababab","babab")
		NE("abababa","bababa")
		NE("abababab","bababab")
		NE("ababababa","babababa")
		NE("abbababab","bababa")
		NE("abbababab","ababab")
		NE("abacabcabcab","abcabcab")
		NE("nanabanabanana","aba")
		NE("nanabanabanana","ban")
		NE("nanabanabanana","anab")
		NE("nanabanabanana","banana")
		NE("_ _\xff_ _","_\xff_")

		NE("","4")
		NE("4","4")
		NE("4","b")
		NE("44","b")
		NE("44","4")
		NE("4b4","b")
		NE("4bb4","b")
		NE("4bb4","b4")
		NE("4bc 4bc","d")
		NE("0-1-2-3-4-5-6-7-8-9","")
		NE("0-1-2-3-4-5-6-7-8-9","")
		NE("_ _ _\xff_ _ _","\x7f_")
		NE("_ _ _\x7f_ _ _","\xff_")
		NE("","")
		NE("4bcd","")
		NE("4bcd","4")
		NE("4bcd","b")
		NE("4bcd","c")
		NE("4bcd","d")
		NE("4bcd","4b")
		NE("4bcd","bc")
		NE("4bcd","cd")
		NE("4b4b4","b4b4")
		NE("4b4b4b","b4b4b")
		NE("4b4b4b4","b4b4b4")
		NE("4b4b4b4b","b4b4b4b")
		NE("4b4b4b4b4","b4b4b4b4")
		NE("4bb4b4b4b","b4b4b4")
		NE("4bb4b4b4b","4b4b4b")
		NE("4b4c4bc4bc4b","4bc4bc4b")
		NE("n4n4b4n4b4n4n4","4b4")
		NE("n4n4b4n4b4n4n4","b4n")
		NE("n4n4b4n4b4n4n4","4n4b")
		NE("n4n4b4n4b4n4n4","b4n4n4")
		NE("_ _\xff_ _","_\xff_")

		{"end", "EnD"}

  /* clang-format on */
	};
	T_STRSTR(jstr_strcasestr, simple_strcasestr);
	T_STRSTR_LEN(jstr_strcasestr_len, simple_strcasestr_len);
	T_STRSTR_LEN(jstr_memmem, simple_memmem);
	T_STRSTR_LEN(jstr_strrstr_len, simple_strrstr_len);
	T_STRNSTR(jstr_strnstr, simple_strnstr);
	SUCCESS();
	return EXIT_SUCCESS;
}
