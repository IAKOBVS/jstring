/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2024 James Tirta Halim <tirtajames45 at gmail dot com>
 * This file is part of the jstring library.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#ifndef JSTR_STRING_H
#define JSTR_STRING_H 1

#include "jstr-macros.h"

JSTR__BEGIN_DECLS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
JSTR__END_DECLS

#include "jstr-config.h"
#include "jstr-stdstring.h"
#include "jstr-ctype.h"
#include "_musl.h"

#if defined __AVX512BW__ || defined __AVX2__ || defined __SSE2__
#	define JSTR_HAVE_SIMD 1
#	include "_simd.h"
#endif

#define R JSTR_RESTRICT

JSTR__BEGIN_DECLS

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memrchr(const void *s, int c, size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMRCHR && !JSTR_TEST
	return (void *)memrchr(s, c, n);
#elif JSTR_HAVE_SIMD && !JSTR_HAVENT_MEMRCHR_SIMD
	return jstr__simd_memrchr(s, c, n);
#else
	return jstr__memrchr_musl(s, c, n);
#endif
}

JSTR_FUNC_PURE
JSTR_ATTR_RETURNS_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_strchrnul(const char *s, int c)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL && !JSTR_TEST
	return (char *)strchrnul(s, c);
#elif JSTR_HAVE_SIMD && !JSTR_HAVENT_STRCHRNUL_SIMD
	return jstr__simd_strchrnul(s, c);
#else
	char *const p = strchr(s, c);
	return p ? p : (char *)s + strlen(s);
#endif
}

/* Return value:
 * ptr to first C in S ignoring case;
 * NULL if not found. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strcasechrnul(const char *s, int c)
JSTR_NOEXCEPT
{
	if (!jstr_isalpha(c))
		return jstr_strchrnul(s, c);
#if JSTR_HAVE_SIMD && !JSTR_HAVENT_STRCASECHRNUL_SIMD
	return jstr__simd_strcasechrnul(s, c);
#elif JSTR_HAVE_STRCSPN_OPTIMIZED
	c = jstr_tolower(c);
	if (jstr_tolower(*s) != c) {
		const char a[] = { (char)c, (char)jstr_toupper(c), '\0' };
		s += strcspn(s, a);
	}
	return (char *)s;
#else
	return jstr__strcasechrnul_musl(s, c);
#endif
}

/* Return value:
 * ptr to first C in S ignoring case;
 * NULL if not found. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strcasechr(const char *s, int c)
JSTR_NOEXCEPT
{
	if (!jstr_isalpha(c))
		return (char *)strchr(s, c);
	c = jstr_tolower(c);
#if JSTR_HAVE_SIMD && !JSTR_HAVENT_STRCASECHRNUL_SIMD
	s = jstr__simd_strcasechrnul(s, c);
#elif JSTR_HAVE_STRCSPN_OPTIMIZED
	if (jstr_tolower(*s) != c) {
		const char a[] = { (char)c, (char)jstr_toupper(c), '\0' };
		s += strcspn(s, a);
	}
#else
	s = jstr__strcasechrnul_musl(s, c);
#endif
	return jstr_tolower(*s) == (char)c ? (char *)s : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memcasechr(const void *s, int c, size_t n)
JSTR_NOEXCEPT
{
	if (!jstr_isalpha(c))
		return (char *)memchr(s, c, n);
#if JSTR_HAVE_SIMD && !JSTR_HAVENT_MEMCASECHR_SIMD
	return jstr__simd_memcasechr(s, c, n);
#else
	return jstr__memcasechr_musl(s, c, n);
#endif
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memrchrnul(const void *s, int c, size_t sz)
JSTR_NOEXCEPT
{
	const void *const p = jstr_memrchr(s, c, sz);
	return (void *)(p ? p : (char *)s + sz);
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memchrnul(const void *s, int c, size_t sz)
JSTR_NOEXCEPT
{
	const void *const p = memchr(s, c, sz);
	return (void *)(p ? p : (char *)s + sz);
}

/* Return value:
 * ptr to '\0' in DST. */
JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_stpcpy(char *R dst, const char *R src)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STPCPY && !JSTR_TEST
	return stpcpy(dst, src);
#else
	return jstr_stpcpy_len(dst, src, strlen(src));
#endif
}

/* Return value:
 * Ptr to '\0' in DST. */
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_stpcat(char *R dst, const char *R src)
JSTR_NOEXCEPT
{
	dst += strlen(dst);
	return jstr_stpcpy(dst, src);
}

/* strchr() before s + N. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strnchr(const char *s, int c, size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_SIMD && !JSTR_HAVENT_STRNCHR_SIMD
	return jstr__simd_strnchr(s, c, n);
#else
	return jstr__strnchr_musl(s, c, n);
#endif
}

/* strcasechr() before s + N. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strncasechr(const char *s, int c, size_t n)
JSTR_NOEXCEPT
{
	if (!jstr_isalpha(c))
		return jstr_strnchr(s, c, n);
#if JSTR_HAVE_SIMD && !JSTR_HAVENT_STRNCASECHR_SIMD
	return jstr__simd_strncasechr(s, c, n);
#else
	return jstr__strncasechr_musl(s, c, n);
#endif
}

/* basename() for non nul-terminated strings. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_basename_len(const char *fname, size_t sz)
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
jstr_strstrnul(const char *hs, const char *ne)
JSTR_NOEXCEPT
{
	if (*ne && ne[1]) {
		const char *const p = strstr(hs, ne);
		return (char *)(p ? p : hs + strlen(hs));
	} else {
		return jstr_strchrnul(hs, *ne);
	}
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_cmpeq(const void *s1, size_t s1_len, const void *s2, size_t s2_len)
JSTR_NOEXCEPT
{
	return (s1_len == s2_len) ? memcmp(s1, s2, s1_len) : 1;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_cmpcaseeq(const char *s1, size_t s1_len, const char *s2, size_t s2_len)
JSTR_NOEXCEPT
{
	return (s1_len == s2_len) ? jstr_strcasecmpeq(s1, s2) : 1;
}

/* Check if S2 is in end of S1.
 * Return value:
 * 1 if true;
 * 0 if false. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static int
jstr_endscase_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	return jstr_likely(hs_len >= ne_len) ? !jstr_strcasecmpeq_len(hs + hs_len - ne_len, ne, ne_len) : 0;
}

/* Check if S2 is in end of S1.
 * Return value:
 * 1 if true;
 * 0 if false. */
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static int
jstr_endscase(const char *hs, const char *ne)
JSTR_NOEXCEPT
{
	return jstr_endscase_len(hs, strlen(hs), ne, strlen(ne));
}

