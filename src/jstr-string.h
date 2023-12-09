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
#include "jstr-ctype.h"

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

#define PJSTR_STRSTR234_FUNC pjstr_strstr
#include "_strstr234.h"

#define PJSTR_STRSTR234_MEMMEM
#define PJSTR_STRSTR234_FUNC pjstr_memmem
#include "_strstr234.h"

#define PJSTR_STRSTR234_FUNC     pjstr_strcasestr
#define PJSTR_STRSTR234_CANONIZE jstr_tolower
#include "_strstr234.h"

#define PJSTR_STRSTR234_MEMMEM
#define PJSTR_STRSTR234_FUNC     pjstr_memcasemem
#define PJSTR_STRSTR234_CANONIZE jstr_tolower
#include "_strstr234.h"

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strnstr2(const unsigned char *hs,
               const unsigned char *const ne,
               size_t n)
JSTR_NOEXCEPT
{
#if JSTR_LP64
	typedef uint32_t size_ty;
	enum { SHIFT = 16 };
#else
	typedef uint16_t size_ty;
	enum { SHIFT = 8 };
#endif
	const size_ty nw = (size_ty)ne[0] << SHIFT | ne[1];
	size_ty hw = (size_ty)hs[0] << SHIFT | hs[1];
	for (++hs, --n; n-- && *hs && hw != nw; hw = hw << SHIFT | *++hs) {}
	return (hw == nw) ? (char *)hs - 1 : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strnstr3(const unsigned char *hs,
               const unsigned char *const ne,
               size_t n)
JSTR_NOEXCEPT
{
	const uint32_t nw = (uint32_t)ne[0] << 24 | ne[1] << 16 | ne[2] << 8;
	uint32_t hw = (uint32_t)hs[0] << 24 | hs[1] << 16 | hs[2] << 8;
	for (hs += 2, n -= 2; n-- && *hs && hw != nw; hw = (hw | *++hs) << 8) {}
	return (hw == nw) ? (char *)hs - 2 : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strnstr4(const unsigned char *hs,
               const unsigned char *const ne,
               size_t n)
JSTR_NOEXCEPT
{
	const uint32_t nw = (uint32_t)ne[0] << 24 | ne[1] << 16 | ne[2] << 8 | ne[3];
	uint32_t hw = (uint32_t)hs[0] << 24 | hs[1] << 16 | hs[2] << 8 | hs[3];
	for (hs += 3, n -= 3; n-- && *hs && hw != nw; hw = hw << 8 | *++hs) {}
	return (hw == nw) ? (char *)hs - 3 : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
pjstr_memrmem2(const unsigned char *hs,
               const unsigned char *const ne,
               size_t l)
JSTR_NOEXCEPT
{
#if JSTR_LP64
	typedef uint32_t size_ty;
	enum { SHIFT = 16 };
#else
	typedef uint16_t size_ty;
	enum { SHIFT = 8 };
#endif
	hs += l - 2;
	const size_ty nw = (size_ty)ne[1] << SHIFT | ne[0];
	size_ty hw = (size_ty)hs[1] << SHIFT | hs[0];
	for (l -= 2; l-- && hw != nw; hw = hw << SHIFT | *--hs) {}
	return (hw == nw) ? (void *)hs : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
pjstr_memrmem3(const unsigned char *hs,
               const unsigned char *const ne,
               size_t l)
JSTR_NOEXCEPT
{
	hs += l - 3;
	const uint32_t nw = (uint32_t)ne[2] << 24 | ne[1] << 16 | ne[0] << 8;
	uint32_t hw = (uint32_t)hs[2] << 24 | hs[1] << 16 | hs[0] << 8;
	for (l -= 3; l-- && hw != nw; hw = (hw | *--hs) << 8) {}
	return (hw == nw) ? (void *)hs : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
pjstr_memrmem4(const unsigned char *hs,
               const unsigned char *const ne,
               size_t l)
JSTR_NOEXCEPT
{
	hs += l - 4;
	const uint32_t nw = (uint32_t)ne[3] << 24 | ne[2] << 16 | ne[1] << 8 | ne[0];
	uint32_t hw = (uint32_t)hs[3] << 24 | hs[2] << 16 | hs[1] << 8 | hs[0];
	for (l -= 4; l-- && hw != nw; hw = hw << 8 | *--hs) {}
	return (hw == nw) ? (void *)hs : NULL;
}

#define PJSTR_MEMMEM_RETTYPE void *
#define PJSTR_MEMMEM_FUNC    pjstr_memmem_bmh
#include "_memmem-bmh.h"

#define PJSTR_RAREBYTE_RETTYPE void *
#define PJSTR_RAREBYTE_FUNC    pjstr_memmem_rarebyte
#include "_jstr-rarebyte-memmem.h"

#if JSTR_HAVE_MEMMEM && (JSTR_USE_STANDARD_ALWAYS || JSTR_HAVE_MEMMEM_OPTIMIZED) && !JSTR_TEST
#	define JSTR_USE_STANDARD_MEMMEM 1
#else
#	define JSTR_USE_STANDARD_MEMMEM 0
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
#if JSTR_USE_STANDARD_MEMMEM
JSTR_ATTR_INLINE
#endif
static void *
jstr_memmem(const void *hs,
            size_t hs_len,
            const void *ne,
            size_t ne_len)
JSTR_NOEXCEPT
{
#if JSTR_USE_STANDARD_MEMMEM
	return memmem(hs, hs_len, ne, ne_len);
#else
	typedef const unsigned char cu;
	enum { LONG_NE_THRES = 16,
	       VERY_LONG_NE_THRES = 100 };
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	if (jstr_unlikely(ne_len == 0))
		return (char *)hs;
	if (jstr_unlikely(ne_len >= VERY_LONG_NE_THRES))
		goto MEMMEM;
	cu *rare;
	rare = (cu *)jstr_rarebytefind_len(ne, ne_len);
	size_t shift;
	shift = JSTR_PTR_DIFF(rare, ne);
	hs = (cu *)hs + shift;
	hs_len -= shift;
	const void *start;
	start = (const void *)hs;
	hs = (void *)memchr(hs, *rare, hs_len - (ne_len - shift) + 1);
	if (jstr_unlikely(hs == NULL) || ne_len == 1)
		return (void *)hs;
	hs = (cu *)hs - shift;
	hs_len = hs_len - JSTR_PTR_DIFF(hs, start);
	if (ne_len == 2)
		return pjstr_memmem2((cu *)hs, (cu *)ne, hs_len);
	if (ne_len == 3)
		return pjstr_memmem3((cu *)hs, (cu *)ne, hs_len);
	if (ne_len == 4)
		return pjstr_memmem4((cu *)hs, (cu *)ne, hs_len);
	if (*(cu *)hs == *(cu *)ne && !memcmp(hs, ne, ne_len))
		return (char *)hs;
	if (jstr_unlikely(hs_len == ne_len))
		return NULL;
	hs = (char *)hs + 1;
	--hs_len;
	if (ne_len < LONG_NE_THRES)
		return pjstr_memmem_rarebyte((cu *)hs, hs_len, (cu *)ne, ne_len, rare);
MEMMEM:
	return pjstr_memmem_bmh((cu *)hs, hs_len, (cu *)ne, ne_len);
#endif
}

/* HS and NE must be nul-terminated. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strstr_len(const char *hs,
                size_t hslen,
                const char *ne,
                size_t nelen)
JSTR_NOEXCEPT
{
	/* Optimized strstr() is still faster than a C memmem(). */
#if JSTR_HAVE_STRSTR_OPTIMIZED
	if (nelen < JSTR_MEMMEM_THRES)
		return (char *)strstr(hs, ne);
#endif
	return (char *)jstr_memmem(hs, hslen, ne, nelen);
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
}

JSTR_FUNC_PURE
static char *
jstr_strnstr(const char *hs,
             const char *const ne,
             size_t n)
JSTR_NOEXCEPT
{
	typedef const unsigned char cu;
	enum { LONG_NE_THRES = 16 };
	if (jstr_unlikely(*ne == '\0'))
		return (char *)hs;
	const char *const start = hs;
	hs = jstr_strnchr(hs, *ne, n);
	if (jstr_unlikely(hs == NULL) || ne[1] == '\0')
		return (char *)hs;
	n -= JSTR_PTR_DIFF(hs, start);
	if (jstr_unlikely(hs[1] == '\0')
	    || jstr_unlikely(n == 1))
		return NULL;
	if (ne[2] == '\0')
		return pjstr_strnstr2((cu *)hs, (cu *)ne, n);
	if (jstr_unlikely(hs[2] == '\0')
	    || jstr_unlikely(n == 2))
		return NULL;
	if (ne[3] == '\0')
		return pjstr_strnstr3((cu *)hs, (cu *)ne, n);
	if (jstr_unlikely(hs[3] == '\0')
	    || jstr_unlikely(n == 3))
		return NULL;
	if (ne[4] == '\0')
		return pjstr_strnstr4((cu *)hs, (cu *)ne, n);
	if (jstr_unlikely(hs[4] == '\0')
	    || jstr_unlikely(n == 4))
		return NULL;
	cu *hp = (cu *)hs;
	cu *np = (cu *)ne;
	size_t tmp = n;
	for (; tmp-- && *hp == *np && *hp; ++hp, ++np) {}
	if (*np == '\0')
		return (char *)hs;
	if (jstr_unlikely(*hp == '\0'))
		return NULL;
	tmp = JSTR_PTR_DIFF(np, ne);
	const size_t ne_len = strlen((char *)np) + tmp;
	if (jstr_unlikely(n < ne_len))
		return NULL;
	const size_t hs_len = jstr_strnlen((char *)hp, n - tmp) + tmp;
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
#if JSTR_USE_STANDARD_MEMMEM
	return (char *)memmem(hs, hs_len, ne, ne_len);
#else
	if (ne_len < LONG_NE_THRES)
		return (char *)pjstr_memmem_rarebyte((cu *)hs, hs_len, (cu *)ne, ne_len, (cu *)jstr_rarebytefind(ne));
	else
		return (char *)pjstr_memmem_bmh((cu *)hs, hs_len, (cu *)ne, ne_len);
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

#define PJSTR_RAREBYTE_RETTYPE char *
#define PJSTR_RAREBYTE_FUNC    pjstr_memrmem_len_rarebyte
#include "_jstr-memrmem-rarebyte.h"

/* Find last NE in HS.
   Return value:
   Pointer to NE;
   NULL if not found. */
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
	typedef const unsigned char cu;
	if (jstr_unlikely(ne_len == 0))
		return (char *)hs + hs_len;
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	cu *const rare = (cu *)jstr_rarebytefind_len(ne, ne_len);
	const size_t shift = JSTR_PTR_DIFF(rare, ne);
	cu *p = (cu *)jstr_memrchr(hs, *rare, hs_len - (ne_len - shift) + 1);
	if (jstr_unlikely(p == NULL) || ne_len == 1)
		return (void *)p;
	hs_len = JSTR_PTR_DIFF(p, hs) - shift + ne_len + 1;
	if (ne_len == 2)
		return pjstr_memrmem2((cu *)hs, (cu *)ne, hs_len);
	if (ne_len == 3)
		return pjstr_memrmem3((cu *)hs, (cu *)ne, hs_len);
	if (ne_len == 4)
		return pjstr_memrmem4((cu *)hs, (cu *)ne, hs_len);
	return pjstr_memrmem_len_rarebyte((cu *)hs, hs_len, (cu *)ne, ne_len, rare);
}

/* Find last NE in HS.
   Return value:
   Pointer to NE;
   NULL if not found. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
static char *
jstr_strrstr_len(const char *hs,
                 size_t hs_len,
                 const char *ne,
                 size_t ne_len)
JSTR_NOEXCEPT
{
	return (char *)jstr_memrmem(hs, hs_len, ne, ne_len);
}

#define PJSTR_MEMMEM_FUNC        pjstr_strcasestr_bmh
#define PJSTR_MEMMEM_RETTYPE     char *
#define PJSTR_MEMMEM_CMP_FUNC    jstr_strcasecmpeq_len
#define PJSTR_MEMMEM_HASH2_ICASE 1
#define PJSTR_MEMMEM_CHECK_EOL   1
#include "_memmem-bmh.h"

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
pjstr_strcasestr_long(const char *hs,
                      const char *ne)
JSTR_NOEXCEPT
{
	const size_t ne_len = strlen(ne);
	const size_t hs_len = jstr_strnlen(hs, ne_len + 256);
	if (hs_len < ne_len)
		return NULL;
	if (jstr_tolower(*hs) == jstr_tolower(*ne) && !jstr_strcasecmpeq_len(hs, ne, ne_len))
		return (char *)hs;
	if (jstr_unlikely(hs_len == ne_len))
		return NULL;
	return pjstr_strcasestr_bmh(hs, hs_len, ne, ne_len);
}

#define PJSTR_MEMMEM_FUNC        pjstr_strcasestr_len_bmh
#define PJSTR_MEMMEM_RETTYPE     char *
#define PJSTR_MEMMEM_CMP_FUNC    jstr_strcasecmpeq_len
#define PJSTR_MEMMEM_HASH2_ICASE 1
#include "_memmem-bmh.h"

#define PJSTR_RAREBYTE_RETTYPE  char *
#define PJSTR_RAREBYTE_FUNC     pjstr_strcasestr_len_rarebyte
#define PJSTR_RAREBYTE_CMP_FUNC jstr_strcasecmpeq_len
#define PJSTR_RAREBYTE_CANONIZE jstr_tolower
#include "_jstr-rarebyte-memmem.h"

#if (JSTR_HAVE_STRCASESTR && (JSTR_USE_STANDARD_ALWAYS || JSTR_HAVE_STRCASESTR_OPTIMIZED) && !JSTR_TEST)
#	define JSTR_USE_STANDARD_STRCASESTR 1
#else
#	define JSTR_USE_STANDARD_STRCASESTR 0
#endif

/* Find NE in HS case-insensitively (ASCII).
   Return value:
   Pointer to NE;
   NULL if not found. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
#if JSTR_USE_STANDARD_STRCASESTR
JSTR_ATTR_INLINE
#endif
static char *
jstr_strcasestr_len(const char *hs,
                    size_t hs_len,
                    const char *ne,
                    size_t ne_len)
JSTR_NOEXCEPT
{
#if JSTR_USE_STANDARD_STRCASESTR
	return (char *)strcasestr(hs, ne);
#else
	typedef const unsigned char cu;
	enum { LONG_NE_THRES = 16 };
	if (jstr_unlikely(ne_len == 0))
		return (char *)hs;
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	if (ne_len > 4)
		return pjstr_strcasestr_len_bmh(hs, hs_len, ne, ne_len);
	cu *rare = (cu *)jstr_rarebytefindeither_len(ne, ne_len);
	/* ne_len <= 4 */
	unsigned int shift = JSTR_PTR_DIFF(rare, ne);
	hs += shift;
	hs_len -= shift;
	const char *const start = hs;
	hs = jstr_strcasechr(hs, *rare);
	if (jstr_unlikely(hs == NULL) || ne_len == 1)
		return (char *)hs;
	hs -= shift;
	hs_len = hs_len - JSTR_PTR_DIFF(hs, start);
	/* Reuse SHIFT as IS_ALPHA. */
	shift = jstr_isalpha(*ne) | jstr_isalpha(ne[1]);
	if (ne_len == 2) {
		if (shift)
			return pjstr_strcasestr2((cu *)hs, (cu *)ne);
#	if JSTR_HAVE_MEMMEM_OPTIMIZED || JSTR_HAVE_STRSTR_OPTIMIZED
		goto STRSTR;
#	else
		return pjstr_strstr2((cu *)hs, (cu *)ne);
#	endif
	}
	shift |= jstr_isalpha(ne[2]);
	if (ne_len == 3) {
		if (shift)
			return pjstr_strcasestr3((cu *)hs, (cu *)ne);
#	if JSTR_HAVE_MEMMEM_OPTIMIZED || JSTR_HAVE_STRSTR_OPTIMIZED
		goto STRSTR;
#	else
		return pjstr_strstr3((cu *)hs, (cu *)ne);
#	endif
	}
	/* ne_len == 4 */
	if (shift | jstr_isalpha(ne[3]))
		return pjstr_strcasestr4((cu *)hs, (cu *)ne);
#	if JSTR_HAVE_MEMMEM_OPTIMIZED || JSTR_HAVE_STRSTR_OPTIMIZED
	return pjstr_strstr4((cu *)hs, (cu *)ne);
#	endif
#	if JSTR_HAVE_STRSTR_OPTIMIZED
STRSTR:
	return (char *)strstr(hs, ne);
#	elif JSTR_HAVE_MEMMEM_OPTIMIZED
STRSTR:
	return (char *)memmem(hs, hs_len, ne, ne_len);
#	endif
#endif
}

/* Find NE in HS case-insensitively.
   Return value:
   Pointer to NE;
   NULL if not found. */
JSTR_FUNC_PURE
#if JSTR_USE_STANDARD_STRCASESTR
JSTR_ATTR_INLINE
#endif
static char *
jstr_strcasestr(const char *hs,
                const char *ne)
JSTR_NOEXCEPT
{
#if JSTR_USE_STANDARD_STRCASESTR
	return (char *)strcasestr(hs, ne);
#else
	if (jstr_unlikely(ne[0] == '\0'))
		return (char *)hs;
	typedef const unsigned char cu;
	size_t shift = JSTR_PTR_DIFF(jstr_rarebytefindeither(ne), ne);
	if (jstr_unlikely(jstr_strnlen(hs, shift) < shift))
		return NULL;
	hs = jstr_strcasechr(hs + shift, *(ne + shift));
	if (jstr_unlikely(hs == NULL) || ne[1] == '\0')
		return (char *)hs;
	hs -= shift;
	if (ne[2] == '\0') {
		if (jstr_isalpha(*ne)
		    | jstr_isalpha(ne[1]))
			return pjstr_strcasestr2((cu *)hs, (cu *)ne);
#	if !JSTR_HAVE_STRSTR_OPTIMIZED
		return pjstr_strstr2((cu *)hs, (cu *)ne);
#	else
		goto STRSTR;
#	endif
	}
	if (ne[3] == '\0') {
		if (jstr_isalpha(*ne)
		    | jstr_isalpha(ne[1])
		    | jstr_isalpha(ne[2]))
			return pjstr_strcasestr3((cu *)hs, (cu *)ne);
#	if !JSTR_HAVE_STRSTR_OPTIMIZED
		return pjstr_strstr3((cu *)hs, (cu *)ne);
#	else
		goto STRSTR;
#	endif
	}
	if (ne[4] == '\0') {
		if (jstr_isalpha(*ne)
		    | jstr_isalpha(ne[1])
		    | jstr_isalpha(ne[2])
		    | jstr_isalpha(ne[3]))
			return pjstr_strcasestr4((cu *)hs, (cu *)ne);
#	if !JSTR_HAVE_STRSTR_OPTIMIZED
		return pjstr_strstr4((cu *)hs, (cu *)ne);
#	else
		goto STRSTR;
#	endif
	}
	return pjstr_strcasestr_long(hs, ne);
#	if JSTR_HAVE_STRSTR_OPTIMIZED
STRSTR:
	return (char *)strstr(hs, ne);
#	endif
#endif
}

/* Reverse of STRCSPN.
   Return the offset from S if found;
   otherwise, return S + SZ. */
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
		const char *const p = (const char *)jstr_memrchr(s, *reject, sz);
		return p ? (size_t)(p - s) : sz;
	}
	u t[256];
	JSTR_BZERO_ARRAY(t);
	const u *p = (const u *)reject;
	do
		t[*p] = 1;
	while (*p++);
	p = (u *)s + sz - 1;
	int i = 0;
	int n = sz % 4;
	for (;; --i) {
		if (t[p[i]])
			return JSTR_PTR_DIFF(p + i, s);
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
	size_t cnt = JSTR_PTR_DIFF(s + sz, p);
	cnt = sz - (((c0 | c1) != 0) ? cnt - c0 + 1 : cnt - c2 + 3);
	return (cnt < sz) ? cnt : sz;
}

/* Reverse of STRCSPN. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_strrcspn(const char *s,
              const char *reject)
JSTR_NOEXCEPT
{
	return jstr_strrcspn_len(s, reject, strlen(s));
}

/* Reverse of STRSPN.
   Return the offset from S if found;
   otherwise, return S + SZ. */
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
		const int c = *accept;
		for (; sz-- && *p == c; --p) {}
		return JSTR_PTR_DIFF(p, s);
	}
	const u *p = (u *)accept;
	u t[256];
	JSTR_BZERO_ARRAY(t);
	do
		t[*p++] = 1;
	while (*p);
	p = (u *)s + sz - 1;
	int i = 0;
	int n = sz % 4;
	for (;; --i) {
		if (!t[p[i]])
			return JSTR_PTR_DIFF(p + i, s);
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
	size_t cnt = JSTR_PTR_DIFF(s + sz, p);
	cnt = (sz - (((c0 & c1) == 0) ? cnt + c0 : cnt + c2 + 2));
	return (cnt < sz) ? cnt : sz;
}

/* Reverse of STRSPN.
   Return the offset from S if found;
   otherwise, return S + SZ. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_strrspn(const char *s,
             const char *accept)
JSTR_NOEXCEPT
{
	return jstr_strrspn_len(s, accept, strlen(s));
}

/* Reverse of STRPBRK. */
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

/* Reverse of STRPBRK. */
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
		const int c = *accept;
		for (; sz-- && *p == c; ++p) {}
		return JSTR_PTR_DIFF(p, s);
	}
	const u *p = (u *)accept;
	u t[256];
	JSTR_BZERO_ARRAY(t);
	do
		t[*p++] = 1;
	while (*p);
	p = (u *)s;
	for (int i = 0, n = sz % 4;; ++i) {
		if (!t[p[i]])
			return JSTR_PTR_DIFF(p + i, s);
		if (--n == 0) {
			if (sz < 4)
				return sz;
			break;
		}
	}
	p = (u *)JSTR_PTR_ALIGN_DOWN(p, 4);
	const u *const end = (const u *)s + sz;
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
		return JSTR_PTR_DIFF(jstr_memchrnul(s, *reject, sz), s);
	unsigned char t[256];
	JSTR_BZERO_ARRAY(t);
	const unsigned char *p = (const u *)reject;
	do
		t[*p] = 1;
	while (*p++);
	p = (u *)s + sz - 1;
	for (int i = 0, n = sz % 4;; ++i) {
		if (t[p[i]])
			return JSTR_PTR_DIFF(p + i, s);
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
	size_t cnt = JSTR_PTR_DIFF(p, s);
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
	const unsigned char *p = (const unsigned char *)s;
	p += jstr_memcspn(p, accept, sz);
	return *p ? (char *)p : NULL;
}

