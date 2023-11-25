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
   SOFTWARE.

   MIT License (Expat) */

#ifndef JSTR_STRING_H
#define JSTR_STRING_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-rarebyte.h"
#include "jstr-stdstring.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

/* basename() for non nul-terminated strings. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_basename_len(const char *fname,
                  size_t sz)
JSTR_NOEXCEPT
{
	char *p = (char *)memchr(fname, '/', sz);
	return p ? p + 1 : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_basename(const char *fname)
JSTR_NOEXCEPT
{
	char *p = (char *)strchr(fname, '/');
	return p ? p + 1 : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strstr_len(const char *hs,
                size_t hs_len,
                const char *ne,
                size_t ne_len)
JSTR_NOEXCEPT
{
	return (char *)JSTR_MEMMEM(hs, hs_len, ne, ne_len);
	(void)hs_len;
	(void)ne_len;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strstr(const char *hs,
            const char *ne)
JSTR_NOEXCEPT
{
	return (char *)strstr(hs, ne);
}

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strnstr_len(const char *hs,
                 size_t hs_len,
                 const char *ne,
                 size_t ne_len,
                 size_t n)
JSTR_NOEXCEPT
{
	return jstr_strstr_len(hs, JSTR_MIN(hs_len, n), ne, ne_len);
}

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strstrnul_len(const char *hs,
                   size_t hs_len,
                   const char *ne,
                   size_t ne_len)
JSTR_NOEXCEPT
{
	char *p = jstr_strstr_len(hs, hs_len, ne, ne_len);
	return p ? p : (char *)hs + hs_len;
	(void)ne_len;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strstrnul(const char *hs,
               const char *ne)
JSTR_NOEXCEPT
{
	const char *const p = strstr(hs, ne);
	return (char *)(p ? p : hs + strlen(hs));
}

/*
  Compare S1 with S2 case-insensitively.
  Return value:
  0 if strings match;
  non-zero otherwise.
*/
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
#if JSTR_HAVE_STRNCASECMP
	return strncasecmp(s1, s2, n);
#else
	int ret;
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	if (jstr_unlikely(n == 0))
		return 0;
	while (!(ret = jstr_tolower(*p1) - jstr_tolower(*p2++))
	       && *p1++
	       && n--)
		;
	return ret;
#endif
}

/*
  Compare S1 with S2 case-insensitively.
  Return value:
  0 if strings match;
  non-zero otherwise.
*/
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
#if JSTR_HAVE_STRNCASECMP
	return strncasecmp(s1, s2, n);
#else
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	if (jstr_unlikely(n == 0))
		return 0;
	for (; jstr_tolower(*p1) == jstr_tolower(*p2++) && *p1 && n; ++p1, --n)
		;
	return *p1 && n;
#endif
}

/*
  Compare S1 with S2 case-insensitively.
  N must be lower than the length of S1 or S2.
  Return value:
  0 if strings match;
  non-zero otherwise.
*/
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
#if JSTR_HAVE_STRNCASECMP
	return strncasecmp(s1, s2, n);
#else
	int ret;
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	if (jstr_unlikely(n == 0))
		return 0;
	while (!(ret = jstr_tolower(*p1++) - jstr_tolower(*p2++))
	       && n--)
		;
	return ret;
#endif
}

/*
  Compare S1 with S2 case-insensitively.
  N must be lower than the length of S1 or S2.
  Return value:
  0 if strings match;
  non-zero otherwise.
*/
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
#if JSTR_HAVE_STRNCASECMP
	return strncasecmp(s1, s2, n);
#else
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	if (jstr_unlikely(n == 0))
		return 0;
	for (; jstr_tolower(*p1++) == jstr_tolower(*p2++) && n; --n)
		;
	return n;
#endif
}

/*
  Compare S1 with S2 case-insensitively.
  Return value:
  0 if strings match;
  non-zero otherwise.
*/
JSTR_FUNC_PURE
#if JSTR_HAVE_STRCASECMP
JSTR_ATTR_INLINE
#endif
static int
jstr_strcasecmp(const char *s1,
                const char *s2)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASECMP
	return strcasecmp(s1, s2);
#else
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	int ret;
	while (!(ret = jstr_tolower(*p1) - jstr_tolower(*p2++)) && *p1++)
		;
	return ret;
#endif
}

/*
  Compare S1 with S2 case-insensitively.
  Return value:
  0 if strings match;
  non-zero otherwise.
*/
JSTR_FUNC_PURE
#if JSTR_HAVE_STRCASECMP
JSTR_ATTR_INLINE
#endif
static int
jstr_strcasecmpeq(const char *s1,
                  const char *s2)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASECMP
	return strcasecmp(s1, s2);
#else
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	while (jstr_tolower(*p1) == jstr_tolower(*p2++) && *p1++)
		;
	return *(p2 - 1);
#endif
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_cmpeq(const void *s1,
           size_t s1_len,
           const void *s2,
           size_t s2_len)