/* Check if S2 is in end of S1.
 * Return value:
 * 1 if true;
 * 0 if false. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static int
jstr_ends_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	return jstr_likely(hs_len >= ne_len) ? !memcmp(hs + hs_len - ne_len, ne, ne_len) : 0;
}

/* Check if S2 is in end of S1.
 * Return value:
 * 1 if true;
 * 0 if false. */
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static int
jstr_ends(const char *hs, const char *ne)
JSTR_NOEXCEPT
{
	return jstr_ends_len(hs, strlen(hs), ne, strlen(ne));
}

/* Check if S1 starts with S2 case-insensitively.
 * Return value:
 * 1 if true;
 * 0 if false. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_startscase_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	return jstr_likely(hs_len >= ne_len) ? !jstr_strcasecmpeq_len(hs, ne, ne_len) : 0;
}

/* Check if S1 starts with S2 case-insensitively.
 * Return value:
 * 1 if true;
 * 0 if false. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_startscase(const char *hs, const char *ne)
JSTR_NOEXCEPT
{
	return (jstr_tolower(*hs) == jstr_tolower(*ne))
	       ? !jstr_strcasecmpeq_len(hs, ne, strlen(ne))
	       : (*ne == '\0');
}

/* Check if S1 starts with S2 case-insensitively.
 * Return value:
 * NELEN if true;
 * 0 if false. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_startscasenul(const char *hs, const char *ne)
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
 * Return value:
 * 1 if true;
 * 0 if false. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_starts_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	return jstr_likely(hs_len >= ne_len) ? !memcmp(hs, ne, ne_len) : 0;
}

/* Check if S1 starts with S2.
 * Return value:
 * 1 if true;
 * 0 if false. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_starts(const char *hs, const char *ne)
JSTR_NOEXCEPT
{
	return (*hs == *ne) ? !strncmp(hs, ne, strlen(ne)) : (*ne == '\0');
}

#define JSTR__STRSTR234_MEMMEM    1
#define JSTR__STRSTR234_FUNC_NAME jstr__memmem
#include "_strstr-lt8.h"

#define JSTR__STRSTR234_CANON     jstr_tolower
#define JSTR__STRSTR234_FUNC_NAME jstr__strcasestr
#include "_strstr-lt8.h"

#define JSTR__STRSTR234_MEMMEM    1
#define JSTR__STRSTR234_CANON     jstr_tolower
#define JSTR__STRSTR234_FUNC_NAME jstr__memcasemem
#include "_strstr-lt8.h"

#define JSTR__MUSL_FUNC_NAME jstr__memmem_musl
#include "_musl-twoway.h"

#if JSTR_HAVE_MEMMEM && JSTR_HAVE_MEMMEM_OPTIMIZED && !JSTR_TEST
#	define JSTR_USE_MEMMEM_LIBC 1
#else
#	define JSTR_USE_MEMMEM_LIBC 0
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
#if JSTR_USE_MEMMEM_LIBC || !JSTR_HAVENT_MEMMEM_SIMD
JSTR_ATTR_INLINE
#endif
static void *
jstr_memmem(const void *hs, size_t hs_len, const void *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	enum { LONG_NE_THRES = 64 };
#if JSTR_USE_MEMMEM_LIBC
	return memmem(hs, hs_len, ne, ne_len);
#elif JSTR_HAVE_SIMD && !JSTR_HAVENT_MEMMEM_SIMD
	if (jstr_likely(ne_len <= sizeof(jstr_vvec_ty)))
		return jstr__simd_memmem(hs, hs_len, ne, ne_len);
	return (hs_len >= ne_len)
	       ? jstr__memmem_musl((const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len)
	       : NULL;
#else
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	if (ne_len <= 4) {
		if (jstr_unlikely(ne_len == 0))
			return (char *)hs;
		const void *start = hs;
		hs = memchr((const unsigned char *)hs, *(char *)ne, hs_len - ne_len + 1);
		if (jstr_unlikely(hs == NULL) || ne_len == 1)
			return (void *)hs;
		hs_len -= JSTR_DIFF(hs, start);
		return jstr__memmem_lt8((const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len);
	}
	return jstr__memmem_musl((const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len);
#endif
}

/* Needles <= THRES will not compile a jstr_twoway_ty. */
#if JSTR_HAVE_SIMD && !JSTR_HAVENT_MEMMEM_SIMD
#	define JSTR_TWOWAY_MEMMEM_THRES sizeof(jstr_vvec_ty)
#else
#	define JSTR_TWOWAY_MEMMEM_THRES 2
#endif
#define JSTR_TWOWAY_STRSTR_THRES         JSTR_TWOWAY_MEMMEM_THRES
#define JSTR_TWOWAY_STRCASESTR_LEN_THRES 2
#define JSTR_TWOWAY_STRCASESTR_THRES     JSTR_TWOWAY_STRCASESTR_LEN_THRES

JSTR_FUNC_VOID
static void
jstr_memmem_comp(jstr_twoway_ty *const t, const void *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	if (ne_len > JSTR_TWOWAY_MEMMEM_THRES)
		jstr__memmem_musl_comp(t, (const unsigned char *)ne, ne_len);
	else
		t->needle_len = ne_len;
}

