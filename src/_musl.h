/* Taken from or based on musl's str*, mem* functions with modifications.
   Copyright © 2005-2020 Rich Felker, et al.

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

#ifndef PJSTR_MUSL_H
#define PJSTR_MUSL_H

#include "jstr-macros.h"
#include "jstr-ctype.h"
#include <limits.h>

#define ONES       ((size_t)-1 / UCHAR_MAX)
#define HIGHS      (ONES * (UCHAR_MAX / 2 + 1))
#define HASZERO(x) (((x)-ONES) & ~(x)&HIGHS)

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC_PURE
static char *
pjstr_strchrnul_musl(const char *s,
                     int c)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(c == '\0'))
		return (char *)s + strlen(s);
#if JSTR_HAVE_ATTR_MAY_ALIAS
	enum { ALIGN = sizeof(size_t) };
	typedef size_t JSTR_ATTR_MAY_ALIAS word;
	for (; (uintptr_t)s % ALIGN; ++s)
		if (jstr_unlikely(*s == '\0') || *s == (char)c)
			return (char *)s;
	const size_t k = ONES * (unsigned char)c;
	const word *w = w = (word *)s;
	for (; !HASZERO(*w) && !HASZERO(*w ^ k); ++w) {}
	s = (char *)w;
#endif
	for (; *s && *s != (char)c; ++s) {}
	return (char *)s;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static void *
pjstr_memrchr_musl(const void *s,
                   int c,
                   size_t n)
JSTR_NOEXCEPT
{
	enum { SS = sizeof(size_t),
	       ALIGN = (sizeof(size_t) - 1) };
	const unsigned char *p = (const unsigned char *)s + n - 1;
	c = (unsigned char)c;
#if JSTR_HAVE_ATTR_MAY_ALIAS
	for (; (uintptr_t)(p + 1) & ALIGN; --p) {
		if (jstr_unlikely(n-- == 0))
			return NULL;
		if (*p == c)
			return (char *)p;
	}
	if (n >= SS && *p != c) {
		typedef size_t JSTR_ATTR_MAY_ALIAS word;
		const word *w = (const word *)(p - SS + 1);
		const size_t k = ONES * (unsigned char)c;
		for (; n >= SS && !HASZERO(*w ^ k); --w, n -= SS) {}
		p = (unsigned char *)w + SS - 1;
	}
#endif
	for (; n--; --p)
		if (*p == c)
			return (char *)p;
	return NULL;
}

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC_PURE
static char *
pjstr_strcasechrnul_musl(const char *s,
                         int c)
JSTR_NOEXCEPT
{
	enum { ALIGN = sizeof(size_t) };
	c = jstr_tolower(c);
#if JSTR_HAVE_ATTR_MAY_ALIAS
	typedef size_t JSTR_ATTR_MAY_ALIAS word;
	for (; (uintptr_t)s % ALIGN; ++s)
		if (jstr_unlikely(*s == '\0') || jstr_tolower(*s) == c)
			return (char *)s;
	const size_t k = ONES * (unsigned char)c;
	const size_t l = ONES * (unsigned char)jstr_toupper(c);
	const word *w = w = (word *)s;
	for (; !HASZERO(*w) && !HASZERO(*w ^ k) && !HASZERO(*w ^ l); ++w) {}
	s = (char *)w;
#endif
	for (; *s && jstr_tolower(*s) != c; ++s) {}
	return (char *)s;
}

JSTR_FUNC_PURE
static void *
pjstr_memcasechr_musl(const void *s,
                      int c,
                      size_t n)
JSTR_NOEXCEPT
{
	enum { ALIGN = sizeof(size_t) };
	if (jstr_unlikely(c == '\0'))
#if JSTR_HAVE_STRNLEN
		return (char *)s + strnlen((char *)s, n);
#else
		return (void *)memchr(s, 0, n);
#endif
	const unsigned char *p = (const unsigned char *)s;
	c = jstr_tolower(c);
#if JSTR_HAVE_ATTR_MAY_ALIAS
	if (n >= sizeof(size_t) && jstr_tolower(*p) != c) {
		typedef size_t JSTR_ATTR_MAY_ALIAS word;
		for (; (uintptr_t)p % ALIGN; ++p) {
			if (jstr_unlikely(n-- == 0))
				return NULL;
			if (jstr_tolower(*p) == c)
				return (char *)p;
		}
		const size_t k = ONES * (unsigned char)c;
		const size_t l = ONES * jstr_toupper(c);
		const word *w = w = (word *)p;
		for (; n >= sizeof(size_t) && !HASZERO(*w ^ k) && !HASZERO(*w ^ l); n -= sizeof(size_t), ++w) {}
		p = (unsigned char *)w;
	}
#endif
	for (; n && jstr_tolower(*p) != c; --n, ++p) {}
	return n ? (void *)p : NULL;
}

/* Return value:
   ptr to '\0' in DST. */
JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC_RET_NONNULL
static char *
pjstr_stpcpy_musl(char *JSTR_RESTRICT dst,
                  const char *JSTR_RESTRICT src)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_ATTR_MAY_ALIAS
	enum { ALIGN = sizeof(size_t) };
	typedef size_t JSTR_ATTR_MAY_ALIAS word;
	if ((uintptr_t)src % ALIGN == (uintptr_t)dst % ALIGN) {
		for (; (uintptr_t)src % ALIGN; ++src, ++dst)
			if (!(*dst = *src))
				return dst;
		word *wd = (word *)dst;
		const word *ws = (const word *)src;
		for (; !HASZERO(*ws); *wd++ = *ws++) {}
		dst = (char *)wd;
		src = (const char *)ws;
	}
#endif
	while ((*dst++ = *src++)) {}
	return dst - 1;
}

#undef HASZERO
#undef HIGHS
#undef ONES

#endif /* PJSTR_MUSL_H */
