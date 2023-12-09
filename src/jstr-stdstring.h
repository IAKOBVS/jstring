/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023 James Tirta Halim <tirtajames45 at gmail dot com>
   This file is part of the jstring library.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE. */

#ifndef JSTR_STDSTRING_H
#define JSTR_STDSTRING_H 1

#include "jstr-macros.h"
#include "jstr-ctype.h"
#include "jstr-struct.h"
#include "jstr-word-at-a-time.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#define R JSTR_RESTRICT
#define JSTR_LGPL_IMPL

PJSTR_BEGIN_DECLS

/* Compare S1 with S2 case-insensitively.
   Return value:
   0 if strings match;
   non-zero otherwise. */
#if JSTR_HAVE_STRNCASECMP
JSTR_ATTR_INLINE
#endif
JSTR_FUNC_PURE
static int
jstr_strncasecmp(const char *s1,
                 const char *s2,
                 size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNCASECMP && !JSTR_TEST
	return strncasecmp(s1, s2, n);
#else
	if (jstr_unlikely(n == 0))
		return 0;
	int ret;
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	while (!(ret = jstr_tolower(*p1) - jstr_tolower(*p2++)) && *p1++ && n--) {}
	return ret;
#endif
}

/* Compare S1 with S2 case-insensitively.
   Return value:
   0 if strings match;
   non-zero otherwise. */
#if JSTR_HAVE_STRNCASECMP
JSTR_ATTR_INLINE
#endif
JSTR_FUNC_PURE
static int
jstr_strncasecmpeq(const char *s1,
                   const char *s2,
                   size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNCASECMP && !JSTR_TEST
	return strncasecmp(s1, s2, n);
#else
	if (jstr_unlikely(n == 0))
		return 0;
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	for (; jstr_tolower(*p1) == jstr_tolower(*p2++) && *p1 && n; ++p1, --n) {}
	return *p1 && n;
#endif
}

/* Compare S1 with S2 case-insensitively.
   N must be lower than the length of S1 or S2.
   Return value:
   0 if strings match;
   non-zero otherwise. */
#if JSTR_HAVE_STRNCASECMP
JSTR_ATTR_INLINE
#endif
JSTR_FUNC_PURE
static int
jstr_strcasecmp_len(const char *s1,
                    const char *s2,
                    size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNCASECMP && !JSTR_TEST
	return strncasecmp(s1, s2, n);
#else
	if (jstr_unlikely(n == 0))
		return 0;
	int ret;
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	while (!(ret = jstr_tolower(*p1++) - jstr_tolower(*p2++)) && n--) {}
	return ret;
#endif
}

/* Compare S1 with S2 case-insensitively.
   N must be lower than the length of S1 or S2.
   Return value:
   0 if strings match;
   non-zero otherwise. */
#if JSTR_HAVE_STRNCASECMP
JSTR_ATTR_INLINE
#endif
JSTR_FUNC_PURE
static int
jstr_strcasecmpeq_len(const char *s1,
                      const char *s2,
                      size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNCASECMP && !JSTR_TEST
	return strncasecmp(s1, s2, n);
#else
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	for (; n && jstr_tolower(*p1++) == jstr_tolower(*p2++); --n) {}
	return n;
#endif
}

/* Compare S1 with S2 case-insensitively.
   Return value:
   0 if strings match;
   non-zero otherwise. */
JSTR_FUNC_PURE
#if JSTR_HAVE_STRCASECMP
JSTR_ATTR_INLINE
#endif
static int
jstr_strcasecmp(const char *s1,
                const char *s2)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASECMP && !JSTR_TEST
	return strcasecmp(s1, s2);
#else
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	int ret;
	while (!(ret = jstr_tolower(*p1) - jstr_tolower(*p2++)) && *p1++) {}
	return ret;
#endif
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_strcasecmpeq_loop(const char *s1,
                       const char *s2)
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	for (; jstr_tolower(*p1) == jstr_tolower(*p2++) && *p1++;) {}
	return *(p2 - 1);
}

/* Compare S1 with S2 case-insensitively.
   Return value:
   0 if strings match;
   non-zero otherwise. */
JSTR_FUNC_PURE
#if JSTR_HAVE_STRCASECMP
JSTR_ATTR_INLINE
#endif
static int
jstr_strcasecmpeq(const char *s1,
                  const char *s2)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASECMP && !JSTR_TEST
	return strcasecmp(s1, s2);
#else
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	for (; jstr_tolower(*p1) == jstr_tolower(*p2++) && *p1++;) {}
	return *(p2 - 1);