/* Return value:
   ptr to first non-C in S;
   ptr to '\0' if C is not found;
   S if C is '\0'; */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strchrnulinv(const char *s,
                  int c)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(c == '\0'))
		return (char *)s;
	while (*s++ == (char)c) {}
	return (char *)s - 1;
}

/* Return value:
   ptr to first non-C in S;
   NULL if non-C is not found. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
JSTR_ATTR_INLINE
static char *
jstr_strchrinv(const char *s,
               int c)
JSTR_NOEXCEPT
{
	s = jstr_strchrnulinv(s, c);
	return *s != (char)c ? (char *)s : NULL;
}

/* Return value:
   ptr to first non-C in S.
   S + N if C is not found. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memchrnulinv(const void *s,
                  int c,
                  size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s;
	for (; n-- && *p == (unsigned char)c; ++p) {}
	return (void *)p;
}

/* Return value:
   ptr to first non-C in S.
   NULL if C is not found. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memchrinv(const void *s,
               int c,
               size_t n)
JSTR_NOEXCEPT
{
	const void *const end = (const unsigned char *)s + n;
	s = jstr_memchrnulinv(s, c, n);
	return (s != end) ? (void *)s : NULL;
}

/* Return value:
   ptr to first non-C in S from S + N - 1
   NULL if C is not found. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memrchrinv(const void *s,
                int c,
                size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s + n - 1;
	for (; n && *p != (unsigned char)c; --n, --p) {}
	return n ? (char *)p : NULL;
}

/* Return value:
   ptr to first non-C in S from S + strlen(S) - 1
   NULL if C is not found. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_strrchrinv(const char *s,
                int c)
JSTR_NOEXCEPT
{
	return jstr_memrchrinv(s, c, strlen(s));
}

/* Check if S2 is in end of S1.
   Return value:
   1 if true;
   0 if false. */
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