JSTR_ATTR_ACCESS((__read_only__, 2, 3))
JSTR_FUNC_PURE
static void *
jstr_memmem_exec(const jstr_twoway_ty *const t, const void *hs, size_t hs_len, const void *ne)
JSTR_NOEXCEPT
{
	if (t->needle_len > JSTR_TWOWAY_MEMMEM_THRES)
		return jstr__memmem_musl_exec(t, (const unsigned char *)hs, hs_len, (const unsigned char *)ne);
#if JSTR_HAVE_SIMD && !JSTR_HAVENT_MEMMEM_SIMD
	return jstr__simd_memmem(hs, hs_len, ne, t->needle_len);
#else
	if (t->needle_len == 2) {
		if (jstr_unlikely(hs_len < 2))
			return NULL;
		return jstr__memmem2((const unsigned char *)hs, hs_len, (const unsigned char *)ne);
	}
	if (t->needle_len == 1)
		return (void *)memchr(hs, *(const unsigned char *)ne, hs_len);
	/* t->needle_len == 0 */
	return (char *)hs;
#endif
}

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strstr_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	return (char *)jstr_memmem(hs, hs_len, ne, ne_len);
}

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strnstr_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len, size_t n)
JSTR_NOEXCEPT
{
	return (char *)jstr_memmem(hs, JSTR_MIN(hs_len, n), ne, ne_len);
}

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memnmem(const void *hs, size_t hs_len, const void *ne, size_t ne_len, size_t n)
JSTR_NOEXCEPT
{
	return jstr_memmem(hs, JSTR_MIN(hs_len, n), ne, ne_len);
}

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memmemnul(const void *hs, size_t hs_len, const void *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	hs = jstr_memmem(hs, hs_len, ne, ne_len);
	return (void *)(hs ? hs : (char *)hs + hs_len);
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strstrnul_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	return (char *)jstr_memmemnul(hs, hs_len, ne, ne_len);
}

#define JSTR__STRSTR234_FUNC_NAME jstr__memrmem
#define JSTR__STRSTR234_MEMRMEM   1
#include "_strstr-lt8.h"

/* Find last NE in HS.
 * Return value:
 * Ptr to NE;
 * NULL if not found.
 * The current implementation is O(m * n). */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
static void *
jstr_memrmem(const void *hs, size_t hs_len, const void *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(ne_len == 0))
		return (char *)hs + hs_len;
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	const unsigned char *p = (const unsigned char *)
	jstr_memrchr((const unsigned char *)hs, *(const unsigned char *)ne, hs_len - ne_len + 1);
	if (p == NULL || ne_len == 1)
		return (void *)p;
	hs_len = JSTR_DIFF((char *)p + ne_len, hs);
	/* Use 8more for ne_len == 8 because it's faster (avoids masking). */
	if (ne_len < 8)
		return jstr__memrmem_lt8((const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len);
	return jstr__memrmem_8more((const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len);
}

/* Find last NE in HS.
 * Return value:
 * Ptr to NE;
 * NULL if not found.
 * The current implementation is O(m * n). */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
static char *
jstr_strrstr_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	return (char *)jstr_memrmem(hs, hs_len, ne, ne_len);
}

#if JSTR_HAVE_MEMMEM_OPTIMIZED || JSTR_HAVE_STRSTR_OPTIMIZED \
|| (JSTR_HAVE_SIMD && !JSTR_HAVENT_STRCASESTR_LEN_SIMD)
#	define JSTR_USE_MEMMEM_OPTIMIZED 1
#else
#	define JSTR_USE_MEMMEM_OPTIMIZED 0
#endif

#define JSTR__MUSL_FUNC_NAME jstr__strcasestr_len_musl
#define JSTR__MUSL_CANON     jstr_tolower
#define JSTR__MUSL_CMP_FUNC  jstr_strcasecmpeq_len
#include "_musl-twoway.h"

/* Find NE in HS case-insensitively (ASCII).
 * Return value:
 * Ptr to NE;
 * NULL if not found. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
static char *
jstr_strcasestr_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	enum { LONG_NE_THRES = 64 };
#if !JSTR_TEST
	for (size_t i = 0;; ++i) {
		if (i == ne_len)
			return (char *)jstr_memmem(hs, hs_len, ne, ne_len);
		if (jstr_isalpha(ne[i]))
			break;
	}
#endif
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	if (ne_len <= 8) {
		if (jstr_unlikely(ne_len == 0))
			return (char *)hs;
		const char *start = hs;
		hs = (const char *)jstr_memcasechr(hs, *ne, hs_len - ne_len + 1);
		if (jstr_unlikely(hs == NULL) || ne_len == 1)
			return (char *)hs;
		hs_len -= JSTR_DIFF(hs, start);
		return jstr__memcasemem_lt8((const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len);
	}
	return jstr__strcasestr_len_musl((const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len);
}

#if JSTR_TWOWAY_STRCASESTR_LEN_THRES > 2
#	error "JSTR_TWOWAY_STRCASESTR_LEN_THRES is assumed to be 2."
#endif

JSTR_FUNC_VOID
static void
jstr_strcasestr_len_comp(jstr_twoway_ty *const t, const char *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	if (ne_len > JSTR_TWOWAY_STRCASESTR_LEN_THRES)
		jstr__strcasestr_len_musl_comp(t, (const unsigned char *)ne, ne_len);
	else
		t->needle_len = ne_len;
}

JSTR_ATTR_ACCESS((__read_only__, 2, 3))
JSTR_FUNC_PURE
static char *
jstr_strcasestr_len_exec(const jstr_twoway_ty *const t, const char *hs, size_t hs_len, const char *ne)
JSTR_NOEXCEPT
{
	if (t->needle_len > JSTR_TWOWAY_STRCASESTR_LEN_THRES)
		return jstr__strcasestr_len_musl_exec(t, (const unsigned char *)hs, hs_len, (const unsigned char *)ne);
	if (t->needle_len == 2) {
		if (jstr_unlikely(hs_len < 2))
			return NULL;
		return jstr__memcasemem2((const unsigned char *)hs, hs_len, (const unsigned char *)ne);
	}
	if (t->needle_len == 1)
		return (char *)jstr_memcasechr(hs, *(const unsigned char *)ne, hs_len);
	/* t->needle_len == 0 */
	return (char *)hs;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strncasestr_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len, size_t n)