#endif
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_strcmpeq_loop(const char *s1,
                   const char *s2)
{
	while ((*s1 == *s2++) && *s1++) {}
	return *(s2 - 1);
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_ATTR_ACCESS((__read_only__, 2, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_memcmpeq_loop(const void *s1,
                   const void *s2,
                   size_t n)
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	for (; n && (*p1++ == *p2++); --n) {}
	return n;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_strcasecmpeq_len_loop(const void *s1,
                           const void *s2,
                           size_t n)
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	for (; n && (jstr_tolower(*p1++) == jstr_tolower(*p2++)); --n) {}
	return n;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_strnlen(const char *s,
             size_t maxlen)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNLEN
	return strnlen(s, maxlen);
#else
	const char *const p = (char *)memchr(s, '\0', maxlen);
	return p ? (size_t)(p - s) : maxlen;
#endif
}

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_strnlen_loop(const char *s,
                  size_t n)
{
	const int l = s[0] + s[1] + s[2] + s[3];
	return l <= 4 ? (size_t)l : jstr_strnlen(s, n);
}

JSTR_ATTR_ACCESS((__read_write__, 1, 3))
JSTR_FUNC
JSTR_ATTR_INLINE
static void *
jstr_mempset(void *s,
             int c,
             size_t n)
{
	return (char *)memset(s, c, n) + n;
}

JSTR_ATTR_ACCESS((__read_write__, 1, 3))
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_stpset_len(char *s,
                int c,
                size_t n)
{
	return (char *)memset(s, c, n) + n;
}

JSTR_ATTR_ACCESS((__read_write__, 1, 2))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstr_stpset(char *s,
            int c)
JSTR_NOEXCEPT
{
	return jstr_stpset_len(s, c, strlen(s));
}

JSTR_ATTR_ACCESS((__read_write__, 1, 2))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_bzero(void *s,
           size_t n)
JSTR_NOEXCEPT
{
	memset(s, 0, n);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static int
jstr_bcmp(const void *s1,
          const void *s2,
          size_t n)
JSTR_NOEXCEPT
{
	return memcmp(s1, s2, n);
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_strzero(char *s)
JSTR_NOEXCEPT
{
	memset(s, 0, strlen(s));
}

/* Return pointer to '\0' in S. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstr_stpzero(char *s)
JSTR_NOEXCEPT
{
	const size_t sz = strlen(s);
	return (char *)memset(s, 0, sz) + sz;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
#if JSTR_HAVE_MEMRCHR || !JSTR_HAVE_ATTR_MAY_ALIAS
JSTR_ATTR_INLINE
#endif
static void *
jstr_memrchr(const void *s,
             int c,
             size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMRCHR
	return (void *)memrchr(s, c, n);
#else
	/* The following is based on musl's memchr().
	 * Copyright © 2005-2020 Rich Felker, et al.
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
	const unsigned char *p = (const unsigned char *)s + n - 1;
	c = (unsigned char)c;
#	if JSTR_HAVE_ATTR_MAY_ALIAS
#		define SS         (sizeof(size_t))
#		define ALIGN      (sizeof(size_t) - 1)
#		define ONES       ((size_t)-1 / UCHAR_MAX)
#		define HIGHS      (ONES * (UCHAR_MAX / 2 + 1))
#		define HASZERO(x) (((x)-ONES) & ~(x)&HIGHS)
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
#		undef SS
#		undef ALIGN
#		undef ONES
#		undef HIGHS
#		undef HASZERO
#	endif
	for (; n--; --p)
		if (*p == c)
			return (char *)p;
	return NULL;
#endif
}

#undef JSTR_HAVE_STRCHRNUL
#undef JSTR_HAVE_STRCHR_OPTIMIZED

JSTR_FUNC_PURE
#if JSTR_HAVE_STRCHRNUL || JSTR_HAVE_STRCHR_OPTIMIZED || !JSTR_HAVE_ATTR_MAY_ALIAS
JSTR_ATTR_INLINE
#endif
static char *
jstr_strchrnul(const char *s,
               int c)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL && !JSTR_TEST
	return (char *)strchrnul(s, c);
#elif JSTR_HAVE_STRCHR_OPTIMIZED && !JSTR_TEST
	/* Optimized strchr() + strlen() is still faster than a C strchrnul(). */
	const char *const start = s;
	s = strchr(s, c);
	return (char *)(s ? s : start + strlen(start));
#else
	/* The following is taken from musl's strchrnul() with minor modifications.
	 * Copyright © 2005-2020 Rich Felker, et al.
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
	if (jstr_unlikely(c == '\0'))
		return (char *)s + strlen(s);
#	if JSTR_HAVE_ATTR_MAY_ALIAS
#		define ALIGN      (sizeof(size_t))
#		define ONES       ((size_t)-1 / UCHAR_MAX)
#		define HIGHS      (ONES * (UCHAR_MAX / 2 + 1))
#		define HASZERO(x) (((x)-ONES) & ~(x)&HIGHS)
	typedef size_t JSTR_ATTR_MAY_ALIAS word;
	for (; (uintptr_t)s % ALIGN; ++s)
		if (*s == '\0' || *s == (char)c)
			return (char *)s;
	const size_t k = ONES * (unsigned char)c;
	const word *w = w = (word *)s;
	for (; !HASZERO(*w) && !HASZERO(*w ^ k); ++w) {}
	s = (char *)w;
#		undef ALIGN
#		undef ONES
#		undef HIGHS
#		undef HASZERO
#	endif
	for (; *s && *s != (char)c; ++s) {}
	return (char *)s;
#endif
}

JSTR_FUNC_PURE
#if !JSTR_HAVE_ATTR_MAY_ALIAS
JSTR_ATTR_INLINE
#endif
static char *
pjstr_strcasechrnul_word(const char *s,
                         int c)
JSTR_NOEXCEPT
{
	/* The following is based on musl's strchrnul().
	 * Copyright © 2005-2020 Rich Felker, et al.
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
	c = jstr_tolower(c);
#if JSTR_HAVE_ATTR_MAY_ALIAS
#	define ALIGN      (sizeof(size_t))
#	define ONES       ((size_t)-1 / UCHAR_MAX)
#	define HIGHS      (ONES * (UCHAR_MAX / 2 + 1))
#	define HASZERO(x) (((x)-ONES) & ~(x)&HIGHS)
	typedef size_t JSTR_ATTR_MAY_ALIAS word;
	for (; (uintptr_t)s % ALIGN; ++s)
		if (jstr_unlikely(*s == '\0') || jstr_tolower(*s) == c)
			return (char *)s;
	const size_t k = ONES * (unsigned char)c;
	const size_t l = ONES * (unsigned char)jstr_toupper(c);
	const word *w = w = (word *)s;
	for (; !HASZERO(*w) && !HASZERO(*w ^ k) && !HASZERO(*w ^ l); ++w) {}
	s = (char *)w;
#	undef ALIGN
#	undef ONES
#	undef HIGHS
#	undef HASZERO
#endif
	for (; *s && jstr_tolower(*s) != c; ++s) {}
	return (char *)s;
}

JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static char *
pjstr_strcasechr_word(const char *s,
                      int c)
JSTR_NOEXCEPT
{
	s = pjstr_strcasechrnul_word(s, c);
	return *s == (char)c ? (char *)s : NULL;
}

JSTR_FUNC_PURE
#if !JSTR_HAVE_ATTR_MAY_ALIAS
JSTR_ATTR_INLINE
#endif
static void *
jstr_memcasechr(const void *s,
                int c,
                size_t n)
JSTR_NOEXCEPT
{
	/* The following is based on musl's strchrnul().
	 * Copyright © 2005-2020 Rich Felker, et al.
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
	if (jstr_unlikely(c == '\0'))
		return (char *)s + jstr_strnlen((char *)s, n);
	const unsigned char *p = (const unsigned char *)s;
	c = jstr_tolower(c);
#if JSTR_HAVE_ATTR_MAY_ALIAS
#	define ALIGN      (sizeof(size_t))
#	define ONES       ((size_t)-1 / UCHAR_MAX)
#	define HIGHS      (ONES * (UCHAR_MAX / 2 + 1))
#	define HASZERO(x) (((x)-ONES) & ~(x)&HIGHS)
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
#	undef ALIGN
#	undef ONES
#	undef HIGHS
#	undef HASZERO
#endif
	for (; n && jstr_tolower(*p) != c; --n, ++p) {}
	return n ? (void *)p : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strcasechrnul(const char *s,
                    int c)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCSPN_OPTIMIZED
	const char a[] = { (char)jstr_tolower(c), (char)jstr_toupper(c), '\0' };
	s += strcspn(s, a);
	return (char *)s;
#else
	return pjstr_strcasechrnul_word(s, c);
#endif
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strcasechr(const char *s,
                 int c)
JSTR_NOEXCEPT
{
	c = jstr_tolower(c);
	if (jstr_tolower(*s) == c)
		return (char *)s;
	s = pjstr_strcasechrnul(s, c);
	return jstr_tolower(*s) == c ? (char *)s : NULL;
}

/* Return value:
   ptr to first C in S ignoring case;
   NULL if not found. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strcasechr(const char *s,
                int c)
JSTR_NOEXCEPT
{
	if (jstr_isalpha(c))
		return pjstr_strcasechr(s, c);
	return (char *)strchr(s, c);
}

/* Return value:
   ptr to first C in S ignoring case;
   NULL if not found. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strcasechrnul(const char *s,
                   int c)
JSTR_NOEXCEPT
{
	if (jstr_isalpha(c))
		return pjstr_strcasechrnul(s, c);
	if (jstr_likely(c))
		return jstr_strchrnul(s, c);
	return (char *)s + strlen(s);
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memnchr(const void *s,
             int c,
             size_t sz,
             size_t n)
JSTR_NOEXCEPT
{
	return (void *)memchr(s, c, JSTR_MIN(n, sz));
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strnchr_len(const char *s,
                 int c,
                 size_t sz,
                 size_t n)
JSTR_NOEXCEPT
{
	return (char *)jstr_memnchr(s, c, sz, n);
}

/* strchr() before s + N. */
JSTR_FUNC_PURE
#if !JSTR_HAVE_ATTR_MAY_ALIAS
JSTR_ATTR_INLINE
#endif
static char *
jstr_strnchr(const char *s,
             int c,
             size_t n)
JSTR_NOEXCEPT
{
	/* The following is based on musl's strchr() and memchr().
	 * Copyright © 2005-2020 Rich Felker, et al.
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
#if JSTR_HAVE_ATTR_MAY_ALIAS
#	define SS         (sizeof(size_t))
#	define ALIGN      (sizeof(size_t) - 1)
#	define ONES       ((size_t)-1 / UCHAR_MAX)
#	define HIGHS      (ONES * (UCHAR_MAX / 2 + 1))
#	define HASZERO(x) (((x)-ONES) & ~(x)&HIGHS)
	for (; (uintptr_t)s & ALIGN; ++s) {
		if (*s == '\0' || jstr_unlikely(n-- == 0))
			return NULL;
		if (*s == (char)c)
			return (char *)s;
	}
	if (n && *s != (char)c) {
		typedef size_t JSTR_ATTR_MAY_ALIAS word;
		const size_t k = ONES * (unsigned char)c;
		const word *w = w = (const word *)s;
		for (; n >= SS && !HASZERO(*w) && !HASZERO(*w ^ k); ++w, n -= SS) {}
		s = (const char *)w;
	}
#	undef SS
#	undef ALIGN
#	undef ONES
#	undef HIGHS
#	undef HASZERO
#endif
	for (; n && *s && *s != (char)c; ++s, --n) {}
	return n ? (char *)s : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memchrnul(const void *s,
               int c,
               size_t sz)
JSTR_NOEXCEPT
{
	const void *const p = jstr_memrchr(s, c, sz);
	return (void *)(p ? p : (char *)s + sz);
}

/* Return value:
   ptr to '\0' in DST. */
JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static void *
jstr_mempcpy(void *R dst,
             const void *R src,
             size_t sz)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMPCPY
	return mempcpy(dst, src, sz);
#else
	return (char *)memcpy(dst, src, sz) + sz;
#endif
}

JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC
JSTR_ATTR_INLINE
static void *
jstr_mempmove(void *dst,
              const void *src,
              size_t n)
JSTR_NOEXCEPT
{
	return (char *)memmove(dst, src, n) + n;
}

JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_strmove_len(char *dst,
                 const char *src,
                 size_t n)
JSTR_NOEXCEPT
{
	*(char *)jstr_mempmove(dst, src, n) = '\0';
}

JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_stpmove_len(char *dst,
                 const char *src,
                 size_t n)
JSTR_NOEXCEPT
{
	jstr_strmove_len(dst, src, n);
	return (char *)dst + n;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_stpmove(char *dst,
             const char *src)
JSTR_NOEXCEPT
{
	return jstr_stpmove_len(dst, src, strlen((const char *)src));
}

JSTR_ATTR_INLINE
JSTR_FUNC_VOID
static void
jstr_strcpy_len(char *R dst,
                const char *R src,
                size_t n)
JSTR_NOEXCEPT
{
	*(char *)jstr_mempcpy(dst, src, n) = '\0';
}

JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_ATTR_INLINE
JSTR_FUNC_VOID
static char *
jstr_stpcpy_len(char *R dst,
                const char *R src,
                size_t n)
JSTR_NOEXCEPT
{
	jstr_strcpy_len(dst, src, n);
	return (char *)dst + n;
}

/* Return value:
   ptr to '\0' in DST. */
JSTR_FUNC_RET_NONNULL
#if JSTR_HAVE_STRCPY_OPTIMIZED && JSTR_HAVE_STRLEN_OPTIMIZED || !JSTR_HAVE_ATTR_MAY_ALIAS
JSTR_ATTR_INLINE
#endif
static char *
jstr_stpcpy(char *R dst,
            const char *R src)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STPCPY && !JSTR_TEST
	return stpcpy(dst, src);
#elif JSTR_HAVE_STRLEN_OPTIMIZED && !JSTR_TEST
	/* Optimized memcpy() + strlen() is still faster than a C stpcpy(). */
	return jstr_stpcpy_len(dst, src, strlen(src));
#elif JSTR_HAVE_WORD_AT_A_TIME && JSTR_USE_LGPL
#	include "_lgpl-stpcpy.h"
#else
#	if JSTR_HAVE_ATTR_MAY_ALIAS
	/* The following is taken from musl's stpcpy() with minor modifications.
	 * Copyright © 2005-2020 Rich Felker, et al.
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
#		define ALIGN      (sizeof(size_t))
#		define ONES       ((size_t)-1 / UCHAR_MAX)
#		define HIGHS      (ONES * (UCHAR_MAX / 2 + 1))
#		define HASZERO(x) (((x)-ONES) & ~(x)&HIGHS)
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
#		undef ALIGN
#		undef ONES
#		undef HIGHS
#		undef HASZERO
#	endif
	while ((*dst++ = *src++)) {}
	return dst - 1;
#endif
}

/* Copy until either N is 0 or C is found */
JSTR_ATTR_ACCESS((__write_only__, 1, 4))
JSTR_FUNC
JSTR_ATTR_INLINE
static void *
jstr_memccpy(void *R dst,
             const void *R src,
             int c,
             size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMCCPY
	return memccpy(dst, src, c, n);
#else
	const char *const p = (char *)memchr(src, c, n);
	if (p != NULL)
		return jstr_stpcpy_len(dst, src, JSTR_PTR_DIFF(p, src));
	memcpy(dst, src, n);
	return NULL;
#endif /* HAVE_MEMCPY */
}

/* Return value:
   Pointer to '\0' in DST. */
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_stpcat(char *R dst,
            const char *R src)
JSTR_NOEXCEPT
{
	dst += strlen(dst);
	return jstr_stpcpy(dst, src);
}

JSTR_ATTR_MALLOC
JSTR_FUNC
JSTR_ATTR_INLINE
static void *
jstr_memdup(const void *R s,
            size_t n)
JSTR_NOEXCEPT
{
	void *p = malloc(n);
	return (jstr_likely(p != NULL)) ? memcpy(p, s, n) : NULL;
}

JSTR_ATTR_MALLOC
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_strdup_len(const char *R s,
                size_t n)
JSTR_NOEXCEPT
{
	char *const p = (char *)malloc(n + 1);
	if (jstr_likely(p != NULL))
		return jstr_stpcpy_len(p, s, n);
	return NULL;
}

JSTR_ATTR_MALLOC
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_strdup(const char *R s)
JSTR_NOEXCEPT
{
	return jstr_strdup_len(s, strlen(s));
}

#define PJSTR_DEFINE_ATOI(T, name, func) \
	JSTR_FUNC_PURE                   \
	JSTR_ATTR_INLINE                 \
	static T                         \
	jstr_##name(const char *s)       \
	JSTR_NOEXCEPT                    \
	{                                \
		return func;             \
	}

PJSTR_DEFINE_ATOI(unsigned int, atou, strtoul(s, NULL, 0))
PJSTR_DEFINE_ATOI(unsigned long, atoul, strtoul(s, NULL, 0))
PJSTR_DEFINE_ATOI(unsigned long long, atoull, strtoull(s, NULL, 0))
PJSTR_DEFINE_ATOI(int, atoi, strtol(s, NULL, 0))
PJSTR_DEFINE_ATOI(long, atol, strtol(s, NULL, 0))
PJSTR_DEFINE_ATOI(long long, atoll, strtol(s, NULL, 0))
PJSTR_DEFINE_ATOI(double, atod, strtod(s, NULL))
PJSTR_DEFINE_ATOI(float, atof, strtof(s, NULL))

#undef PJSTR_DEFINE_ATOI

PJSTR_END_DECLS

#undef R

#endif /* JSTR_STDSTRING_H */