/* Check if S2 is in end of S1.
   Return value:
   1 if true;
   0 if false. */
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static int
jstr_endscase(const char *hs,
              const char *ne)
JSTR_NOEXCEPT
{
	return jstr_endscase_len(hs, strlen(hs), ne, strlen(ne));
}

/* Check if S2 is in end of S1.
   Return value:
   1 if true;
   0 if false. */
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

/* Check if S2 is in end of S1.
   Return value:
   1 if true;
   0 if false. */
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static int
jstr_ends(const char *hs,
          const char *ne)
JSTR_NOEXCEPT
{
	return jstr_ends_len(hs, strlen(hs), ne, strlen(ne));
}

/* Check if S1 starts with S2 case-insensitively.
   Return value:
   1 if true;
   0 if false. */
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

/* Check if S1 starts with S2 case-insensitively.
   Return value:
   1 if true;
   0 if false. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_startscase(const char *hs,
                const char *ne)
JSTR_NOEXCEPT
{
	return (jstr_tolower(*hs) == jstr_tolower(*ne)) ? !jstr_strcasecmpeq_len(hs, ne, strlen(ne)) : (*ne == '\0');
}

/* Check if S1 starts with S2 case-insensitively.
   Return value:
   NELEN if true;
   0 if false. */
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

/* Check if S1 starts with S2.
   Return value:
   1 if true;
   0 if false. */
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