JSTR_NOEXCEPT
{
	return jstr_strcasestr_len(hs, JSTR_MIN(hs_len, n), ne, ne_len);
}

#define JSTR__MUSL_FUNC_NAME jstr__strcasestr_musl
#define JSTR__MUSL_CANON     jstr_tolower
#define JSTR__MUSL_CMP_FUNC  jstr_strcasecmpeq_len
#define JSTR__MUSL_CHECK_EOL 1
#include "_musl-twoway.h"

/* Find NE in HS case-insensitively.
 * Return value:
 * Ptr to NE;
 * NULL if not found. */
JSTR_FUNC_PURE
#if JSTR_HAVE_STRCASESTR_OPTIMIZED
JSTR_ATTR_INLINE
#endif
static char *
jstr_strcasestr(const char *hs, const char *ne)
JSTR_NOEXCEPT
{
	const unsigned char *np;
#if !JSTR_TEST
	for (np = (const unsigned char *)ne;; ++np) {
		if (*np == '\0')
			return (char *)strstr(hs, ne);
		if (jstr_isalpha(*np))
			break;
	}
#else
	if (jstr_unlikely(*ne == '\0'))
		return (char *)hs;
#endif
#if JSTR_HAVE_STRCASESTR_OPTIMIZED
	return (char *)strcasestr(hs, ne);
#else
	hs = jstr_strcasechr(hs, *ne);
	if (jstr_unlikely(hs == NULL) || ne[1] == '\0')
		return (char *)hs;
	unsigned int ne_len;
	for (ne_len = 2; ne_len < 8 + 1 && ne[ne_len]; ne_len++) {}
	if (ne_len <= 8)
		return jstr__strcasestr_lt8((const unsigned char *)hs, (const unsigned char *)ne, ne_len);
	if (jstr_tolower(hs[1]) == jstr_tolower(ne[1])) {
		np = (const unsigned char *)ne + 2;
		const unsigned char *hp = (const unsigned char *)hs + 2;
		for (;;) {
			if (*np == '\0')
				return (char *)hs;
			if (jstr_unlikely(*hp == '\0'))
				return NULL;
			if (jstr_tolower(*hp++) != jstr_tolower(*np++))
				break;
		}
	}
	return jstr__strcasestr_musl((const unsigned char *)hs, (const unsigned char *)ne);
#endif
}

#if JSTR_TWOWAY_STRCASESTR_THRES > 2
#	error "JSTR_TWOWAY_STRCASESTR_THRES is assumed to be 2."
#endif

JSTR_FUNC_VOID
static void
jstr_strcasestr_comp(jstr_twoway_ty *const t, const char *ne)
JSTR_NOEXCEPT
{
	if (*ne == '\0')
		t->needle_len = 0;
	else if (ne[1] == '\0')
		t->needle_len = 1;
	else if (ne[2] == '\0')
		t->needle_len = 2;
	else
		jstr__strcasestr_musl_comp(t, (const unsigned char *)ne);
}

JSTR_FUNC_PURE
static char *
jstr_strcasestr_exec(const jstr_twoway_ty *const t, const char *hs, const char *ne)
JSTR_NOEXCEPT
{
	if (t->needle_len > JSTR_TWOWAY_STRCASESTR_THRES)
		return jstr__strcasestr_musl_exec(t, (const unsigned char *)hs, (const unsigned char *)ne);
	if (t->needle_len == 2)
		return jstr__strcasestr2((const unsigned char *)hs, (const unsigned char *)ne);
	if (t->needle_len == 1)
		return (char *)jstr_strcasechr(hs, *ne);
	/* t->needle_len == 0 */
	return (char *)hs;
}

#define JSTR__MUSL_FUNC_NAME jstr__strstr_musl
#define JSTR__MUSL_CHECK_EOL 1
#include "_musl-twoway.h"

JSTR_FUNC_VOID
static void
jstr_strstr_comp(jstr_twoway_ty *const t, const char *ne)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*ne == '\0')) {
		t->needle_len = 0;
		return;
	} else if (jstr_unlikely(ne[1] == '\0')) {
		t->needle_len = 1;
		return;
	} else if (jstr_unlikely(ne[2] == '\0')) {
		t->needle_len = 2;
		return;
	}
	jstr__strstr_musl_comp(t, (const unsigned char *)ne);
}

JSTR_FUNC_PURE
static char *
jstr_strstr_exec(const jstr_twoway_ty *const t, const char *hs, const char *ne)
JSTR_NOEXCEPT
{
	if (t->needle_len <= JSTR_TWOWAY_STRSTR_THRES)
		return (char *)strstr(hs, ne);
	return jstr__strstr_musl_exec(t, (const unsigned char *)hs, (const unsigned char *)ne);
}