JSTR_NOEXCEPT
{
	return (s1_len == s2_len) ? memcmp(s1, s2, s1_len) : 1;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_cmpcaseeq(const char *s1,
               size_t s1_len,
               const char *s2,
               size_t s2_len)
JSTR_NOEXCEPT
{
	return (s1_len == s2_len) ? jstr_strcasecmpeq(s1, s2) : 1;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strnstr2(const unsigned char *hs,
               const unsigned char *const ne,
               size_t l)
JSTR_NOEXCEPT
{
	const uint16_t nw = (uint16_t)ne[0] << 8 | ne[1];
	uint16_t hw = (uint16_t)hs[0] << 8 | hs[1];
	for (++hs, --l; l-- && *hs && hw != nw; hw = hw << 8 | *++hs)
		;
	return (hw == nw) ? (char *)hs - 1 : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strnstr3(const unsigned char *hs,
               const unsigned char *const ne,
               size_t l)
JSTR_NOEXCEPT
{
	const uint32_t nw = (uint32_t)ne[0] << 24 | ne[1] << 16 | ne[2] << 8;
	uint32_t hw = (uint32_t)hs[0] << 24 | hs[1] << 16 | hs[2] << 8;
	for (hs += 2, l -= 2; l-- && *hs && hw != nw; hw = (hw | *++hs) << 8)
		;
	return (hw == nw) ? (char *)hs - 2 : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strnstr4(const unsigned char *hs,
               const unsigned char *const ne,
               size_t l)
JSTR_NOEXCEPT
{
	const uint32_t nw = (uint32_t)ne[0] << 24 | ne[1] << 16 | ne[2] << 8 | ne[3];
	uint32_t hw = (uint32_t)hs[0] << 24 | hs[1] << 16 | hs[2] << 8 | hs[3];
	for (hs += 3, l -= 3; l-- && *hs && hw != nw; hw = hw << 8 | *++hs)
		;
	return (hw == nw) ? (char *)hs - 3 : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strnstr5plus(const unsigned char *hs,
                   const unsigned char *const ne,
                   size_t l,
                   size_t ne_len)
JSTR_NOEXCEPT
{
	const uint32_t nw = (uint32_t)ne[0] << 24 | ne[1] << 16 | ne[2] << 8 | ne[3];
	uint32_t hw = (uint32_t)hs[0] << 24 | hs[1] << 16 | hs[2] << 8 | hs[3];
	for (hs += 3, l -= 3; l-- && *hs; hw = hw << 8 | *++hs)
		if (hw == nw && !memcmp(hs - 3 + 4, ne + 4, ne_len - 4))
			goto ret;
	if (*hs && hw == nw && !memcmp(hs - 3 + 4, ne + 4, ne_len - 4))
ret:
		return (char *)hs - 3;
	return NULL;
}

#if !JSTR_HAVE_MEMMEM

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
pjstr_memmem2(const unsigned char *hs,
              const unsigned char *const ne,
              size_t l)
JSTR_NOEXCEPT
{
	const uint16_t nw = (uint16_t)ne[0] << 8 | ne[1];
	uint16_t hw = (uint16_t)hs[0] << 8 | hs[1];
	for (++hs, --l; l-- && hw != nw; hw = hw << 8 | *++hs)
		;
	return (hw == nw) ? (void *)(hs - 1) : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
pjstr_memmem3(const unsigned char *hs,
              const unsigned char *const ne,
              size_t l)
JSTR_NOEXCEPT
{
	const uint32_t nw = (uint32_t)ne[0] << 24 | ne[1] << 16 | ne[2] << 8;
	uint32_t hw = (uint32_t)hs[0] << 24 | hs[1] << 16 | hs[2] << 8;
	for (hs += 2, l -= 2; l-- && hw != nw; hw = (hw | *++hs) << 8)
		;
	return (hw == nw) ? (void *)(hs - 2) : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
pjstr_memmem4(const unsigned char *hs,
              const unsigned char *const ne,
              size_t l)
JSTR_NOEXCEPT
{
	const uint32_t nw = (uint32_t)ne[0] << 24 | ne[1] << 16 | ne[2] << 8 | ne[3];
	uint32_t hw = (uint32_t)hs[0] << 24 | hs[1] << 16 | hs[2] << 8 | hs[3];
	for (hs += 3, l -= 3; l-- && hw != nw; hw = hw << 8 | *++hs)
		;
	return (hw == nw) ? (void *)(hs - 3) : NULL;
}

#	if !JSTR_USE_LGPL

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
pjstr_memmem5plus(const unsigned char *hs,
                  size_t hsl,
                  const unsigned char *const ne,
                  size_t nel)
JSTR_NOEXCEPT
{
	const uint32_t nw = (uint32_t)ne[0] << 24 | ne[1] << 16 | ne[2] << 8 | ne[3];
	uint32_t hw = (uint32_t)hs[0] << 24 | hs[1] << 16 | hs[2] << 8 | hs[3];
	for (hs += 3, hsl -= 3; hsl--; hw = hw << 8 | *++hs)
		if (hw == nw && !memcmp(hs - 3 + 4, ne + 4, nel - 4))
			goto ret;
	if (hw == nw && !memcmp(hs - 3 + 4, ne + 4, nel - 4))
ret:
		return (void *)(hs - 3);
	return NULL;
}

#	endif

#endif

#if !JSTR_HAVE_MEMMEM
#	if JSTR_USE_LGPL
#		define PJSTR_MEMMEM_FUNC pjstr_memmem
#		include "_lgpl-memmem.h"
#	else
#		define PJSTR_RAREBYTE_RETTYPE void *
#		define PJSTR_RAREBYTE_FUNC    pjstr_memmem
#		include "_jstr-rarebyte-memmem.h"
#	endif
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
static void *
jstr_memmem(const void *hs,
            size_t hs_len,
            const void *ne,
            size_t ne_len)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMMEM
	return memmem(hs, hs_len, ne, ne_len);
#else
	typedef unsigned char u;
#	if JSTR_USE_LGPL
	if (ne_len == 1)
		return (void *)memchr(hs, *(const u *)ne, hs_len);
	if (jstr_unlikely(ne_len == 0))
		return (void *)hs;
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
#	else
	if (jstr_unlikely(ne_len == 0))
		return (void *)hs;
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	const void *const start = (const void *)hs;
	hs = (void *)memchr(hs, *(const u *)ne, hs_len);
	if (hs == NULL || ne_len == 1)
		return (void *)hs;
	hs_len -= hs - start;
	if (hs_len < ne_len)
		return NULL;
#	endif
	if (ne_len == 2)
		return pjstr_memmem2((const u *)hs, (const u *)ne, hs_len);
	if (ne_len == 3)
		return pjstr_memmem3((const u *)hs, (const u *)ne, hs_len);
	if (ne_len == 4)
		return pjstr_memmem4((const u *)hs, (const u *)ne, hs_len);
#	if JSTR_USE_LGPL
	return pjstr_memmem((const u *)hs, hs_len, (const u *)ne, ne_len);
#	else
	const u *const p = (const u *)jstr_rarebyteget_len(ne, ne_len);
	if (p)
		return pjstr_memmem((const u *)hs, hs_len, (const u *)ne, ne_len, p);
	return pjstr_memmem5plus((const u *)hs, hs_len, (const u *)ne, ne_len);
#	endif
#endif
}

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memnmem(const void *hs,
             size_t hs_len,
             const void *ne,
             size_t ne_len,
             size_t n)
JSTR_NOEXCEPT
{
	return jstr_memmem(hs, JSTR_MIN(hs_len, n), ne, ne_len);
}

#if JSTR_USE_LGPL
#	define PJSTR_MEMMEM_FUNC    pjstr_strnstr
#	define PJSTR_MEMMEM_RETTYPE char *
#	include "_lgpl-memmem.h"
#else

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strnstr(const unsigned char *hs,
              const unsigned char *ne,
              const unsigned char *rarebyte,
              size_t n)
{
	typedef unsigned char u;
	const unsigned char *hp, *np;
	const unsigned char *const end = hs + n;
	const int c = *(u *)rarebyte;
	const size_t idx = JSTR_PTR_DIFF(rarebyte, ne);
	hs += idx;
	for (hp = hs - idx, np = ne - idx;
	     (hs = (const u *)jstr_strnchr((char *)hs, c, end - hs));
	     ++hs) {
		for (; *hp == *np && *hp && hp < end; ++hp, ++np)
			;
		if (*np == '\0')
			return (char *)hs - idx;
		if (jstr_unlikely(*hp == '\0'))
			return NULL;
	}
	return NULL;
}

#endif
JSTR_FUNC_PURE
static char *
jstr_strnstr(const char *hs,
             const char *const ne,
             size_t n)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	if (jstr_unlikely(*ne == '\0'))
		return (char *)hs;
	const char *const start = hs;
	hs = jstr_strnchr(hs, *ne, n);
	n -= hs - start;
	if (hs == NULL || ne[1] == '\0')
		return (char *)hs;
	if (jstr_unlikely(hs[1] == '\0'))
		return NULL;
	if (ne[2] == '\0')
		return pjstr_strnstr2((const u *)hs, (const u *)ne, n);
	if (jstr_unlikely(hs[2] == '\0'))
		return NULL;
	if (ne[3] == '\0')
		return pjstr_strnstr3((const u *)hs, (const u *)ne, n);
	if (jstr_unlikely(hs[3] == '\0'))
		return NULL;
	if (ne[4] == '\0')
		return pjstr_strnstr4((const u *)hs, (const u *)ne, n);
#if JSTR_USE_LGPL
	size_t ne_len = strlen(ne);
	if (jstr_unlikely(n < ne_len))
		return NULL;
	size_t hs_len = jstr_strnlen(hs, ne_len);
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	if (!memcmp(hs, ne, ne_len))
		return (char *)hs;
	if (hs_len == ne_len)
		return NULL;
	++hs;
	--hs_len;
	hs_len += jstr_strnlen(hs + hs_len, n - hs_len);
	return pjstr_strnstr(hs, hs_len, ne, ne_len);
#else
	const u *const p = (const u *)jstr_rarebyteget(ne);
	if (p)
		return pjstr_strnstr((const u *)hs, (const u *)ne, p, n);
	return pjstr_strnstr5plus((const u *)hs, (const u *)ne, n, strlen(ne));
#endif
}

/*
   Find last NE in HS.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
static char *
jstr_strrstr_len(const void *hs,
                 size_t hs_len,
                 const void *const ne,
                 size_t ne_len)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	if (jstr_unlikely(ne_len == 0))
		return (char *)hs + hs_len;
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	const unsigned char *h = (const u *)jstr_memrchr(hs, *((char *)ne + ne_len - 1), hs_len);
	if (h == NULL || ne_len == 1)
		return (char *)h;
	if (JSTR_PTR_DIFF(h + 1, (const u *)hs) < ne_len)
		return NULL;
	const unsigned char *const start = (const u *)hs;
	const unsigned char *const n = (const u *)ne;
	if (ne_len == 2) {
		const uint16_t nw = (uint16_t)n[1] << 8 | n[0];
		uint16_t hw = (uint16_t)h[0] << 8 | h[-1];
		for (--h; h >= start && hw != nw; hw = hw << 8 | *--h)
			;
		if (hw == nw)
			return (char *)h;
	} else if (ne_len == 3) {
		const uint32_t nw = (uint32_t)n[2] << 24 | n[1] << 16 | n[0] << 8;
		uint32_t hw = (uint32_t)h[0] << 24 | h[-1] << 16 | h[-2] << 8;
		for (h -= 2; h >= start && hw != nw; hw = (hw | *--h) << 8)
			;
		if (hw == nw)
			return (char *)h;
	} else if (ne_len == 4) {
		const uint32_t nw = (uint32_t)n[3] << 24 | n[2] << 16 | n[1] << 8 | n[0];
		uint32_t hw = (uint32_t)h[0] << 24 | h[-1] << 16 | h[-2] << 8 | h[-3];
		for (h -= 3; h >= start && hw != nw; hw = hw << 8 | *--h)
			;
		if (hw == nw)
			return (char *)h;
	} else {
		uint32_t nw;
		uint32_t hw;
		h -= ne_len - 4;
		nw = (uint32_t)n[3] << 24 | n[2] << 16 | n[1] << 8 | n[0];
		hw = (uint32_t)h[0] << 24 | h[-1] << 16 | h[-2] << 8 | h[-3];
		for (h -= 3; h >= start; hw = hw << 8 | *--h)
			if (hw == nw && !memcmp(h, n, ne_len))
				return (char *)h;
	}
	return NULL;
}

/*
   Find last NE in HS.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
static void *
jstr_memrmem(const void *hs,
             size_t hs_len,
             const void *ne,
             size_t ne_len)
JSTR_NOEXCEPT
{
	return jstr_strrstr_len(hs, hs_len, ne, ne_len);
}

#if !JSTR_HAVE_STRCASESTR_OPTIMIZED

#	define L(c) jstr_tolower(c)

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strcasestr2(const unsigned char *hs,
                  const unsigned char *const ne)
JSTR_NOEXCEPT
{
	const uint16_t nw = (uint32_t)L(ne[0]) << 8 | L(ne[1]);
	uint16_t hw = (uint32_t)L(hs[0]) << 8 | L(hs[1]);
	for (++hs; *hs && hw != nw; hw = hw << 8 | L(*++hs))
		;
	return (hw == nw) ? (char *)hs - 1 : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strcasestr3(const unsigned char *hs,
                  const unsigned char *const ne)
JSTR_NOEXCEPT
{
	const uint32_t nw = (uint32_t)L(ne[0]) << 24 | L(ne[1]) << 16 | L(ne[2]) << 8;
	uint32_t hw = (uint32_t)L(hs[0]) << 24 | L(hs[1]) << 16 | L(hs[2]) << 8;
	for (hs += 2; *hs && hw != nw; hw = (hw | L(*++hs)) << 8)
		;
	return (hw == nw) ? (char *)hs - 2 : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strcasestr4(const unsigned char *hs,
                  const unsigned char *const ne)
JSTR_NOEXCEPT
{
	const uint32_t nw = (uint32_t)L(ne[0]) << 24 | L(ne[1]) << 16 | L(ne[2]) << 8 | L(ne[3]);
	uint32_t hw = (uint32_t)L(hs[0]) << 24 | L(hs[1]) << 16 | L(hs[2]) << 8 | L(hs[3]);
	for (hs += 3; *hs && hw != nw; hw = hw << 8 | L(*++hs))
		;
	return (hw == nw) ? (char *)hs - 3 : NULL;
}

#	if !JSTR_USE_LGPL

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strcasestr5plus(const unsigned char *hs,
                      const unsigned char *const ne)
JSTR_NOEXCEPT
{
	const unsigned char *hp;
	const unsigned char *np;
	const uint32_t nw = (uint32_t)L(ne[0]) << 24 | L(ne[1]) << 16 | L(ne[2]) << 8 | L(ne[3]);
	uint32_t hw = (uint32_t)L(hs[0]) << 24 | L(hs[1]) << 16 | L(hs[2]) << 8 | L(hs[3]);
	for (hs += 3; *hs; hw = hw << 8 | L(*++hs))
		if (hw == nw) {
			hp = hs - 3 + 4;
			np = ne + 4;
			for (; L(*hp) == L(*np) && *hp; ++hp, ++np)
				;
			if (*np == '\0')
				return (char *)hs - 3;
			if (jstr_unlikely(*hp == '\0'))
				return NULL;
		}
	return NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strcasestr5plus_len(const unsigned char *hs,
                          const unsigned char *const ne,
                          size_t ne_len)
JSTR_NOEXCEPT
{
	const uint32_t nw = (uint32_t)L(ne[0]) << 24 | L(ne[1]) << 16 | L(ne[2]) << 8 | L(ne[3]);
	uint32_t hw = (uint32_t)L(hs[0]) << 24 | L(hs[1]) << 16 | L(hs[2]) << 8 | L(hs[3]);
	for (hs += 3; *hs; hw = hw << 8 | L(*++hs))
		if (hw == nw && !jstr_strcasecmpeq_len((char *)hs - 3 + 4, (char *)ne + 4, ne_len - 4))
			return (char *)hs - 3;
	return NULL;
}

#	endif

#	if JSTR_USE_LGPL
#		define PJSTR_MEMMEM_FUNC        pjstr_strcasestr
#		define PJSTR_MEMMEM_RETTYPE     char *
#		define PJSTR_MEMMEM_CMP_FUNC    jstr_strcasecmpeq_len
#		define PJSTR_MEMMEM_HASH2_ICASE 1
#		define PJSTR_MEMMEM_CHECK_EOL   1
#		include "_lgpl-memmem.h"
#	else

JSTR_FUNC_PURE
static char *
pjstr_strcasestr(const unsigned char *hs,
                 const unsigned char *ne,
                 const unsigned char *rarebyte)
{
	typedef unsigned char u;
	const unsigned char *hp, *np;
	const int c = *(u *)rarebyte;
	const size_t idx = JSTR_PTR_DIFF(rarebyte, ne);
	hs += idx;
	for (hp = hs - idx, np = ne - idx;
	     (hs = (const u *)strchr((char *)hs, c));
	     ++hs) {
		for (; L(*hp) == L(*np) && *hp; ++hp, ++np)
			;
		if (*np == '\0')
			return (char *)hs - idx;
		if (jstr_unlikely(*hp == '\0'))
			return NULL;
	}
	return NULL;
}

#	endif

#	undef L

JSTR_FUNC_PURE
static char *
pjstr_strcasestr_bmh(const char *hs,
                     const char *ne)
JSTR_NOEXCEPT
{
#	if JSTR_USE_LGPL
	const size_t ne_len = strlen(ne);
	const size_t hs_len = jstr_strnlen(hs, ne_len | 512);
	if (hs_len < ne_len)
		return NULL;
	if (!jstr_strcasecmpeq_len(hs, ne, ne_len))
		return (char *)hs;
	if (hs_len == ne_len)
		return NULL;
	return pjstr_strcasestr(hs + 1, hs_len - 1, ne, ne_len);
#	else
	typedef unsigned char u;
	if (jstr_unlikely(*ne == '\0'))
		return (char *)hs;
	const u *const p = (const u *)jstr_rarebytegetcase(ne);
	if (p)
		return pjstr_strcasestr((const u *)hs, (const u *)ne, (const u *)p);
	return pjstr_strcasestr5plus((const u *)hs, (const u *)ne);
#	endif
}

#endif /* HAVE_STRCASESTR_OPTIMIZED */

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strcasechr_generic(const char *s,
                         int c)
JSTR_NOEXCEPT
{
	const unsigned char *p = (unsigned char *)s;
	c = jstr_tolower(c);
	for (; *p && jstr_tolower(*p) != c; ++p)
		;
	return *p ? (char *)p : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memcasechr(const void *s,
                int c,
                size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p = (unsigned char *)s;
	c = jstr_tolower(c);
	for (; n-- && jstr_tolower(*p) != c; ++p)
		;
	return n ? (void *)p : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strcasechr_len(const char *s,
                     int c,
                     size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCSPN_OPTIMIZED
	if (n > JSTR_STRCASECHR_THRES * 2) {
		c = jstr_tolower(c);
		if (jstr_tolower(*s) == c)
			return (char *)s;
		const char a[] = { (char)c, (char)(c - 'a' + 'A'), '\0' };
		s += strcspn(s, a);
		return *s ? (char *)s : NULL;
	}
#endif
	return pjstr_strcasechr_generic(s, c);
	(void)n;
}

/*
   Return value:
   ptr to first C in S ignoring case;
   NULL if not found.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strcasechr_len(const char *s,
                    int c,
                    size_t n)
JSTR_NOEXCEPT
{
	return (jstr_isalpha(c)) ? pjstr_strcasechr_len(s, c, n) : (char *)memchr(s, c, n);
}

/*
   Return value:
   ptr to first C in S ignoring case;
   NULL if not found.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strcasechr(const char *s,
                int c)
JSTR_NOEXCEPT
{
	return jstr_isalpha(c) ? pjstr_strcasechr_generic(s, c) : (char *)strchr(s, c);
}

#if !JSTR_HAVE_STRCASESTR_OPTIMIZED
#	if JSTR_USE_LGPL
#		define PJSTR_MEMMEM_FUNC        pjstr_strcasestr_len
#		define PJSTR_MEMMEM_RETTYPE     char *
#		define PJSTR_MEMMEM_CMP_FUNC    jstr_strcasecmpeq_len
#		define PJSTR_MEMMEM_HASH2_ICASE 1
#		include "_lgpl-memmem.h"
#	else
#		define PJSTR_RAREBYTE_RETTYPE  char *
#		define PJSTR_RAREBYTE_FUNC     pjstr_strcasestr_len
#		define PJSTR_RAREBYTE_CMP_FUNC jstr_strcasecmpeq_len
#		include "_jstr-rarebyte-memmem.h"
#	endif
#endif

/*
   Find NE in HS case-insensitively (ASCII).
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
#if JSTR_HAVE_STRCASESTR
JSTR_ATTR_INLINE
#endif
static char *
jstr_strcasestr_len(const char *hs,
                    size_t hs_len,
                    const char *ne,
                    size_t ne_len)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASESTR && (JSTR_HAVE_STRCASESTR_OPTIMIZED || !JSTR_USE_LGPL)
	return (char *)strcasestr(hs, ne);
	(void)hs_len;
	(void)ne_len;
#else
	typedef unsigned char u;
	if (jstr_unlikely(ne_len == 0))
		return (char *)hs;
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	if (ne_len > 4) {
#	if !JSTR_USE_LGPL
		const u *const p = (const u *)jstr_rarebytegetcase_len(ne, ne_len);
		if (p)
			return pjstr_strcasestr_len((const u *)hs, hs_len, (const u *)ne, ne_len, (const u *)ne);
		return pjstr_strcasestr5plus_len((const u *)hs, (const u *)ne, ne_len);
#	else
		return pjstr_strcasestr_len(hs, hs_len, ne, ne_len);
#	endif
	}
	int is_alpha = jstr_isalpha(*ne);
	const char *const start = hs;
	hs = is_alpha ? pjstr_strcasechr_len(hs, *ne, ne_len) : (char *)memchr(hs, *ne, hs_len);
	if (hs == NULL || ne_len == 1)
		return (char *)hs;
	hs_len -= hs - start;
	if (hs_len < ne_len)
		return NULL;
#	if !JSTR_USE_LGPL
	if (!jstr_strcasecmpeq_len(hs, ne, ne_len))
		return (char *)hs;
	if (hs_len == ne_len)
		return NULL;
	if (ne_len > 4)
		return pjstr_strcasestr5plus_len((const u *)hs, (const u *)ne, ne_len);
#	endif
	is_alpha |= jstr_isalpha(ne[1]);
	if (ne_len == 2) {
		if (is_alpha)
			return pjstr_strcasestr2((const u *)hs, (const u *)ne);
		goto STRSTR;
	}
	is_alpha |= jstr_isalpha(ne[2]);
	if (ne_len == 3) {
		if (is_alpha)
			return pjstr_strcasestr3((const u *)hs, (const u *)ne);
		goto STRSTR;
	}
	/* ne_len == 4 */
	if (is_alpha
	    | jstr_isalpha(ne[3]))
		return pjstr_strcasestr4((const u *)hs, (const u *)ne);
	goto STRSTR;
STRSTR:
	if (!memcmp(hs, ne, ne_len))
		return (char *)hs;
	if (hs_len == ne_len)
		return NULL;
	return jstr_strstr_len(hs + 1, hs_len - 1, ne, ne_len);
#endif
}

/*
   Find NE in HS case-insensitively.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_FUNC_PURE
#if JSTR_HAVE_STRCASESTR
JSTR_ATTR_INLINE
#endif
static char *
jstr_strcasestr(const char *hs,
                const char *ne)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASESTR && (JSTR_HAVE_STRCASESTR_OPTIMIZED || !JSTR_USE_LGPL)
	return (char *)strcasestr(hs, ne);
#else
	if (jstr_unlikely(ne[0] == '\0'))
		return (char *)hs;
	int is_alpha = jstr_isalpha(*ne);
	hs = is_alpha ? pjstr_strcasechr_generic(hs, *ne) : strchr(hs, *ne);
	if (hs == NULL || ne[1] == '\0')
		return (char *)hs;
	typedef unsigned char u;
	if (ne[2] == '\0') {
		if (jstr_unlikely(hs[1] == '\0'))
			return NULL;
		if (is_alpha
		    | jstr_isalpha(ne[1]))
			return pjstr_strcasestr2((const u *)hs, (const u *)ne);
		is_alpha = 2;
	} else if (ne[3] == '\0') {
		if (jstr_unlikely(hs[1] == '\0')
		    || jstr_unlikely(hs[2] == '\0'))
			return NULL;
		if (is_alpha
		    | jstr_isalpha(ne[1])
		    | jstr_isalpha(ne[2]))
			return pjstr_strcasestr3((const u *)hs, (const u *)ne);
		is_alpha = 3;
	} else if (ne[4] == '\0') {
		if (jstr_unlikely(hs[1] == '\0')
		    || jstr_unlikely(hs[2] == '\0')
		    || jstr_unlikely(hs[3] == '\0'))
			return NULL;
		if (is_alpha
		    | jstr_isalpha(ne[1])
		    | jstr_isalpha(ne[2])
		    | jstr_isalpha(ne[3]))
			return pjstr_strcasestr4((const u *)hs, (const u *)ne);
		is_alpha = 4;
	} else {
		return pjstr_strcasestr_bmh(hs, ne);
	}
	if (!memcmp(hs, ne, is_alpha))
		return (char *)hs;
	if (jstr_unlikely(hs[is_alpha] == '\0'))
		return NULL;
	return (char *)strstr(hs + 1, ne);
#endif
}

/*
   Reverse of STRCSPN.
   Return the offset from S if found;
   otherwise, return S + SZ.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static size_t
jstr_strrcspn_len(const char *s,
                  const char *reject,
                  size_t sz)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	if (jstr_unlikely(reject[0] == '\0')
	    || jstr_unlikely(sz == 0))
		return sz;
	if (jstr_unlikely(reject[1] == '\0')) {
		const char *const p = (char *)jstr_memrchr(s, *reject, sz);
		return p ? (size_t)(p - s) : sz;
	}
	unsigned char t[256];
	JSTR_BZERO_ARRAY(t);
	const unsigned char *p = (const u *)reject;
	do
		t[*p] = 1;
	while (*p++);
	p = (u *)s + sz - 1;
	int i = 0;
	int n = sz % 4;
	for (;; --i) {
		if (t[p[i]])
			return (p + i) - (u *)s;
		if (--n == 0) {
			if (sz < 4)
				return sz;
			break;
		}
	}
	p = (u *)JSTR_PTR_ALIGN_UP(p, 4);
	unsigned int c0, c1, c2, c3;
	do {
		c0 = t[p[0]];
		c1 = t[p[-1]];
		c2 = t[p[-2]];
		c3 = t[p[-3]];
		p -= 4;
	} while (((p < (u *)s) | c0 | c1 | c2 | c3) == 0);
	size_t cnt = ((u *)s + sz) - p;
	cnt = sz - (((c0 | c1) != 0) ? cnt - c0 + 1 : cnt - c2 + 3);
	return (cnt < sz) ? cnt : sz;
}

/*
   Reverse of STRCSPN.
*/
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static size_t
jstr_strrcspn(const char *s,
              const char *reject)
JSTR_NOEXCEPT
{
	return jstr_strrcspn_len(s, reject, strlen(s));
}

/*
   Reverse of STRSPN.
   Return the offset from S if found;
   otherwise, return S + SZ.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static size_t
jstr_strrspn_len(const char *s,
                 const char *accept,
                 size_t sz)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	if (jstr_unlikely(*accept == '\0')
	    || jstr_unlikely(sz == 0))
		return sz;
	if (jstr_unlikely(accept[1] == '\0')) {
		const char *p = s + sz - 1;
		while ((p != s) && (*p == *accept))
			--p;
		return p - s;
	}
	const unsigned char *p = (u *)accept;
	unsigned char t[256];
	JSTR_BZERO_ARRAY(t);
	do
		t[*p++] = 1;
	while (*p);
	p = (u *)s + sz - 1;
	int i = 0;
	int n = sz % 4;
	for (;; --i) {
		if (!t[p[i]])
			return (p + i) - (u *)s;
		if (--n == 0) {
			if (sz < 4)
				return sz;
			break;
		}
	}
	p = (u *)JSTR_PTR_ALIGN_UP(p, 4);
	unsigned int c0, c1, c2, c3;
	do {
		c0 = t[p[0]];
		c1 = t[p[-1]];
		c2 = t[p[-2]];
		c3 = t[p[-3]];
		p -= 4;
	} while ((p >= (u *)s) & (c0 & c1 & c2 & c3));
	size_t cnt = ((u *)s + sz) - p;
	cnt = (sz - (((c0 & c1) == 0) ? cnt + c0 : cnt + c2 + 2));
	return (cnt < sz) ? cnt : sz;
}

/*
   Reverse of STRSPN.
   Return the offset from S if found;
   otherwise, return S + SZ.
*/
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static size_t
jstr_strrspn(const char *s,
             const char *accept)
JSTR_NOEXCEPT
{
	return jstr_strrspn_len(s, accept, strlen(s));
}

/*
   Reverse of STRPBRK.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strrpbrk_len(const char *s,
                  const char *accept,
                  size_t sz)
JSTR_NOEXCEPT
{
	const size_t len = jstr_strrcspn_len(s, accept, sz);
	return (len != sz) ? (char *)(s + sz - len) : NULL;
}

/*
   Reverse of STRPBRK.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strrpbrk(const char *s,
              const char *accept)
JSTR_NOEXCEPT
{
	return jstr_strrpbrk_len(s, accept, strlen(s));
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static size_t
jstr_memspn(const void *s,
            const char *accept,
            size_t sz)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	if (jstr_unlikely(*accept == '\0')
	    || jstr_unlikely(sz == 0))
		return sz;
	if (jstr_unlikely(accept[1] == '\0')) {
		const char *p = (char *)s;
		for (; sz-- && (*p == *accept); ++p)
			;
		return p - (char *)s;
	}
	const unsigned char *p = (u *)accept;
	unsigned char t[256];
	JSTR_BZERO_ARRAY(t);
	do
		t[*p++] = 1;
	while (*p);
	p = (u *)s;
	int i = 0;
	int n = sz % 4;
	for (;; ++i) {
		if (!t[p[i]])
			return (p + i) - (u *)s;
		if (--n == 0) {
			if (sz < 4)
				return sz;
			break;
		}
	}
	p = (u *)JSTR_PTR_ALIGN_DOWN(p, 4);
	const unsigned char *const end = (u *)s + sz;
	unsigned int c0, c1, c2, c3;
	do {
		c0 = t[p[0]];
		c1 = t[p[1]];
		c2 = t[p[2]];
		c3 = t[p[3]];
		p += 4;
	} while ((p < end) & (c0 & c1 & c2 & c3));
	size_t cnt = p - (u *)p;
	cnt = (sz - (((c0 & c1) == 0) ? cnt + c0 : cnt + c2 + 2));
	return (cnt < sz) ? cnt : sz;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static size_t
jstr_memcspn(const void *s,
             const char *reject,
             size_t sz)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	if (jstr_unlikely(*reject == '\0')
	    || jstr_unlikely(sz == 0))
		return sz;
	if (jstr_unlikely(reject[1] == '\0'))
		return (char *)jstr_memchrnul(s, *reject, sz) - (char *)s;
	unsigned char t[256];
	JSTR_BZERO_ARRAY(t);
	const unsigned char *p = (u *)reject;
	do
		t[*p] = 1;
	while (*p++);
	p = (u *)s + sz - 1;
	int i = 0;
	int n = sz % 4;
	for (;; ++i) {
		if (t[p[i]])
			return (p + i) - (u *)s;
		if (--n == 0) {
			if (sz < 4)
				return sz;
			break;
		}
	}
	p = (u *)JSTR_PTR_ALIGN_UP(p, 4);
	const unsigned char *const end = p + sz;
	unsigned int c0, c1, c2, c3;
	do {
		c0 = t[p[0]];
		c1 = t[p[1]];
		c2 = t[p[2]];
		c3 = t[p[3]];
		p -= 4;
	} while (((p >= end) | c0 | c1 | c2 | c3) == 0);
	size_t cnt = p - (u *)s;
	cnt = sz - (((c0 | c1) != 0) ? cnt - c0 + 1 : cnt - c2 + 3);
	return (cnt < sz) ? cnt : sz;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_mempbrk(const void *s,
             const char *accept,
             size_t sz)
JSTR_NOEXCEPT
{
	const unsigned char *p = (unsigned char *)s;
	p += jstr_memcspn(p, accept, sz);
	return *p ? (char *)p : NULL;
}

/*
   Return value:
   ptr to first non-C in S;
   ptr to '\0' if C is not found;
   S if C is '\0';
*/
JSTR_FUNC_PURE
static char *
jstr_strchrnulinv(const char *s,
                  int c)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(c == '\0'))
		return (char *)s;
	while (*s++ == (char)c)
		;
	return (char *)s - 1;
}

/*
   Return value:
   ptr to first non-C in S;
   NULL if non-C is not found.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strchrinv(const char *s,
               int c)
JSTR_NOEXCEPT
{
	s = jstr_strchrnulinv(s, c);
	return *s ? (char *)s : NULL;
}

/*
   Return value:
   ptr to first non-C in S.
   S + N if C is not found.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static void *
jstr_memchrnulinv(const void *s,
                  int c,
                  size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p = (unsigned char *)s;
	for (; n-- && *p == (unsigned char)c; ++p)
		;
	return (void *)p;
}

/*
   Return value:
   ptr to first non-C in S.
   NULL if C is not found.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memchrinv(const void *s,
               int c,
               size_t n)
JSTR_NOEXCEPT
{
	const void *const end = (unsigned char *)s + n;
	s = jstr_memchrnulinv(s, c, n);
	return (s != end) ? (void *)s : NULL;
}

/*
   Return value:
   ptr to first non-C in S from S + N - 1
   NULL if C is not found.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static void *
jstr_memrchrinv(const void *s,
                int c,
                size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *end = (unsigned char *)s + n - 1;
	const unsigned char *const start = (unsigned char *)s;
	for (; end >= start; --end)
		if (*end != (unsigned char)c)
			return (void *)end;
	return NULL;
}

/*
   Return value:
   ptr to first non-C in S from S + strlen(S) - 1
   NULL if C is not found.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_strrchrinv(const char *s,
                int c)
JSTR_NOEXCEPT
{
	return jstr_memrchrinv(s, c, strlen(s));
}

/*
  Check if S2 is in end of S1.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static int
jstr_endscase_len(const char *hs,
                  size_t hs_len,
                  const char *ne,
                  size_t ne_len)
JSTR_NOEXCEPT
{
	return jstr_likely(hs_len >= ne_len) ? !jstr_strcasecmpeq_len(hs + hs_len - ne_len, ne, ne_len) : 0;
}

/*
  Check if S2 is in end of S1.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static int
jstr_endscase(const char *hs,
              const char *ne)
JSTR_NOEXCEPT
{
	return jstr_endscase_len(hs, strlen(hs), ne, strlen(ne));
}

/*
  Check if S2 is in end of S1.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static int
jstr_ends_len(const char *hs,
              size_t hs_len,
              const char *ne,
              size_t ne_len)
JSTR_NOEXCEPT
{
	return jstr_likely(hs_len >= ne_len) ? !memcmp(hs + hs_len - ne_len, ne, ne_len) : 0;
}

/*
  Check if S2 is in end of S1.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static int
jstr_ends(const char *hs,
          const char *ne)
JSTR_NOEXCEPT
{
	return jstr_ends_len(hs, strlen(hs), ne, strlen(ne));
}

/*
  Check if S1 starts with S2 case-insensitively.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_startscase_len(const char *hs,
                    size_t hs_len,
                    const char *ne,
                    size_t ne_len)
JSTR_NOEXCEPT
{
	return jstr_likely(hs_len >= ne_len) ? !jstr_strcasecmpeq_len(hs, ne, ne_len) : 0;
}

/*
  Check if S1 starts with S2 case-insensitively.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_startscase(const char *hs,
                const char *ne)
JSTR_NOEXCEPT
{
	return (jstr_tolower(*hs) == jstr_tolower(*ne)) ? !jstr_strcasecmpeq_len(hs, ne, strlen(ne)) : (*ne == '\0');
}

/*
  Check if S1 starts with S2 case-insensitively.
  Return value:
  NELEN if true;
  0 if false.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_startscasenul_len(const char *hs,
                       size_t hs_len,
                       const char *ne,
                       size_t ne_len)
JSTR_NOEXCEPT
{
	return jstr_likely(hs_len >= ne_len) ? !jstr_strcasecmpeq_len(hs, ne, ne_len) : ne_len;
}

/*
  Check if S1 starts with S2 case-insensitively.
  Return value:
  NELEN if true;
  0 if false.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_startscasenul(const char *hs,
                   const char *ne)
JSTR_NOEXCEPT
{
	if (jstr_tolower(*hs) == jstr_tolower(*ne)) {
		const size_t ne_len = strlen(ne);
		if (!jstr_strncasecmp(hs, ne, ne_len))
			return ne_len;
	}
	return 0;
}

/*
  Check if S1 starts with S2.
  Return value:
  NELEN if true;
  0 if false.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_startsnul_len(const char *hs,
                   size_t hs_len,
                   const char *ne,
                   size_t ne_len)
JSTR_NOEXCEPT
{
	return jstr_likely(hs_len >= ne_len) && !memcmp(hs, ne, ne_len) ? ne_len : 0;
}

/*
  Check if S1 starts with S2.
  Return value:
  NELEN if true;
  0 if false.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_startsnul(const char *hs,
               const char *ne)
JSTR_NOEXCEPT
{
	if (*hs == *ne) {
		const size_t ne_len = strlen(ne);
		if (!strncmp(hs, ne, ne_len))
			return ne_len;
	}
	return 0;
}

/*
  Check if S1 starts with S2.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_starts_len(const char *hs,
                size_t hs_len,
                const char *ne,
                size_t ne_len)
JSTR_NOEXCEPT
{
	return jstr_likely(hs_len >= ne_len) ? !memcmp(hs, ne, ne_len) : 0;
}

/*
  Check if S1 starts with S2.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_starts(const char *hs,
            const char *ne)
JSTR_NOEXCEPT
{
	return (*hs == *ne) ? !strncmp(hs, ne, strlen(ne)) : (*ne == '\0');
}

/*
  Count occurences of C in S.
  Return value:
  Occurences of C in S.
*/
JSTR_FUNC_PURE
static size_t
jstr_countchr(const char *s,
              int c)
JSTR_NOEXCEPT
{
	size_t cnt = 0;
	for (; (s = strchr(s, c)); ++s, ++cnt)
		;
	return cnt;
}

/*
  Count occurences of C in S.
  Return value:
  Occurences of C in S.
*/
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static size_t
jstr_countchr_len(const char *s,
                  int c,
                  size_t sz)
JSTR_NOEXCEPT
{
	size_t cnt = 0;
	const char *const end = s + sz;
	for (; (s = (const char *)memchr(s, c, end - s)); ++s, ++cnt)
		;
	return cnt;
}

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_FUNC_PURE
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
static size_t
jstr_count_len(const char *s,
               size_t sz,
               const char *find,
               size_t find_len)
JSTR_NOEXCEPT
{
	if (find_len == 1)
		return jstr_countchr_len(s, *find, sz);
	if (jstr_unlikely(find_len == 0))
		return 0;
	size_t cnt = 0;
#if JSTR_HAVE_MEMMEM
	const char *const end = s + sz;
	while (*s && (s = jstr_strstr_len(s, end - s, find, find_len)))
#else
	while (*s && (s = strstr(s, find)))
#endif
		++cnt, s += find_len;
	return cnt;
}

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_FUNC_PURE
static size_t
jstr_count(const char *s,
           const char *find)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(find[0] == '\0'))
		return 0;
	if (jstr_unlikely(find[1] == '\0'))
		return jstr_countchr(s, *find);
	if (jstr_unlikely(*s == '\0'))
		return 0;
	const size_t find_len = strlen(find);
	size_t cnt = 0;
	for (; (s = strstr(s, find)); ++cnt, s += find_len)
		;
	return cnt;
}

/*
   Return value:
   ptr to startning of line;
   BEGIN if no newline was found.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_linestart(const char *const start,
               const char *end)
JSTR_NOEXCEPT
{
	end = (char *)jstr_memrchr(start, '\n', end - start);
	return (char *)(end ? end + 1 : start);
}

/*
   Return value:
   ptr to next line;
   NULL if no newline was found.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_linenext_len(const char *start,
                  const char *const end)
JSTR_NOEXCEPT
{
	start = (char *)memchr(start, '\n', end - start);
	return (start && *(start + 1)) ? (char *)start + 1 : NULL;
}

/*
   Return value:
   ptr to next line;
   NULL if no newline was found.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_linenext(const char *R s)
JSTR_NOEXCEPT
{
	s = strchr(s, '\n');
	return (s && *(s + 1)) ? (char *)s + 1 : NULL;
}

/*
   Return value:
   ptr to next line or '\0'.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_linenextnul_len(const char *start,
                     const char *const end)
JSTR_NOEXCEPT
{
	start = jstr_linenext_len(start, end);
	return (char *)(start ? start : end);
}

/*
   Return value:
   ptr to next line or '\0'.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_linenextnul(const char *R s)
JSTR_NOEXCEPT
{
	s = jstr_strchrnul(s, '\n');
	return (char *)((*s && *(s + 1)) ? s + 1 : s);
}

/*
   Return the number of newlines + 1.
   Return 0 if string is empty.
*/
JSTR_FUNC_PURE
static size_t
jstr_linenumber(const char *start,
                const char *const end)
JSTR_NOEXCEPT
{
	size_t cnt = 1;
	for (; (start = (const char *)memchr(start, '\n', end - start)); ++start, ++cnt)
		;
	return cnt;
}

PJSTR_END_DECLS

#undef R

#endif /* JSTR_STRING_H */