/* Check if S1 starts with S2.
   Return value:
   1 if true;
   0 if false. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_starts(const char *hs,
            const char *ne)
JSTR_NOEXCEPT
{
	return (*hs == *ne) ? !strncmp(hs, ne, strlen(ne)) : (*ne == '\0');
}

/* Count occurences of C in S.
   Return value:
   Occurences of C in S. */
JSTR_FUNC_PURE
static size_t
jstr_countchr(const char *s,
              int c)
JSTR_NOEXCEPT
{
	size_t cnt = 0;
	for (; (s = strchr(s, c)); ++s, ++cnt) {}
	return cnt;
}

/* Count occurences of C in S.
   Return value:
   Occurences of C in S. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_countchr_len(const char *s,
                  int c,
                  size_t sz)
JSTR_NOEXCEPT
{
	size_t cnt = 0;
	const char *const end = s + sz;
	for (; (s = (const char *)memchr(s, c, JSTR_PTR_DIFF(end, s))); ++s, ++cnt) {}
	return cnt;
}

/* Count occurences of NE in HS.
   Return value:
   occurences of NE in HS. */
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
	for (const char *const end = s + sz;
	     (s = jstr_strstr_len(s, JSTR_PTR_DIFF(end, s), find, find_len));
	     ++cnt, s += find_len)
		;
	return cnt;
}

