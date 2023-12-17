/* Taken from or based on musl's string functions with modifications.
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

PJSTR_BEGIN_DECLS
#include <limits.h>
PJSTR_END_DECLS

#include "jstr-ctype.h"

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
#if JSTR_HAVE_ATTR_MAY_ALIAS
	enum { ALIGN = sizeof(size_t) };
	typedef size_t JSTR_ATTR_MAY_ALIAS word;
	for (; (uintptr_t)s % ALIGN; ++s)
		if (jstr_unlikely(*s == '\0') || *s == (char)c)
			return (char *)s;
	const size_t k = ONES * (unsigned char)c;
	const word *ws = (word *)s;
	for (word w = *ws; !HASZERO(w) && !HASZERO(w ^ k); w = *++ws) {}
	s = (char *)ws;
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
		const word *ws = (const word *)(p - SS + 1);
		const size_t k = ONES * (unsigned char)c;
		for (word w = *ws; n >= SS && !HASZERO(w ^ k); w = *--ws, n -= SS) {}
		p = (unsigned char *)ws + SS - 1;
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
	const word *ws = (word *)s;
	for (word w = *ws; !HASZERO(w) && !HASZERO(w ^ k) && !HASZERO(w ^ l); w = *++ws) {}
	s = (char *)ws;
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
	if (!jstr_isalpha(c))
		return (void *)memchr(s, c, n);
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
		const word *ws = (word *)p;
		for (word w = *ws; n >= sizeof(size_t) && !HASZERO(w ^ k) && !HASZERO(w ^ l); n -= sizeof(size_t), w = *++ws) {}
		p = (unsigned char *)ws;
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
	if ((uintptr_t)dst % ALIGN == (uintptr_t)src % ALIGN) {
		while (JSTR_PTR_IS_NOT_ALIGNED(dst, ALIGN))
			if (jstr_unlikely((*dst++ = *src++) == '\0'))
				return dst - 1;
		word *wd = (word *)dst;
		const word *ws = (const word *)src;
		for (word w = *ws; !HASZERO(w); w = *++ws)
			*wd++ = w;
		dst = (char *)wd;
		src = (const char *)ws;
	}
#endif
	while ((*dst++ = *src++)) {}
	return dst - 1;
}

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC_PURE
static char *
pjstr_strnchr_musl(const char *s,
                   int c,
                   size_t n)
JSTR_NOEXCEPT
{
	enum { SS = sizeof(size_t),
	       ALIGN = (sizeof(size_t) - 1) };
#if JSTR_HAVE_ATTR_MAY_ALIAS
	for (; (uintptr_t)s & ALIGN; ++s) {
		if (jstr_unlikely(*s == '\0') || jstr_unlikely(n-- == 0))
			return NULL;
		if (*s == (char)c)
			return (char *)s;
	}
	if (n >= SS && *s != (char)c) {
		typedef size_t JSTR_ATTR_MAY_ALIAS word;
		const size_t k = ONES * (unsigned char)c;
		const word *ws = (const word *)s;
		for (word w = *ws; n >= SS && !HASZERO(w) && !HASZERO(w ^ k); w = *++ws, n -= SS) {}
		s = (const char *)ws;
	}
#endif
	for (; n && *s && *s != (char)c; ++s, --n) {}
	return n ? (char *)s : NULL;
}

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC_PURE
static char *
pjstr_strncasechr_musl(const char *s,
                       int c,
                       size_t n)
JSTR_NOEXCEPT
{
	enum { SS = sizeof(size_t),
	       ALIGN = (sizeof(size_t) - 1) };
	if (!jstr_isalpha(c))
		return pjstr_strnchr_musl(s, c, n);
	c = jstr_tolower(c);
#if JSTR_HAVE_ATTR_MAY_ALIAS
	for (; (uintptr_t)s & ALIGN; ++s) {
		if (jstr_unlikely(*s == '\0') || jstr_unlikely(n-- == 0))
			return NULL;
		if (jstr_tolower(*s) == c)
			return (char *)s;
	}
	if (n >= SS && jstr_tolower(*s) != c) {
		typedef size_t JSTR_ATTR_MAY_ALIAS word;
		const size_t k = ONES * (unsigned char)c;
		const size_t l = ONES * jstr_toupper(c);
		const word *ws = (const word *)s;
		for (word w = *ws; n >= SS && !HASZERO(w) && !HASZERO(w ^ k) && !HASZERO(w ^ l); w = *++ws, n -= SS) {}
		s = (const char *)ws;
	}
#endif
	for (; n && *s && jstr_tolower(*s) != c; ++s, --n) {}
	return n ? (char *)s : NULL;
}

#undef ONES
#undef HIGHS
#undef HASZERO

#endif /* PJSTR_MUSL_H */