/* Reverse of STRCSPN.
 * Return the offset from S if found;
 * otherwise, return S + SZ. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static size_t
jstr_strrcspn_len(const char *s, const char *reject, size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(reject[0] == '\0') || jstr_unlikely(sz == 0))
		return sz;
	if (jstr_unlikely(reject[1] == '\0')) {
		const char *const p = (const char *)jstr_memrchr(s, *reject, sz);
		return p ? JSTR_DIFF(p, s) : sz;
	}
	unsigned char t[256];
	JSTR_BZERO_ARRAY(t);
	const unsigned char *p = (const unsigned char *)reject;
	do
		t[*p] = 1;
	while (*p++);
	p = (const unsigned char *)s + sz - 1;
	int i = 0;
	int n = sz % 4;
	for (;; --i) {
		if (t[p[i]])
			return JSTR_DIFF(p + i, s);
		if (--n == 0) {
			if (sz < 4)
				return sz;
			break;
		}
	}
	p = (const unsigned char *)JSTR_PTR_ALIGN_UP(p, 4);
	unsigned int c0, c1, c2, c3;
	do {
		c0 = t[p[0]];
		c1 = t[p[-1]];
		c2 = t[p[-2]];
		c3 = t[p[-3]];
		p -= 4;
	} while (((p < (const unsigned char *)s) | c0 | c1 | c2 | c3) == 0);
	size_t cnt = JSTR_DIFF(s + sz, p);
	cnt = sz - (((c0 | c1) != 0) ? cnt - c0 + 1 : cnt - c2 + 3);
	return (cnt < sz) ? cnt : sz;
}

/* Reverse of STRCSPN. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_strrcspn(const char *s, const char *reject)
JSTR_NOEXCEPT
{
	return jstr_strrcspn_len(s, reject, strlen(s));
}

/* Reverse of STRSPN.
 * Return the offset from S if found;
 * otherwise, return S + SZ. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static size_t
jstr_strrspn_len(const char *s, const char *accept, size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*accept == '\0') || jstr_unlikely(sz == 0))
		return sz;
	if (jstr_unlikely(accept[1] == '\0')) {
		const char *p = s + sz - 1;
		const int c = *accept;
		for (; sz-- && *p == c; --p) {}
		return JSTR_DIFF(p, s);
	}
	const unsigned char *p = (const unsigned char *)accept;
	unsigned char t[256];
	JSTR_BZERO_ARRAY(t);
	do
		t[*p++] = 1;
	while (*p);
	p = (const unsigned char *)s + sz - 1;
	int i = 0;
	int n = sz % 4;
	for (;; --i) {
		if (!t[p[i]])
			return JSTR_DIFF(p + i, s);
		if (--n == 0) {
			if (sz < 4)
				return sz;
			break;
		}
	}
	p = (const unsigned char *)JSTR_PTR_ALIGN_DOWN(p, 4);
	unsigned int c0, c1, c2, c3;
	do {
		c0 = t[p[0]];
		c1 = t[p[-1]];
		c2 = t[p[-2]];
		c3 = t[p[-3]];
		p -= 4;
	} while ((p >= (const unsigned char *)s) & (c0 & c1 & c2 & c3));
	size_t cnt = JSTR_DIFF(s + sz, p);
	cnt = (sz - (((c0 & c1) == 0) ? cnt + c0 : cnt + c2 + 2));
	return (cnt < sz) ? cnt : sz;
}

/* Reverse of STRSPN.
 * Return the offset from S if found;
 * otherwise, return S + SZ. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_strrspn(const char *s, const char *accept)
JSTR_NOEXCEPT
{
	return jstr_strrspn_len(s, accept, strlen(s));
}

/* Reverse of STRPBRK. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strrpbrk_len(const char *s, const char *accept, size_t sz)
JSTR_NOEXCEPT
{
	const size_t len = jstr_strrcspn_len(s, accept, sz);
	return (len != sz) ? (char *)(s + sz - len) : NULL;
}

/* Reverse of STRPBRK. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strrpbrk(const char *s, const char *accept)
JSTR_NOEXCEPT
{
	return jstr_strrpbrk_len(s, accept, strlen(s));
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static size_t
jstr_memspn(const void *s, const char *accept, size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*accept == '\0') || jstr_unlikely(sz == 0))
		return sz;
	if (jstr_unlikely(accept[1] == '\0')) {
		const char *p = (char *)s;
		const int c = *accept;
		for (; sz-- && *p == c; ++p) {}
		return JSTR_DIFF(p, s);
	}
	const unsigned char *p = (const unsigned char *)accept;
	unsigned char t[256];
	JSTR_BZERO_ARRAY(t);
	do
		t[*p++] = 1;
	while (*p);
	p = (const unsigned char *)s;
	int i = 0;
	int n = sz % 4;
	for (;; ++i) {
		if (!t[p[i]])
			return JSTR_DIFF(p + i, s);
		if (--n == 0) {
			if (sz < 4)
				return sz;
			break;
		}
	}
	p = (const unsigned char *)JSTR_PTR_ALIGN_UP(p, 4);
	const unsigned char *const end = (const unsigned char *)s + sz;
	unsigned int c0, c1, c2, c3;
	do {
		c0 = t[p[0]];
		c1 = t[p[1]];
		c2 = t[p[2]];
		c3 = t[p[3]];
		p += 4;
	} while ((p < end) & (c0 & c1 & c2 & c3));
	size_t cnt = p - (const unsigned char *)p;
	cnt = (sz - (((c0 & c1) == 0) ? cnt + c0 : cnt + c2 + 2));
	return (cnt < sz) ? cnt : sz;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static size_t
jstr_memcspn(const void *s, const char *reject, size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*reject == '\0') || jstr_unlikely(sz == 0))
		return sz;
	if (jstr_unlikely(reject[1] == '\0'))
		return JSTR_DIFF(jstr_memchrnul(s, *reject, sz), s);
	unsigned char t[256];
	JSTR_BZERO_ARRAY(t);
	const unsigned char *p = (const unsigned char *)reject;
	do
		t[*p] = 1;
	while (*p++);
	p = (const unsigned char *)s + sz - 1;
	int i = 0;
	int n = sz % 4;
	for (;; ++i) {
		if (t[p[i]])
			return JSTR_DIFF(p + i, s);
		if (--n == 0) {
			if (sz < 4)
				return sz;
			break;
		}
	}
	p = (const unsigned char *)JSTR_PTR_ALIGN_UP(p, 4);
	const unsigned char *const end = p + sz;
	unsigned int c0, c1, c2, c3;
	do {
		c0 = t[p[0]];
		c1 = t[p[1]];
		c2 = t[p[2]];
		c3 = t[p[3]];
		p -= 4;
	} while (((p >= end) | c0 | c1 | c2 | c3) == 0);
	size_t cnt = JSTR_DIFF(p, s);
	cnt = sz - (((c0 | c1) != 0) ? cnt - c0 + 1 : cnt - c2 + 3);
	return (cnt < sz) ? cnt : sz;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_mempbrk(const void *s, const char *accept, size_t sz)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s;
	p += jstr_memcspn(p, accept, sz);
	return *p ? (char *)p : NULL;
}

/* Return value:
 * ptr to first non-C in S;
 * ptr to '\0' if C is not found;
 * S if C is '\0'; */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strchrnulinv(const char *s, int c)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(c == '\0'))
		return (char *)s;
	while (*s++ == (char)c) {}
	return (char *)s - 1;
}