/* Count occurences of NE in HS.
   Return value:
   occurences of NE in HS. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
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
	for (; (s = strstr(s, find)); ++cnt, s += find_len) {}
	return cnt;
}

/* Return value:
   ptr to startning of line;
   BEGIN if no newline was found. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_linestart(const char *const start,
               const char *end)
JSTR_NOEXCEPT
{
	end = (char *)jstr_memrchr(start, '\n', JSTR_PTR_DIFF(end, start));
	return (char *)(end ? end + 1 : start);
}

/* Return value:
   ptr to next line;
   NULL if no newline was found. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_linenext_len(const char *start,
                  const char *const end)
JSTR_NOEXCEPT
{
	start = (char *)memchr(start, '\n', JSTR_PTR_DIFF(end, start));
	return (start && *(start + 1)) ? (char *)start + 1 : NULL;
}

/* Return value:
   ptr to next line;
   NULL if no newline was found. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_linenext(const char *R s)
JSTR_NOEXCEPT
{
	s = strchr(s, '\n');
	return (s && *(s + 1)) ? (char *)s + 1 : NULL;
}

/* Return value:
   ptr to next line or '\0'. */
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

/* Return value:
   ptr to next line or '\0'. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_linenextnul(const char *R s)
JSTR_NOEXCEPT
{
	s = jstr_strchrnul(s, '\n');
	return (char *)((*s && *(s + 1)) ? s + 1 : s);
}

/* Return the number of newlines + 1.
   Return 0 if string is empty. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_linenumber(const char *start,
                const char *const end)
JSTR_NOEXCEPT
{
	size_t cnt = 1;
	for (; (start = (const char *)memchr(start, '\n', JSTR_PTR_DIFF(end, start))); ++start, ++cnt) {}
	return cnt;
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_revcpy_len(char *R dst,
                const char *R src,
                size_t src_len)
JSTR_NOEXCEPT
{
	src += src_len - 1;
	for (; src_len--; *dst++ = *src--) {}
	*dst = '\0';
}

/* DST must not overlap with SRC. */
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_revcpy_p(char *R dst,
              const char *R src)
{
	const size_t len = strlen(src);
	jstr_revcpy_len(dst, src, len);
	return dst + len;
}