/* Return value:
 * ptr to first non-C in S;
 * NULL if non-C is not found. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strchrinv(const char *s, int c)
JSTR_NOEXCEPT
{
	s = jstr_strchrnulinv(s, c);
	return *s != (char)c ? (char *)s : NULL;
}

/* Return value:
 * ptr to first non-C in S.
 * S + N if C is not found. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memchrnulinv(const void *s, int c, size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s;
	for (; n-- && *p == (unsigned char)c; ++p) {}
	return (void *)p;
}

/* Return value:
 * ptr to first non-C in S.
 * NULL if C is not found. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memchrinv(const void *s, int c, size_t n)
JSTR_NOEXCEPT
{
	const void *const end = (const unsigned char *)s + n;
	s = jstr_memchrnulinv(s, c, n);
	return (s != end) ? (void *)s : NULL;
}

/* Return value:
 * ptr to first non-C in S from S + N - 1
 * NULL if C is not found. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_memrchrinv(const void *s, int c, size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s + n - 1;
	for (; n && *p != (unsigned char)c; --n, --p) {}
	return n ? (char *)p : NULL;
}

/* Return value:
 * ptr to first non-C in S from S + strlen(S) - 1
 * NULL if C is not found. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_strrchrinv(const char *s, int c)
JSTR_NOEXCEPT
{
	return jstr_memrchrinv(s, c, strlen(s));
}

/* Count occurences of C in S.
 * Return value:
 * Occurences of C in S. */
#if JSTR_HAVE_SIMD && !JSTR_HAVENT_COUNTCHR_SIMD
JSTR_ATTR_NO_SANITIZE_ADDRESS
#endif
JSTR_FUNC_PURE
static size_t
jstr_countchr(const char *s, int c)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_SIMD && !JSTR_HAVENT_COUNTCHR_SIMD
	return jstr__simd_countchr(s, c);
#else
	size_t cnt = 0;
	if (jstr_likely(c))
		for (; (s = strchr(s, c)); ++s, ++cnt) {}
	return cnt;
#endif
}

/* This is vectorized at -O3 (GCC >= 4.7) and -O2 (clang >= 3.9). */
#if (JSTR_GNUC_PREREQ(4, 7) && (defined __OPTIMIZE__ && __OPTIMIZE__ >= 3)) \
|| defined __clang__ || !(JSTR_HAVE_SIMD && !JSTR_HAVENT_COUNTCHR_LEN_SIMD)
#	define JSTR_USE_COUNTCHR_LEN_C 1
#else
#	define JSTR_USE_COUNTCHR_LEN_C 0
#endif

/* Count occurences of C in S.
 * Return value:
 * Occurences of C in S. */
#if !JSTR_USE_COUNTCHR_LEN_C
JSTR_ATTR_NO_SANITIZE_ADDRESS
#endif
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_countchr_len(const char *s, int c, size_t sz)
JSTR_NOEXCEPT
{
#if JSTR_USE_COUNTCHR_LEN_C
	size_t cnt = 0;
	for (; sz--; cnt += *s++ == (char)c) {}
	return cnt;
#else
	return jstr__simd_countchr_len(s, c, sz);
#endif
}

/* Count occurences of NE in HS.
 * Return value:
 * occurences of NE in HS.
 * T must be precompiled with jstr_memmem_exec. */
JSTR_FUNC_PURE
JSTR_ATTR_ACCESS((__read_only__, 2, 3))
JSTR_ATTR_ACCESS((__read_only__, 4, 5))
static size_t
jstr_count_len_exec(const jstr_twoway_ty *R t, const char *s, size_t sz, const char *find, size_t find_len)
JSTR_NOEXCEPT
{
	if (find_len == 1)
		return jstr_countchr_len(s, *find, sz);
	if (jstr_unlikely(find_len == 0))
		return 0;
	size_t cnt = 0;
	for (const char *const end = s + sz; (s = (const char *)jstr_memmem_exec(t, s, JSTR_DIFF(end, s), find)); ++cnt, s += find_len) {}
	return cnt;
}

/* Count occurences of NE in HS.
 * Return value:
 * occurences of NE in HS. */
JSTR_FUNC_PURE
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
static size_t
jstr_count_len(const char *s, size_t sz, const char *find, size_t find_len)
JSTR_NOEXCEPT
{
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	return jstr_count_len_exec(&t, s, sz, find, find_len);
}