/* Reverse S. */
JSTR_FUNC_VOID
static void
jstr_rev_len(char *R s,
             size_t sz)
JSTR_NOEXCEPT
{
	int c;
	char *e = s + sz - 1;
	while (s < e) {
		c = *s;
		*s++ = *e;
		*e-- = c;
	}
}

/* Reverse S.
   Return value:
   ptr to '\0' in S. */
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_rev_p(char *R s)
JSTR_NOEXCEPT
{
	const size_t _len = strlen(s);
	jstr_rev_len(s, _len);
	return s + _len;
}

/* Trim leading and trailing jstr_isspace() chars in S.
   Return value:
   ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
static char *
jstr_trimend_len_p(char *R s,
                   size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return s;
	char *end = jstr_skipspace_rev(s, s + sz - 1);
	*++end = '\0';
	return end;
}

/* Trim leading and trailing jstr_isspace() chars in S.
   Return value:
   ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_trimend_p(char *R s)
JSTR_NOEXCEPT
{
	return jstr_trimend_len_p(s, strlen(s));
}

/* Trim leading and trailing jstr_isspace() chars in S.
   Return value:
   ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
static char *
jstr_trimstart_len_p(char *R s,
                     size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return s;
	const char *const start = jstr_skipspace(s);
	if (s != start)
		return jstr_stpmove_len(s, start, JSTR_PTR_DIFF(s + sz, start));
	return s + sz;
}

/* Trim leading jstr_isspace() chars in S.
   Return value:
   ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_trimstart_p(char *R s)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return s;
	const char *const start = jstr_skipspace(s);
	if (s != start)
		return jstr_stpmove_len(s, start, strlen(start));
	return s + strlen(start);
}

/* Trim leading jstr_isspace() chars in S. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_trimstart(char *R s)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return;
	const char *const start = jstr_skipspace(s);
	if (s != start)
		jstr_strmove_len(s, start, strlen(start));
}

/* Trim leading and trailing jstr_isspace() chars in S.
   Return value:
   ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
static char *
jstr_trim_len_p(char *R s,
                size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return s;
	const char *const end = jstr_skipspace_rev(s, s + sz - 1) + 1;
	const char *const start = jstr_skipspace(s);
	if (start != s)
		return jstr_stpmove_len(s, start, JSTR_PTR_DIFF(end, start));
	return s + sz;
}

/* Trim leading and trailing jstr_isspace() chars in S.
   Return value:
   ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_trim_p(char *R s)
JSTR_NOEXCEPT
{
	return jstr_trim_len_p(s, strlen(s));
}

/* Place SRC into DST[AT].
   Assume that S have enough space for SRC.
   Return value: */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_place_len_unsafe(char *R s,
                      size_t at,
                      const char *R src,
                      size_t src_len)
JSTR_NOEXCEPT
{
	memcpy(s + at, src, src_len);
}

/* Place SRC into DST[AT].
   Return JSTR_RET_ERR on malloc error;
   otherwise JSTR_RET_SUCC. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_place_len(char *R *R s,
               size_t *R sz,
               size_t *R cap,
               size_t at,
               const char *R src,
               size_t src_len)
JSTR_NOEXCEPT
{
	if (at + src_len > *sz) {
		if (jstr_chk(jstr_reservealways(s, sz, cap, at + src_len)))
			return JSTR_RET_ERR;
		*sz = at + src_len;
		*(*s + *sz) = '\0';
	}
	jstr_place_len_unsafe(*s, at, src, src_len);
	return JSTR_RET_SUCC;
}

/* Place SRC after C in DST.
   Return JSTR_RET_ERR on malloc error;
   otherwise JSTR_RET_SUCC. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_placeafterchr_len(char *R *R s,
                       size_t *R sz,
                       size_t *R cap,
                       int c,
                       const char *R src,
                       size_t src_len)
JSTR_NOEXCEPT
{
	const char *const p = (char *)memchr(*s, c, *sz);
	if (p != NULL)
		return jstr_place_len(s, sz, cap, JSTR_PTR_DIFF(p, *s + 1), src, src_len);
	return JSTR_RET_SUCC;
}

/* Place SRC after end of NE in DST.
   Return JSTR_RET_ERR on malloc error;
   otherwise JSTR_RET_SUCC. */