/* Count occurences of NE in HS.
 * Return value:
 * occurences of NE in HS. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_count(const char *s, const char *find)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(find[0] == '\0'))
		return 0;
	if (jstr_unlikely(find[1] == '\0'))
		return jstr_countchr(s, *find);
	if (jstr_unlikely(*s == '\0'))
		return 0;
	jstr_twoway_ty t;
	jstr_strstr_comp(&t, find);
	size_t cnt = 0;
	for (; (s = jstr_strstr_exec(&t, s, find)); ++cnt, s += t.needle_len) {}
	return cnt;
}

/* Return value:
 * ptr to startning of line;
 * BEGIN if no newline was found. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_linestart(const char *const start, const char *end)
JSTR_NOEXCEPT
{
	end = (char *)jstr_memrchr(start, '\n', JSTR_DIFF(end, start));
	return (char *)(end ? end + 1 : start);
}

/* Return value:
 * ptr to next line;
 * NULL if no newline was found. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_linenext_len(const char *start, const char *const end)
JSTR_NOEXCEPT
{
	start = (char *)memchr(start, '\n', JSTR_DIFF(end, start));
	return (start && *(start + 1)) ? (char *)start + 1 : NULL;
}

/* Return value:
 * ptr to next line;
 * NULL if no newline was found. */
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
 * ptr to next line or '\0'. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_linenextnul_len(const char *start, const char *const end)
JSTR_NOEXCEPT
{
	start = jstr_linenext_len(start, end);
	return (char *)(start ? start : end);
}

/* Return value:
 * ptr to next line or '\0'. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_linenextnul(const char *R s)
JSTR_NOEXCEPT
{
	s = jstr_strchrnul(s, '\n');
	return (char *)((*s && *(s + 1)) ? s + 1 : s);
}

/* Return the number of newlines + 1. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_linenumber(const char *start, const char *const end)
JSTR_NOEXCEPT
{
	return jstr_countchr_len(start, '\n', JSTR_DIFF(end, start)) + 1;
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_revcpy_len(char *R dst, const char *R src, size_t src_len)
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
jstr_revcpy_p(char *R dst, const char *R src)
{
	const size_t len = strlen(src);
	jstr_revcpy_len(dst, src, len);
	return dst + len;
}

/* Reverse S. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_rev_len(char *R s, size_t sz)
JSTR_NOEXCEPT
{
	char *e = s + sz - 1;
	int c;
	for (; s < e;) {
		c = *s;
		*s++ = *e;
		*e-- = c;
	}
}

/* Reverse S.
 * Return value:
 * ptr to '\0' in S. */
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_rev_p(char *R s)
JSTR_NOEXCEPT
{
	const size_t len = strlen(s);
	jstr_rev_len(s, len);
	return s + len;
}

/* Trim leading and trailing jstr_isspace() chars in S.
 * Return value:
 * ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
static char *
jstr_trimend_len_p(char *R s, size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return s;
	s = jstr_skipspace_rev(s, sz);
	*++s = '\0';
	return s;
}

/* Trim leading and trailing jstr_isspace() chars in S.
 * Return value:
 * ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_trimend_p(char *R s)
JSTR_NOEXCEPT
{
	return jstr_trimend_len_p(s, strlen(s));
}

/* Trim leading and trailing jstr_isspace() chars in S.
 * Return value:
 * ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
static char *
jstr_trimstart_len_p(char *R s, size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return s;
	const char *const start = jstr_skipspace(s);
	sz = JSTR_DIFF(s + sz, start);
	if (s != start)
		return jstr_stpmove_len(s, start, sz);
	return s + sz;
}

/* Trim leading jstr_isspace() chars in S.
 * Return value:
 * ptr to '\0' in S; */
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
 * Return value:
 * ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
static char *
jstr_trim_len_p(char *R s, size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return s;
	const char *const end = jstr_skipspace_rev(s, sz);
	const char *const start = jstr_skipspace(s);
	sz = JSTR_DIFF(end, start);
	if (start != s)
		return jstr_stpmove_len(s, start, sz);
	return s + sz;
}

/* Trim leading and trailing jstr_isspace() chars in S.
 * Return value:
 * ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_trim_p(char *R s)
JSTR_NOEXCEPT
{
	return jstr_trim_len_p(s, strlen(s));
}

/* Convert snake_case to camelCase.
 * Return ptr to '\0' in S.
 * Leading underscores are preserved. */
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
 * Return ptr to '\0' in DST.
 * Leading underscores are preserved. */
JSTR_FUNC_RET_NONNULL
static char *
jstr_toCamelCaseCpyP(char *R dst, const char *R src)
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
 * Return ptr to '\0' in S.
 * Leading underscores are preserved. */
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
			jstr_strmove_len((char *)p + 1, (const char *)p, JSTR_DIFF(end++, p));
			*p++ = '_';
			*p = jstr_tolower(*p);
		}
	*p = '\0';
	return (char *)p;
}

/* Convert camelCase to snake_case.
 * Return ptr to '\0' in DST.
 * Leading underscores are preserved. */
JSTR_FUNC_RET_NONNULL
static char *
jstr_to_snake_case_cpy_p(char *R dst, const char *R src)
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
 * END must be NUL terminated.
 * Instead of nul-termination, use the save_ptr to know the length of the
 * string. */
JSTR_FUNC_PURE
static char *
jstr_strtok_ne_len(const char **R const save_ptr, const char *R const end, const char *R ne, size_t ne_len)
JSTR_NOEXCEPT
{
	const char *const s = *save_ptr;
	if (jstr_unlikely(*s == '\0'))
		return NULL;
	*save_ptr = jstr_strstrnul_len(s, JSTR_DIFF(end, s), ne, ne_len);
	if (jstr_likely(**save_ptr != '\0'))
		*save_ptr += ne_len;
	return (char *)s;
}

/* Non-destructive strtok.
 * Instead of nul-termination, use the save_ptr to know the length of the
 * string. */
JSTR_FUNC_PURE
static char *
jstr_strtok_ne(const char **R const save_ptr, const char *R ne)
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
 * Instead of nul-termination, use the save_ptr to know the length of the
 * string. */