JSTR_FUNC
static jstr_ret_ty
jstr_placeafter_len(char *R *R s,
                    size_t *R sz,
                    size_t *R cap,
                    const char *R find,
                    size_t find_len,
                    const char *R src,
                    size_t src_len)
JSTR_NOEXCEPT
{
	if (find_len == 1)
		return jstr_placeafterchr_len(s, sz, cap, *find, src, src_len);
	if (jstr_unlikely(find_len == 0))
		return JSTR_RET_SUCC;
	const char *const p = (char *)JSTR_REPLACE_SEARCHER(*s, *sz, find, find_len);
	if (p != NULL)
		return jstr_place_len(s, sz, cap, JSTR_PTR_DIFF(p, *s + find_len), src, src_len);
	return JSTR_RET_SUCC;
}

/* Convert snake_case to camelCase.
   Return ptr to '\0' in S.
   Leading underscores are preserved. */
JSTR_FUNC_RET_NONNULL
static char *
jstr_toCamelCaseP(char *R s)
JSTR_NOEXCEPT
{
	for (; *s == '_'; ++s) {}
	for (; *s && *s != '_'; ++s) {}
	if (jstr_unlikely(*s == '\0'))
		return s;
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *src = (const unsigned char *)s;
	goto start;
	for (; *src; ++src)
		if (jstr_likely(*src != '_'))
			*dst++ = *src;
		else {
start:
			*dst++ = jstr_toupper(*++src);
			if (jstr_unlikely(*src == '\0'))
				break;
		}
	*dst = '\0';
	return (char *)dst;
}

/* Convert snake_case to camelCase.
   Return ptr to '\0' in DST.
   Leading underscores are preserved. */
JSTR_FUNC_RET_NONNULL
static char *
jstr_toCamelCaseCpyP(char *R dst,
                     const char *R src)
JSTR_NOEXCEPT
{
	unsigned char *d = (unsigned char *)dst;
	const unsigned char *s = (const unsigned char *)src;
	for (; *s == '_'; ++s, *d++ = '_') {}
	while (*s)
		if (*s != '_') {
			*d++ = *s++;
		} else {
			if (jstr_unlikely(*++s == '\0'))
				break;
			*d++ = jstr_toupper(*s++);
		}
	*d = '\0';
	return (char *)d;
}

/* Convert camelCase to snake_case.
   Return ptr to '\0' in S.
   Leading underscores are preserved. */
JSTR_FUNC_RET_NONNULL
static char *
jstr_to_snake_case_p(char *R s)
JSTR_NOEXCEPT
{
	unsigned char *p = (unsigned char *)s;
	for (; *p == '_'; ++p) {}
	*p = jstr_tolower(*p);
	for (; *p && !jstr_isupper(*p); ++p) {}
	if (jstr_unlikely(*p == '\0'))
		return (char *)p;
	const unsigned char *end = p + strlen((char *)p);
	goto start;
	for (; *p; ++p)
		if (jstr_isupper(*p)) {
start:
			jstr_strmove_len((char *)p + 1, (char *)p, JSTR_PTR_DIFF(end++, p));
			*p++ = '_';
			*p = jstr_tolower(*p);
		}
	*p = '\0';
	return (char *)p;
}

/* Convert camelCase to snake_case.
   Return ptr to '\0' in DST.
   Leading underscores are preserved. */
JSTR_FUNC_RET_NONNULL
static char *
jstr_to_snake_case_cpy_p(char *R dst,
                         const char *R src)
JSTR_NOEXCEPT
{
	unsigned char *d = (unsigned char *)dst;
	const unsigned char *s = (const unsigned char *)src;
	for (; *s == '_'; ++s, *d++ = '_') {}
	*d = jstr_tolower(*s);
	while (*s)
		if (!jstr_isupper(*s)) {
			*d++ = *s++;
		} else {
			*d = '_';
			*(d + 1) = jstr_tolower(*s++);
			d += 2;
		}
	*d = '\0';
	return (char *)d;
}

/* Non-destructive strtok.
   END must be NUL terminated.
   Instead of nul-termination, use the save_ptr to know the length of the string. */
JSTR_FUNC_PURE
static char *
jstr_strtok_ne_len(const char **R const save_ptr,
                   const char *R const end,
                   const char *R ne,
                   size_t ne_len)
JSTR_NOEXCEPT
{
	const char *const s = *save_ptr;
	if (jstr_unlikely(*s == '\0'))
		return NULL;
	*save_ptr = jstr_strstrnul_len(s, JSTR_PTR_DIFF(end, s), ne, ne_len);
	if (jstr_likely(**save_ptr != '\0'))
		*save_ptr += ne_len;
	return (char *)s;
}

/* Non-destructive strtok.
   Instead of nul-termination, use the save_ptr to know the length of the string. */
JSTR_FUNC_PURE
static char *
jstr_strtok_ne(const char **R const save_ptr,
               const char *R ne)
JSTR_NOEXCEPT
{
	const char *const s = *save_ptr;
	if (jstr_unlikely(*s == '\0'))
		return NULL;
	*save_ptr = jstr_strstrnul(s, ne);
	if (jstr_likely(**save_ptr != '\0'))
		*save_ptr += strlen(ne);
	return (char *)s;
}