JSTR_FUNC_PURE
static char *
jstr_strtok(const char **R save_ptr, const char *R delim)
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
jstr_cpy_p(char *R dst, const jstr_ty *R src)
JSTR_NOEXCEPT
{
	return jstr_stpcpy_len(dst, src->data, src->size);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_dup(jstr_ty *R dst, const jstr_ty *R src)
JSTR_NOEXCEPT
{
	dst->data = (char *)malloc(src->capacity);
	if (jstr_nullchk(dst->data))
		return JSTR_RET_ERR;
	dst->size = JSTR_DIFF(jstr_cpy_p(dst->data, dst), dst->data);
	dst->size = src->size;
	dst->capacity = src->capacity;
	return JSTR_RET_SUCC;
}

/* Return ptr to '\0' in DST. */
JSTR_FUNC
static char *
jstr_repeat_len_unsafe_p(char *s, size_t sz, size_t n)
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

/* Return ptr to '\0' in DST. */
JSTR_FUNC
static char *
jstr_repeatcpy_len_p(char *R dst, const char *R src, size_t src_len, size_t n)
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
 * Return value:
 * ptr to '\0' in NPTR.
 * For example: 1234 becomes 1,234. */
JSTR_FUNC_RET_NONNULL
static char *
jstr_thousep_len_p(char *R nptr, size_t sz, char separator)
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
 * Return value:
 * ptr to '\0' in NPTR.
 * For example: 1234 becomes 1,234. */
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_thousep_p(char *R nptr, char separator)
JSTR_NOEXCEPT
{
	return jstr_thousep_len_p(nptr, strlen(nptr), separator);
}

/* Copy SRC to DST, adding thousand separator.
 * Return value:
 * ptr to '\0' in DST. */
JSTR_ATTR_ACCESS((__read_only__, 2, 3))
JSTR_FUNC_RET_NONNULL
static char *
jstr_thousepcpy_len_p(char *R dst, const char *R src, size_t src_len, char separator)
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
	int j = i;
	for (; j--; *dst++ = *src++) {}
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
 * Return value:
 * ptr to '\0' in DST. */
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_thousepcpy_p(char *R dst, const char *R src, char separator)
JSTR_NOEXCEPT
{
	return jstr_thousepcpy_len_p(dst, src, strlen(src), separator);
}

/* Unescape \b, \f, \n, \r, \t, \v, \\, \", \ooo (octal).
 * Otherwise, remove unescaped backslashes.
 * For example: "\q\x" -> "qx".
 * Trailing backslashes are ignored. */
JSTR_FUNC
static char *
jstr_unescapecpy_p(char *dst, const char *src)
JSTR_NOEXCEPT
{
	for (;; ++dst) {
		if (jstr_likely(*src != '\\')) {
			if (jstr_unlikely(*src == '\0'))
				break;
			*dst = *src++;
		} else {
			switch (*(src + 1)) {
			case '\0':
				goto out;
				/* clang-format off */
			case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': /* clang-format on */
			{
				int o = 3;
				*dst = 0, ++src;
				for (; o-- && *src <= '7' && *src >= '0'; ++src)
					*dst = (*dst << 3) + (*src - '0');
				goto CONT;
				break;
			}
			case 'b': *dst = '\b'; break;
			case 'f': *dst = '\f'; break;
			case 'n': *dst = '\n'; break;
			case 'r': *dst = '\r'; break;
			case 't': *dst = '\t'; break;
			case 'v': *dst = '\v'; break;
			default: *dst = *(src + 1); break;
			}
			src += 2;
		}
CONT:;
	}
out:
	*dst = '\0';
	return dst;
}

/* Unescape \b, \f, \n, \r, \t, \v, \\, \", \ooo (octal).
 * Otherwise, remove unescaped backslashes.
 * For example: "\q\x" -> "qx".
 * Trailing backslashes are ignored. */
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_unescape_p(char *s)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	s = strchrnul(s, '\\');
#else
	for (; *s && *s != '\\'; ++s) {}
#endif
	return jstr_unescapecpy_p(s, s);
}

/* Unescape \b, \f, \n, \r, \t, \v, \\, \", \ooo (octal).
 * Otherwise, remove unescaped backslashes.
 * For example: "\q\x" -> "qx".
 * Trailing backslashes are ignored. */
JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_ATTR_ACCESS((__read_only__, 2, 3))
JSTR_FUNC
static void *
jstr_unescapecpy_len_p(void *dst, const void *src, size_t n)
JSTR_NOEXCEPT
{
	unsigned char *d = (unsigned char *)dst;
	const unsigned char *s = (const unsigned char *)src;
	for (; n--; ++d) {
		if (jstr_likely(*s != '\\')) {
			*d = *s++;
		} else {
			if (jstr_unlikely(n-- == 0))
				break;
			switch (*(s + 1)) {
				/* clang-format off */
			case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': /* clang-format on */
			{
				int o = 3;
				*d = 0, ++s;
				for (; o-- && n && *s <= '7' && *s >= '0'; ++s, --n)
					*d = (*d << 3) + (*s - '0');
				goto CONT;
				break;
			}
			case 'b': *d = '\b'; break;
			case 'f': *d = '\f'; break;
			case 'n': *d = '\n'; break;
			case 'r': *d = '\r'; break;
			case 't': *d = '\t'; break;
			case 'v': *d = '\v'; break;
			default: *d = *(s + 1); break;
			}
			s += 2;
		}
CONT:;
	}
	*d = '\0';
	return d;
}

/* Unescape \b, \f, \n, \r, \t, \v, \\, \", \ooo (octal).
 * Otherwise, remove unescaped backslashes.
 * For example: "\q\x" -> "qx".
 * Trailing backslashes are ignored. */
JSTR_FUNC
JSTR_ATTR_INLINE
JSTR_ATTR_ACCESS((__read_write__, 1, 2))
static void *
jstr_unescape_len_p(void *s, size_t n)
JSTR_NOEXCEPT
{
	unsigned char *s_e = (unsigned char *)s + n;
	s = memchr(s, '\\', n);
	return s ? jstr_unescapecpy_len_p(s, s, JSTR_DIFF(s_e, s)) : s_e;
}

JSTR__END_DECLS

#undef R

#endif /* JSTR_STRING_H */