/* Non-destructive strtok.
   Instead of nul-termination, use the save_ptr to know the length of the string. */
JSTR_FUNC_PURE
static char *
jstr_strtok(const char **R save_ptr,
            const char *R delim)
JSTR_NOEXCEPT
{
	const char *s = *save_ptr;
	if (jstr_unlikely(*s == '\0'))
		return NULL;
	s += strspn(s, delim);
	if (jstr_unlikely(*s == '\0')) {
		*save_ptr = s;
		return NULL;
	}
	*save_ptr = s + strcspn(s, delim);
	return (char *)s;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_cpy_p(char *R dst,
           const jstr_ty *R src)
JSTR_NOEXCEPT
{
	return jstr_stpcpy_len(dst, src->data, src->size);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_dup(jstr_ty *R dst,
         const jstr_ty *R src)
JSTR_NOEXCEPT
{
	dst->data = (char *)malloc(src->capacity);
	if (jstr_nullchk(dst->data))
		return JSTR_RET_ERR;
	dst->size = JSTR_PTR_DIFF(jstr_cpy_p(dst->data, dst), dst->data);
	dst->size = src->size;
	dst->capacity = src->capacity;
	return JSTR_RET_SUCC;
}

/* Return ptr to '\0' in DST. */
JSTR_FUNC
static char *
jstr_repeat_len_unsafe_p(char *s,
                         size_t sz,
                         size_t n)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(n < 2))
		return s + sz;
	--n;
	if (jstr_likely(sz > 1))
		while (n--)
			s = (char *)jstr_mempmove(s + sz, s, sz);
	else if (sz == 1)
		s = (char *)memset(s, *s, n) + n;
	*s = '\0';
	return s;
}

/* Return value:
   JSTR_RET_ERR on error;
   JSTR_RET_SUCC otherwise. */
JSTR_FUNC
static jstr_ret_ty
jstr_repeat_len(char *R *R s,
                size_t *R sz,
                size_t *R cap,
                size_t n)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(n <= 1))
		return JSTR_RET_SUCC;
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz * n)))
		return JSTR_RET_ERR;
	*sz = JSTR_PTR_DIFF(jstr_repeat_len_unsafe_p(*s, *sz, n), *s);
	return JSTR_RET_SUCC;
}

/* Return ptr to '\0' in DST. */
JSTR_FUNC
static char *
jstr_repeatcpy_len_p(char *R dst,
                     const char *R src,
                     size_t src_len,
                     size_t n)
JSTR_NOEXCEPT
{
	if (jstr_likely(src_len > 1))
		while (n--)
			dst = (char *)jstr_mempcpy(dst, src, src_len);
	else if (src_len == 1)
		dst = (char *)memset(dst, *src, n) + n;
	*dst = '\0';
	return dst;
}

/* Add thousand separator to NPTR.
   Return value:
   ptr to '\0' in NPTR.
   For example: 1234 becomes 1,234. */
JSTR_FUNC_RET_NONNULL
static char *
jstr_thousep_len_p(char *R nptr,
                   size_t sz,
                   char separator)
JSTR_NOEXCEPT
{
	char *end = nptr + sz;
	if (*nptr == '-') {
		++nptr;
		--sz;
	} else if (jstr_unlikely(sz == 0))
		return nptr;
	if (sz < 4)
		return end;
	size_t dif = (sz - 1) / 3;
	end += dif;
	const char *const start = nptr;
	nptr += (sz - 1);
	int n;
	for (n = 0; nptr >= start;) {
		*(nptr + dif) = *nptr;
		--nptr;
		if (++n == 3) {
			*(nptr + dif) = (char)separator;
			if (jstr_unlikely(--dif == 0))
				break;
			n = 0;
		}
	}
	*end = '\0';
	return (char *)end;
}

/* Add thousand separator to NPTR.
   Return value:
   ptr to '\0' in NPTR.
   For example: 1234 becomes 1,234. */
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_thousep_p(char *R nptr,
               char separator)
JSTR_NOEXCEPT
{
	return jstr_thousep_len_p(nptr, strlen(nptr), separator);
}

/* Copy SRC to DST, adding thousand separator.
   Return value:
   ptr to '\0' in DST. */
JSTR_ATTR_ACCESS((__read_only__, 2, 3))
JSTR_FUNC_RET_NONNULL
static char *
jstr_thousepcpy_len_p(char *R dst,
                      const char *R src,
                      size_t src_len,
                      char separator)
JSTR_NOEXCEPT
{
	if (*src == '-') {
		*dst++ = '-';
		++src;
		--src_len;
	}
	if (src_len < 4) {
		while ((*dst++ = *src++)) {}
		return dst - 1;
	}
	int i = src_len % 3;
	for (int j = i; j--; *dst++ = *src++) {}
	if (i) {
		*dst++ = separator;
		i = 0;
	}
	for (; *src; ++i) {
		if (i == 3) {
			*dst = separator;
			*(dst + 1) = *src++;
			dst += 2;
			i = 0;
		} else {
			*dst++ = *src++;
		}
	}
	*dst = '\0';
	return dst;
}

/* Copy SRC to DST, adding thousand separator.
   Return value:
   ptr to '\0' in DST. */
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_thousepcpy_p(char *R dst,
                  const char *R src,
                  char separator)
JSTR_NOEXCEPT
{
	return jstr_thousepcpy_len_p(dst, src, strlen(src), separator);
}

PJSTR_END_DECLS

#undef R

#endif /* JSTR_STRING_H */
