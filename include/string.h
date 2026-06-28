/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
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
#	define JSTR_STRING_H 1

#	include "macros.h"
#	include "config.h"
#	include "struct.h"

JSTR_INTERNAL_BEGIN_DECLS
#	include <stdio.h>
#	include <stdlib.h>
#	include <string.h>
JSTR_INTERNAL_END_DECLS

#ifdef JSTR_IMPLEMENTATION
#undef JSTR_IMPLEMENTATION
#	include "stdstring.h"
#	include "ctype.h"
#	include "internal/musl/stdstring.h"
#define JSTR_IMPLEMENTATION 1
#endif

#ifdef JSTR_IMPLEMENTATION
#	if defined __AVX512BW__ || defined __AVX2__ || defined __SSE2__
#		define JSTR_HAVE_SIMD 1
#		include "internal/simd.h"
#	endif
#endif

#	define R JSTR_RESTRICT

JSTR_INTERNAL_BEGIN_DECLS

JSTR_FUNC_PURE
JSTR_ATTR_RETURNS_NONNULL
char *
jstr_strchrnul(const char *s, int c) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_STRCHRNUL && !JSTR_TEST
	return (char *)strchrnul(s, c);
#	elif JSTR_HAVE_SIMD && !JSTR_HAVENT_STRCHRNUL_SIMD
	return jstr_internal_simd_strchrnul(s, c);
#	else
	char *p = strchr(s, c);
	return p ? p : (char *)s + strlen(s);
#	endif
}
#else
;
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
void *
jstr_memrchr(const void *s, int c, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_MEMRCHR && !JSTR_TEST
	return (void *)memrchr(s, c, n);
#	elif JSTR_HAVE_SIMD && !JSTR_HAVENT_MEMRCHR_SIMD
	return jstr_internal_simd_memrchr(s, c, n);
#	else
	return jstr_internal_memrchr_musl(s, c, n);
#	endif
}
#else
;
#endif

/* Return value:
 * ptr to first C in S ignoring case.
 * NULL if not found. */
JSTR_FUNC_PURE
char *
jstr_strcasechrnul(const char *s, int c) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (!jstr_isalpha(c))
		return jstr_strchrnul(s, c);
#	if JSTR_HAVE_SIMD && !JSTR_HAVENT_STRCASECHRNUL_SIMD
	return jstr_internal_simd_strcasechrnul(s, c);
#	elif JSTR_HAVE_STRCSPN_OPTIMIZED
	c = jstr_tolower(c);
	if (jstr_tolower(*s) != c) {
		const char a[] = { (char)c, (char)jstr_toupper(c), '\0' };
		s += strcspn(s, a);
	}
	return (char *)s;
#	else
	return jstr_internal_strcasechrnul_musl(s, c);
#	endif
}
#else
;
#endif

/* Return value:
 * ptr to first C in S ignoring case.
 * NULL if not found. */
JSTR_FUNC_PURE
char *
jstr_strcasechr(const char *s, int c) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (!jstr_isalpha(c))
		return (char *)strchr(s, c);
	c = jstr_tolower(c);
#	if JSTR_HAVE_SIMD && !JSTR_HAVENT_STRCASECHRNUL_SIMD
	s = jstr_internal_simd_strcasechrnul(s, c);
#	elif JSTR_HAVE_STRCSPN_OPTIMIZED
	if (jstr_tolower(*s) != c) {
		const char a[] = { (char)c, (char)jstr_toupper(c), '\0' };
		s += strcspn(s, a);
	}
#	else
	s = jstr_internal_strcasechrnul_musl(s, c);
#	endif
	return jstr_tolower(*s) == (char)c ? (char *)s : NULL;
}
#else
;
#endif

JSTR_FUNC_PURE
void *
jstr_memcasechr(const void *s, int c, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (!jstr_isalpha(c))
		return (char *)memchr(s, c, n);
#	if JSTR_HAVE_SIMD && !JSTR_HAVENT_MEMCASECHR_SIMD
	return jstr_internal_simd_memcasechr(s, c, n);
#	else
	return jstr_internal_memcasechr_musl(s, c, n);
#	endif
}
#else
;
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
void *
jstr_memrchrnul(const void *s, int c, size_t sz) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const void *p = jstr_memrchr(s, c, sz);
	return (void *)(p ? p : (char *)s + sz);
}
#else
;
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
void *
jstr_memchrnul(const void *s, int c, size_t sz) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const void *p = memchr(s, c, sz);
	return (void *)(p ? p : (char *)s + sz);
}
#else
;
#endif

/* Return value:
 * ptr to '\0' in DST. */
JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC_RET_NONNULL
char *
jstr_stpcpy(char *R dst, const char *R src) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_STPCPY && !JSTR_TEST
	return stpcpy(dst, src);
#	else
	return jstr_stpcpy_len(dst, src, strlen(src));
#	endif
}
#else
;
#endif

/* Return value:
 * ptr to '\0' in DST. */
JSTR_FUNC_RET_NONNULL
char *
jstr_stpcat(char *R dst, const char *R src) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	dst += strlen(dst);
	return jstr_stpcpy(dst, src);
}
#else
;
#endif

/* strchr before s + N. */
JSTR_FUNC_PURE
char *
jstr_strnchr(const char *s, int c, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_SIMD && !JSTR_HAVENT_STRNCHR_SIMD
	return jstr_internal_simd_strnchr(s, c, n);
#	else
	return jstr_internal_strnchr_musl(s, c, n);
#	endif
}
#else
;
#endif

/* strcasechr before s + N. */
JSTR_FUNC_PURE
char *
jstr_strncasechr(const char *s, int c, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (!jstr_isalpha(c))
		return jstr_strnchr(s, c, n);
#	if JSTR_HAVE_SIMD && !JSTR_HAVENT_STRNCASECHR_SIMD
	return jstr_internal_simd_strncasechr(s, c, n);
#	else
	return jstr_internal_strncasechr_musl(s, c, n);
#	endif
}
#else
;
#endif

/* basename for non nul-terminated strings. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_FUNC_PURE
char *
jstr_basename_len(const char *fname, size_t sz) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	char *p = (char *)jstr_memrchr(fname, '/', sz);
	return p ? p + 1 : (char *)fname;
}
#else
;
#endif

JSTR_FUNC_PURE
char *
jstr_basename(const char *fname) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	char *p = (char *)strrchr(fname, '/');
	return p ? p + 1 : (char *)fname;
}
#else
;
#endif

JSTR_FUNC_PURE
char *
jstr_strstrnul(const char *hs, const char *ne) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (*ne && ne[1]) {
		const char *p = strstr(hs, ne);
		return (char *)(p ? p : hs + strlen(hs));
	} else {
		return jstr_strchrnul(hs, *ne);
	}
}
#else
;
#endif

JSTR_FUNC_PURE
int
jstr_cmpeq(const void *s1, size_t s1_len, const void *s2, size_t s2_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return (s1_len == s2_len) ? memcmp(s1, s2, s1_len) : 1;
}
#else
;
#endif

JSTR_FUNC_PURE
int
jstr_cmpcaseeq(const char *s1, size_t s1_len, const char *s2, size_t s2_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return (s1_len == s2_len) ? jstr_strcasecmpeq(s1, s2) : 1;
}
#else
;
#endif

/* Check if S2 is in end of S1.
 * Return value:
 * 1 if true.
 * 0 if false. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
int
jstr_endscase_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_likely(hs_len >= ne_len) ? !jstr_strcasecmpeq_len(hs + hs_len - ne_len, ne, ne_len) : 0;
}
#else
;
#endif

/* Check if S2 is in end of S1.
 * Return value:
 * 1 if true.
 * 0 if false. */
JSTR_FUNC_PURE
int
jstr_endscase(const char *hs, const char *ne) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_endscase_len(hs, strlen(hs), ne, strlen(ne));
}
#else
;
#endif

/* Check if S2 is in end of S1.
 * Return value:
 * 1 if true.
 * 0 if false. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
int
jstr_ends_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_likely(hs_len >= ne_len) ? !memcmp(hs + hs_len - ne_len, ne, ne_len) : 0;
}
#else
;
#endif

/* Check if S2 is in end of S1.
 * Return value:
 * 1 if true.
 * 0 if false. */
JSTR_FUNC_PURE
int
jstr_ends(const char *hs, const char *ne) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_ends_len(hs, strlen(hs), ne, strlen(ne));
}
#else
;
#endif

/* Check if S1 starts with S2 case-insensitively.
 * Return value:
 * 1 if true.
 * 0 if false. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
int
jstr_startscase_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_likely(hs_len >= ne_len) ? !jstr_strcasecmpeq_len(hs, ne, ne_len) : 0;
}
#else
;
#endif

/* Check if S1 starts with S2 case-insensitively.
 * Return value:
 * 1 if true.
 * 0 if false. */
JSTR_FUNC_PURE
int
jstr_startscase(const char *hs, const char *ne) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return (jstr_tolower(*hs) == jstr_tolower(*ne)) ? !jstr_strcasecmpeq_len(hs, ne, strlen(ne)) : (*ne == '\0');
}
#else
;
#endif

/* Check if S1 starts with S2 case-insensitively.
 * Return value:
 * NELEN if true.
 * 0 if false. */
JSTR_FUNC_PURE
size_t
jstr_startscasenul(const char *hs, const char *ne) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_tolower(*hs) == jstr_tolower(*ne)) {
		const size_t ne_len = strlen(ne);
		if (!jstr_strncasecmp(hs, ne, ne_len))
			return ne_len;
	}
	return 0;
}
#else
;
#endif

/* Check if S1 starts with S2.
 * Return value:
 * 1 if true.
 * 0 if false. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
int
jstr_starts_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_likely(hs_len >= ne_len) ? !memcmp(hs, ne, ne_len) : 0;
}
#else
;
#endif

/* Check if S1 starts with S2.
 * Return value:
 * 1 if true.
 * 0 if false. */
JSTR_FUNC_PURE
int
jstr_starts(const char *hs, const char *ne) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return (*hs == *ne) ? !strncmp(hs, ne, strlen(ne)) : (*ne == '\0');
}
#else
;
#endif

#ifdef JSTR_IMPLEMENTATION
#	define JSTR_INTERNAL_STRSTR234_MEMMEM    1
#	define JSTR_INTERNAL_STRSTR234_FUNC_NAME jstr_internal_memmem
#	include "internal/musl/strstr-lt8.h"

#	define JSTR_INTERNAL_STRSTR234_CANON     jstr_tolower
#	define JSTR_INTERNAL_STRSTR234_FUNC_NAME jstr_internal_strcasestr
#	include "internal/musl/strstr-lt8.h"

#	define JSTR_INTERNAL_STRSTR234_MEMMEM    1
#	define JSTR_INTERNAL_STRSTR234_CANON     jstr_tolower
#	define JSTR_INTERNAL_STRSTR234_FUNC_NAME jstr_internal_memcasemem
#	include "internal/musl/strstr-lt8.h"

#	define JSTR_INTERNAL_MUSL_FUNC_NAME jstr_internal_memmem_musl
#	include "internal/musl/twoway.h"

#	if JSTR_HAVE_MEMMEM && JSTR_HAVE_MEMMEM_OPTIMIZED && !JSTR_TEST
#		define JSTR_USE_MEMMEM_LIBC 1
#	else
#		define JSTR_USE_MEMMEM_LIBC 0
#	endif
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
void *
jstr_memmem(const void *hs, size_t hs_len, const void *ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_USE_MEMMEM_LIBC
	return memmem(hs, hs_len, ne, ne_len);
#	elif JSTR_HAVE_SIMD && !JSTR_HAVENT_MEMMEM_SIMD
	if (jstr_likely(ne_len <= sizeof(jstr_vvec_ty)))
		return jstr_internal_simd_memmem(hs, hs_len, ne, ne_len);
	return (hs_len >= ne_len) ? jstr_internal_memmem_musl((const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len) : NULL;
#	else
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
		return jstr_internal_memmem_lt8((const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len);
	}
	return jstr_internal_memmem_musl((const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len);
#	endif
}
#else
;
#endif

/* Needles <= THRES will not compile a jstr_twoway_ty. */
#	if JSTR_HAVE_SIMD && !JSTR_HAVENT_MEMMEM_SIMD
#		define JSTR_TWOWAY_MEMMEM_THRES sizeof(jstr_vvec_ty)
#	else
#		define JSTR_TWOWAY_MEMMEM_THRES 2
#	endif
#	define JSTR_TWOWAY_STRSTR_THRES         JSTR_TWOWAY_MEMMEM_THRES
#	define JSTR_TWOWAY_STRCASESTR_LEN_THRES 2
#	define JSTR_TWOWAY_STRCASESTR_THRES     JSTR_TWOWAY_STRCASESTR_LEN_THRES

JSTR_FUNC_VOID
void
jstr_memmem_comp(jstr_twoway_ty *t, const void *ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (ne_len > JSTR_TWOWAY_MEMMEM_THRES)
		jstr_internal_memmem_musl_comp((jstr_internal_twoway_ty *)t, (const unsigned char *)ne, ne_len);
	else
		jstr_twoway_set_len(t, ne_len);
}
#else
;
#endif

JSTR_ATTR_ACCESS((__read_only__, 2, 3))
JSTR_FUNC_PURE
void *
jstr_memmem_exec(const jstr_twoway_ty *t, const void *hs, size_t hs_len, const void *ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (ne_len > JSTR_TWOWAY_MEMMEM_THRES)
		return jstr_internal_memmem_musl_exec((const jstr_internal_twoway_ty *)t, (const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len);
#	if JSTR_HAVE_SIMD && !JSTR_HAVENT_MEMMEM_SIMD
	return jstr_internal_simd_memmem(hs, hs_len, ne, ne_len);
#	else
	if (ne_len == 2) {
		if (jstr_unlikely(hs_len < 2))
			return NULL;
		return jstr_internal_memmem2((const unsigned char *)hs, hs_len, (const unsigned char *)ne);
	}
	if (ne_len == 1)
		return (void *)memchr(hs, *(const unsigned char *)ne, hs_len);
	/* if (ne_len == 0) */
	return (char *)hs;
#	endif
}
#else
;
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
char *
jstr_strstr_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return (char *)jstr_memmem(hs, hs_len, ne, ne_len);
}
#else
;
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
char *
jstr_strnstr_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return (char *)jstr_memmem(hs, JSTR_MIN(hs_len, n), ne, ne_len);
}
#else
;
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
void *
jstr_memnmem(const void *hs, size_t hs_len, const void *ne, size_t ne_len, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_memmem(hs, JSTR_MIN(hs_len, n), ne, ne_len);
}
#else
;
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
void *
jstr_memmemnul(const void *hs, size_t hs_len, const void *ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	hs = jstr_memmem(hs, hs_len, ne, ne_len);
	return (void *)(hs ? hs : (char *)hs + hs_len);
}
#else
;
#endif

JSTR_FUNC_PURE
char *
jstr_strstrnul_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return (char *)jstr_memmemnul(hs, hs_len, ne, ne_len);
}
#else
;
#endif

#	define JSTR_INTERNAL_STRSTR234_FUNC_NAME jstr_internal_memrmem
#	define JSTR_INTERNAL_STRSTR234_MEMRMEM   1
#	include "internal/musl/strstr-lt8.h"

/* Find last NE in HS.
 * Return value:
 * ptr to NE.
 * NULL if not found.
 * The current implementation is O(m * n). */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
void *
jstr_memrmem(const void *hs, size_t hs_len, const void *ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
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
		return jstr_internal_memrmem_lt8((const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len);
	return jstr_internal_memrmem_8more((const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len);
}
#else
;
#endif

/* Find last NE in HS.
 * Return value:
 * ptr to NE.
 * NULL if not found.
 * The current implementation is O(m * n). */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
char *
jstr_strrstr_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return (char *)jstr_memrmem(hs, hs_len, ne, ne_len);
}
#else
;
#endif

#	if JSTR_HAVE_MEMMEM_OPTIMIZED || JSTR_HAVE_STRSTR_OPTIMIZED \
	|| (JSTR_HAVE_SIMD && !JSTR_HAVENT_STRCASESTR_LEN_SIMD)
#		define JSTR_USE_MEMMEM_OPTIMIZED 1
#	else
#		define JSTR_USE_MEMMEM_OPTIMIZED 0
#	endif

#ifdef JSTR_IMPLEMENTATION
#	define JSTR_INTERNAL_MUSL_FUNC_NAME jstr_internal_strcasestr_len_musl
#	define JSTR_INTERNAL_MUSL_CANON     jstr_tolower
#	define JSTR_INTERNAL_MUSL_CMP_FUNC  jstr_strcasecmpeq_len
#	include "internal/musl/twoway.h"
#endif

/* Find NE in HS case-insensitively (ASCII).
 * Return value:
 * ptr to NE.
 * NULL if not found. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
char *
jstr_strcasestr_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if !JSTR_TEST
	for (size_t i = 0;; ++i) {
		if (i == ne_len)
			return (char *)jstr_memmem(hs, hs_len, ne, ne_len);
		if (jstr_isalpha(ne[i]))
			break;
	}
#	endif
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
		return jstr_internal_memcasemem_lt8((const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len);
	}
	return jstr_internal_strcasestr_len_musl((const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len);
}
#else
;
#endif

JSTR_FUNC_VOID
void
jstr_strcasestr_len_comp(jstr_twoway_ty *t, const char *ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (ne_len > JSTR_TWOWAY_STRCASESTR_LEN_THRES)
		jstr_internal_strcasestr_len_musl_comp((jstr_internal_twoway_ty *)t, (const unsigned char *)ne, ne_len);
	else
		jstr_twoway_set_len(t, ne_len);
}
#else
;
#endif

JSTR_ATTR_ACCESS((__read_only__, 2, 3))
JSTR_FUNC_PURE
char *
jstr_strcasestr_len_exec(const jstr_twoway_ty *t, const char *hs, size_t hs_len, const char *ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (ne_len > JSTR_TWOWAY_STRCASESTR_LEN_THRES)
		return jstr_internal_strcasestr_len_musl_exec((const jstr_internal_twoway_ty *)t, (const unsigned char *)hs, hs_len, (const unsigned char *)ne, ne_len);
	if (ne_len == 2) {
		if (jstr_unlikely(hs_len < 2))
			return NULL;
		return jstr_internal_memcasemem2((const unsigned char *)hs, hs_len, (const unsigned char *)ne);
	}
	if (ne_len == 1)
		return (char *)jstr_memcasechr(hs, *(const unsigned char *)ne, hs_len);
	/* if (ne_len == 0) */
	return (char *)hs;
}
#else
;
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
char *
jstr_strncasestr_len(const char *hs, size_t hs_len, const char *ne, size_t ne_len, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_strcasestr_len(hs, JSTR_MIN(hs_len, n), ne, ne_len);
}
#else
;
#endif

#ifdef JSTR_IMPLEMENTATION
#	define JSTR_INTERNAL_MUSL_FUNC_NAME jstr_internal_strcasestr_musl
#	define JSTR_INTERNAL_MUSL_CANON     jstr_tolower
#	define JSTR_INTERNAL_MUSL_CMP_FUNC  jstr_strcasecmpeq_len
#	define JSTR_INTERNAL_MUSL_CHECK_EOL 1
#	include "internal/musl/twoway.h"
#endif

/* Find NE in HS case-insensitively.
 * Return value:
 * ptr to NE.
 * NULL if not found. */
JSTR_FUNC_PURE
char *
jstr_strcasestr(const char *hs, const char *ne) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const unsigned char *np;
#	if !JSTR_TEST
	for (np = (const unsigned char *)ne;; ++np) {
		if (*np == '\0')
			return (char *)strstr(hs, ne);
		if (jstr_isalpha(*np))
			break;
	}
#	else
	if (jstr_unlikely(*ne == '\0'))
		return (char *)hs;
#	endif
#	if JSTR_HAVE_STRCASESTR_OPTIMIZED
	return (char *)strcasestr(hs, ne);
#	else
	hs = jstr_strcasechr(hs, *ne);
	if (jstr_unlikely(hs == NULL) || ne[1] == '\0')
		return (char *)hs;
	unsigned int ne_len;
	for (ne_len = 2; ne_len < 8 + 1 && ne[ne_len]; ne_len++) {}
	if (ne_len <= 8)
		return jstr_internal_strcasestr_lt8((const unsigned char *)hs, (const unsigned char *)ne, ne_len);
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
	return jstr_internal_strcasestr_musl((const unsigned char *)hs, (const unsigned char *)ne);
#	endif
}
#else
;
#endif

#	if JSTR_TWOWAY_STRCASESTR_THRES != 2
#		error "JSTR_TWOWAY_STRCASESTR_THRES is assumed to be 2."
#	endif

JSTR_FUNC_VOID
void
jstr_strcasestr_comp(jstr_twoway_ty *t, const char *ne) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(*ne == '\0'))
		jstr_twoway_set_len(t, 0);
	else if (*(ne + 1) == '\0')
		jstr_twoway_set_len(t, 1);
	else if (*(ne + 2) == '\0')
		jstr_twoway_set_len(t, 2);
	else
		jstr_internal_strcasestr_musl_comp((jstr_internal_twoway_ty *)t, (const unsigned char *)ne);
}
#else
;
#endif

JSTR_FUNC_PURE
char *
jstr_strcasestr_exec(const jstr_twoway_ty *t, const char *hs, const char *ne) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const size_t ne_len = jstr_twoway_get_len(t);
	if (ne_len > 2)
		return jstr_internal_strcasestr_musl_exec((const jstr_internal_twoway_ty *)t, (const unsigned char *)hs, (const unsigned char *)ne);
	if (ne_len == 1)
		return (char *)jstr_strcasechr(hs, *ne);
	if (ne_len == 2)
		return jstr_internal_strcasestr2((const unsigned char *)hs, (const unsigned char *)ne);
	/* if (ne_len == 0) */
	return (char *)hs;
}
#else
;
#endif

#ifdef JSTR_IMPLEMENTATION
#	define JSTR_INTERNAL_MUSL_FUNC_NAME jstr_internal_strstr_musl
#	define JSTR_INTERNAL_MUSL_CHECK_EOL 1
#	include "internal/musl/twoway.h"
#endif

JSTR_FUNC_VOID
void
jstr_strstr_comp(jstr_twoway_ty *t, const char *ne) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(*ne == '\0'))
		jstr_twoway_set_len(t, 0);
	if (*(ne + 1) == '\0')
		jstr_twoway_set_len(t, 1);
	if (*(ne + 2) == '\0')
		jstr_twoway_set_len(t, 2);
	else
		jstr_internal_strstr_musl_comp((jstr_internal_twoway_ty *)t, (const unsigned char *)ne);
}
#else
;
#endif

JSTR_FUNC_PURE
char *
jstr_strstr_exec(const jstr_twoway_ty *t, const char *hs, const char *ne) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_twoway_get_len(t) <= JSTR_TWOWAY_STRSTR_THRES)
		return (char *)strstr(hs, ne);
	return jstr_internal_strstr_musl_exec((const jstr_internal_twoway_ty *)t, (const unsigned char *)hs, (const unsigned char *)ne);
}
#else
;
#endif

/* Reverse of STRCSPN.
 * Return the offset from S if found.
 * Otherwise, return S + SZ. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
size_t
jstr_strrcspn_len(const char *s, const char *reject, size_t sz) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_internal_memrcspn_musl(s, reject, sz);
}
#else
;
#endif

/* Reverse of STRCSPN. */
JSTR_FUNC_PURE
size_t
jstr_strrcspn(const char *s, const char *reject) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_strrcspn_len(s, reject, strlen(s));
}
#else
;
#endif

/* Reverse of STRSPN.
 * Return the offset from S if found.
 * Otherwise, return S + SZ. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
size_t
jstr_strrspn_len(const char *s, const char *accept, size_t sz) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_internal_memrspn_musl(s, accept, sz);
}
#else
;
#endif

/* Reverse of STRSPN.
 * Return the offset from S if found.
 * Otherwise, return S + SZ. */
JSTR_FUNC_PURE
size_t
jstr_strrspn(const char *s, const char *accept) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_strrspn_len(s, accept, strlen(s));
}
#else
;
#endif

/* Reverse of STRPBRK. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
char *
jstr_strrpbrk_len(const char *s, const char *accept, size_t sz) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const size_t len = jstr_strrcspn_len(s, accept, sz);
	return (len != sz) ? (char *)(s + sz - len) : NULL;
}
#else
;
#endif

/* Reverse of STRPBRK. */
JSTR_FUNC_PURE
char *
jstr_strrpbrk(const char *s, const char *accept) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_strrpbrk_len(s, accept, strlen(s));
}
#else
;
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
size_t
jstr_memspn(const void *s, const char *accept, size_t sz) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_internal_memspn_musl((const char *)s, accept, sz);
}
#else
;
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
size_t
jstr_memcspn(const void *s, const char *reject, size_t sz) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_internal_memcspn_musl((const char *)s, reject, sz);
}
#else
;
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
char *
jstr_mempbrk(const void *s, const char *accept, size_t sz) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const unsigned char *p = (const unsigned char *)s;
	p += jstr_memcspn(p, accept, sz);
	return *p ? (char *)p : NULL;
}
#else
;
#endif

/* Return value:
 * ptr to first non-C in S,
 * ptr to '\0' if C is not found,
 * S if C is '\0'; */
JSTR_FUNC_PURE
char *
jstr_strchrnulinv(const char *s, int c) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(c == '\0'))
		return (char *)s;
	while (*s++ == (char)c) {}
	return (char *)s - 1;
}
#else
;
#endif

/* Return value:
 * ptr to first non-C in S.
 * NULL if non-C is not found. */
JSTR_FUNC_PURE
char *
jstr_strchrinv(const char *s, int c) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	s = jstr_strchrnulinv(s, c);
	return *s != (char)c ? (char *)s : NULL;
}
#else
;
#endif

/* Return value:
 * ptr to first non-C in S.
 * S + N if C is not found. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
void *
jstr_memchrnulinv(const void *s, int c, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const unsigned char *p = (const unsigned char *)s;
	for (; n-- && *p == (unsigned char)c; ++p) {}
	return (void *)p;
}
#else
;
#endif

/* Return value:
 * ptr to first non-C in S.
 * NULL if C is not found. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
void *
jstr_memchrinv(const void *s, int c, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const void *end = (const unsigned char *)s + n;
	s = jstr_memchrnulinv(s, c, n);
	return (s != end) ? (void *)s : NULL;
}
#else
;
#endif

/* Return value:
 * ptr to first non-C in S from S + N - 1
 * NULL if C is not found. */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
void *
jstr_memrchrinv(const void *s, int c, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const unsigned char *p = (const unsigned char *)s + n - 1;
	for (; n && *p != (unsigned char)c; --n, --p) {}
	return n ? (char *)p : NULL;
}
#else
;
#endif

/* Return value:
 * ptr to first non-C in S from S + strlen(S) - 1
 * NULL if C is not found. */
JSTR_FUNC_PURE
void *
jstr_strrchrinv(const char *s, int c) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_memrchrinv(s, c, strlen(s));
}
#else
;
#endif

/* Count occurences of C in S.
 * Return value:
 * Occurences of C in S. */
#	if JSTR_HAVE_SIMD && !JSTR_HAVENT_COUNTCHR_SIMD
JSTR_ATTR_NO_SANITIZE_ADDRESS
#	endif
JSTR_FUNC_PURE
size_t
jstr_countchr(const char *s, int c) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	s = strchr(s, c);
	if (jstr_nullchk(s))
		return 0;
#	if JSTR_HAVE_SIMD && !JSTR_HAVENT_COUNTCHR_SIMD
	return jstr_internal_simd_countchr(s, c);
#	else
	size_t cnt = 0;
	if (jstr_likely(c != '\0'))
		for (; (s = strchr(s, c)); ++s, ++cnt) {}
	return cnt;
#	endif
}
#else
;
#endif

/* countchr_len is at -O3 (GCC >= 4.7) and -O2 (clang >= 3.9). */
#	if (JSTR_GNUC_PREREQ(4, 7) && (defined __OPTIMIZE__ && __OPTIMIZE__ >= 3))                                                                                                   \
	|| ((defined __clang__ && defined __clang_major__ && __clang_major__ >= 3 && defined __clang_minor__ && __clang_minor__ >= 9) && (defined __OPTIMIZE__ && __OPTIMIZE__ >= 2)) \
	|| !(JSTR_HAVE_SIMD && !JSTR_HAVENT_COUNTCHR_LEN_SIMD)
#		define JSTR_USE_COUNTCHR_LEN_C 1
#	else
#		define JSTR_USE_COUNTCHR_LEN_C 0
#	endif

/* Count occurences of C in S.
 * Return value:
 * Occurences of C in S. */
#	if !JSTR_USE_COUNTCHR_LEN_C
JSTR_ATTR_NO_SANITIZE_ADDRESS
#	endif
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
size_t
jstr_countchr_len(const char *s, int c, size_t sz) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const char *s_e = s + sz;
	s = (const char *)memchr(s, c, sz);
	if (jstr_nullchk(s))
		return 0;
	sz = JSTR_DIFF(s_e, s);
#	if JSTR_USE_COUNTCHR_LEN_C
	size_t cnt = 0;
	for (; sz--; cnt += *s++ == (char)c) {}
	return cnt;
#	else
	return jstr_internal_simd_countchr_len(s, c, sz);
#	endif
}
#else
;
#endif

/* Count occurences of NE in HS.
 * Return value:
 * occurences of NE in HS.
 * T must be precompiled with jstr_memmem_exec. */
JSTR_FUNC_PURE
JSTR_ATTR_ACCESS((__read_only__, 2, 3))
JSTR_ATTR_ACCESS((__read_only__, 4, 5))
size_t
jstr_count_len_exec(const jstr_twoway_ty *t, const char *s, size_t sz, const char *find, size_t find_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (find_len == 1)
		return jstr_countchr_len(s, *find, sz);
	if (jstr_unlikely(find_len == 0))
		return 0;
	size_t cnt = 0;
	for (const char *end = s + sz; (s = (const char *)jstr_memmem_exec(t, s, JSTR_DIFF(end, s), find, find_len)); ++cnt, s += find_len) {}
	return cnt;
}
#else
;
#endif

/* Count occurences of NE in HS.
 * Return value:
 * occurences of NE in HS. */
JSTR_FUNC_PURE
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
size_t
jstr_count_len(const char *s, size_t sz, const char *find, size_t find_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	return jstr_count_len_exec(&t, s, sz, find, find_len);
}
#else
;
#endif

/* Count occurences of NE in HS.
 * Return value:
 * occurences of NE in HS. */
JSTR_FUNC_PURE
size_t
jstr_count(const char *s, const char *find) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
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
	size_t find_len = jstr_twoway_get_len(&t);
	for (; (s = jstr_strstr_exec(&t, s, find)); ++cnt, s += find_len) {}
	return cnt;
}
#else
;
#endif

/* Return value:
 * ptr to start of line.
 * BEGIN if no newline was found. */
JSTR_FUNC_PURE
char *
jstr_linestart(const char *start, const char *end) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	end = (char *)jstr_memrchr(start, '\n', JSTR_DIFF(end, start));
	return (char *)(end ? end + 1 : start);
}
#else
;
#endif

/* Return value:
 * ptr to next start of line.
 * NULL if no newline was found. */
JSTR_FUNC_PURE
char *
jstr_linenext_len(const char *start, const char *end) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	start = (char *)memchr(start, '\n', JSTR_DIFF(end, start));
	return (start && *(start + 1)) ? (char *)start + 1 : NULL;
}
#else
;
#endif

/* Return value:
 * ptr to next start of line.
 * NULL if no newline was found. */
JSTR_FUNC_PURE
char *
jstr_linenext(const char *s) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	s = strchr(s, '\n');
	return (s && *(s + 1)) ? (char *)s + 1 : NULL;
}
#else
;
#endif

/* Return value:
 * ptr to next line or '\0'. */
JSTR_FUNC_PURE
char *
jstr_linenextnul_len(const char *start, const char *end) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	start = jstr_linenext_len(start, end);
	return (char *)(start ? start : end);
}
#else
;
#endif

/* Return value:
 * ptr to next line or '\0'. */
JSTR_FUNC_PURE
char *
jstr_linenextnul(const char *s) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	s = jstr_strchrnul(s, '\n');
	return (char *)((*s && *(s + 1)) ? s + 1 : s);
}
#else
;
#endif

/* Return the number of newlines + 1. */
JSTR_FUNC_PURE
size_t
jstr_linenumber(const char *start, const char *end) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_countchr_len(start, '\n', JSTR_DIFF(end, start)) + 1;
}
#else
;
#endif

JSTR_FUNC_VOID
void
jstr_revcpy_len(char *R dst, const char *R src, size_t src_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	src += src_len - 1;
	for (; src_len--; *dst++ = *src--) {}
	*dst = '\0';
}
#else
;
#endif

/* DST must not overlap with SRC. */
JSTR_FUNC
char *
jstr_revcpy_p(char *R dst, const char *R src) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const size_t len = strlen(src);
	jstr_revcpy_len(dst, src, len);
	return dst + len;
}
#else
;
#endif

/* Reverse S. */
JSTR_FUNC_VOID
void
jstr_rev_len(char *s, size_t sz) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	char *e = s + sz - 1;
	int c;
	for (; s < e;) {
		c = *s;
		*s++ = *e;
		*e-- = c;
	}
}
#else
;
#endif

/* Reverse S.
 * Return value:
 * ptr to '\0' in S. */
JSTR_FUNC_RET_NONNULL
char *
jstr_rev_p(char *s) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const size_t len = strlen(s);
	jstr_rev_len(s, len);
	return s + len;
}
#else
;
#endif

/* Trim leading and trailing jstr_isspace chars in S.
 * Return value:
 * ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
char *
jstr_trimend_len_p(char *s, size_t sz) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(sz == 0))
		return s;
	unsigned char *p = (unsigned char *)s + sz;
	for (; sz-- && jstr_isspace(*(p - 1)); --p) {}
	*p = '\0';
	return (char *)p;
}
#else
;
#endif

/* Trim leading and trailing jstr_isspace chars in S.
 * Return value:
 * ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
char *
jstr_trimend_p(char *s) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_trimend_len_p(s, strlen(s));
}
#else
;
#endif

/* Trim leading and trailing jstr_isspace chars in S.
 * Return value:
 * ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
char *
jstr_trimstart_len_p(char *s, size_t sz) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(sz == 0))
		return s;
	const unsigned char *p = (const unsigned char *)s;
	for (; sz && jstr_isspace(*p); ++p, --sz) {}
	if (p != (unsigned char *)s)
		return jstr_stpmove_len(s, (const char *)p, sz);
	return (char *)p + sz;
}
#else
;
#endif

/* Trim leading jstr_isspace chars in S.
 * Return value:
 * ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
char *
jstr_trimstart_p(char *s) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(*s == '\0'))
		return s;
	const unsigned char *p = (const unsigned char *)s;
	for (; jstr_isspace(*p); ++p) {}
	const size_t p_len = strlen((char *)p);
	if (p != (unsigned char *)s)
		return jstr_stpmove_len(s, (const char *)p, p_len);
	return (char *)p + p_len;
}
#else
;
#endif

/* Trim leading jstr_isspace chars in S. */
JSTR_FUNC_VOID
void
jstr_trimstart(char *s) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(*s == '\0'))
		return;
	const unsigned char *p = (const unsigned char *)s;
	for (; jstr_isspace(*p); ++p) {}
	if (p != (unsigned char *)s)
		jstr_strmove_len(s, (const char *)p, strlen((char *)p));
}
#else
;
#endif

/* Trim leading and trailing jstr_isspace chars in S.
 * Return value:
 * ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
char *
jstr_trim_len_p(char *s, size_t sz) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const char *end = jstr_trimend_len_p(s, sz);
	return jstr_trimstart_len_p(s, JSTR_DIFF(end, s));
}
#else
;
#endif

/* Trim leading and trailing jstr_isspace chars in S.
 * Return value:
 * ptr to '\0' in S; */
JSTR_FUNC_RET_NONNULL
char *
jstr_trim_p(char *s) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_trim_len_p(s, strlen(s));
}
#else
;
#endif

/* Non-destructive strtok.
 * END must be NUL terminated.
 * Instead of nul-termination, use the save_ptr to know the length of the
 * string. */
JSTR_FUNC_PURE
char *
jstr_strtok_ne_len(const char **R const save_ptr, const char *R const end, const char *R ne, size_t ne_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const char *s = *save_ptr;
	if (jstr_unlikely(*s == '\0'))
		return NULL;
	*save_ptr = jstr_strstrnul_len(s, JSTR_DIFF(end, s), ne, ne_len);
	if (jstr_likely(**save_ptr != '\0'))
		*save_ptr += ne_len;
	return (char *)s;
}
#else
;
#endif

/* Non-destructive strtok.
 * Instead of nul-termination, use the save_ptr to know the length of the
 * string. */
JSTR_FUNC_PURE
char *
jstr_strtok_ne(const char **R const save_ptr, const char *R ne) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const char *s = *save_ptr;
	if (jstr_unlikely(*s == '\0'))
		return NULL;
	*save_ptr = jstr_strstrnul(s, ne);
	if (jstr_likely(**save_ptr != '\0'))
		*save_ptr += strlen(ne);
	return (char *)s;
}
#else
;
#endif

/* Non-destructive strtok.
 * Instead of nul-termination, use the save_ptr to know the length of the
 * string. */
JSTR_FUNC_PURE
char *
jstr_strtok(const char **R save_ptr, const char *R delim) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
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
#else
;
#endif

/* Return ptr to '\0' in DST. */
JSTR_FUNC
char *
jstr_repeat_len_unsafe_p(char *s, size_t sz, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
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
#else
;
#endif

/* Return ptr to '\0' in DST. */
JSTR_FUNC
char *
jstr_repeatcpy_len_p(char *R dst, const char *R src, size_t src_len, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_likely(src_len > 1))
		while (n--)
			dst = (char *)jstr_mempcpy(dst, src, src_len);
	else if (src_len == 1)
		dst = (char *)memset(dst, *src, n) + n;
	*dst = '\0';
	return dst;
}
#else
;
#endif

/* Add thousand separator to NPTR.
 * Return value:
 * ptr to '\0' in NPTR.
 * For example: 1234 becomes 1,234. */
JSTR_FUNC_RET_NONNULL
char *
jstr_thousep_len_p(char *nptr, size_t sz, char separator) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	char *end = nptr + sz;
	if (jstr_unlikely(sz == 0))
		return nptr;
	if (*nptr == '-') {
		++nptr;
		--sz;
	}
	if (sz < 4)
		return end;
	size_t dif = (sz - 1) / 3;
	end += dif;
	const char *start = nptr;
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
#else
;
#endif

/* Add thousand separator to NPTR.
 * Return value:
 * ptr to '\0' in NPTR.
 * For example: 1234 becomes 1,234. */
JSTR_FUNC_RET_NONNULL
char *
jstr_thousep_p(char *nptr, char separator) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_thousep_len_p(nptr, strlen(nptr), separator);
}
#else
;
#endif

/* Copy SRC to DST, adding thousand separator.
 * Return value:
 * ptr to '\0' in DST. */
JSTR_ATTR_ACCESS((__read_only__, 2, 3))
JSTR_FUNC_RET_NONNULL
char *
jstr_thousepcpy_len_p(char *R dst, const char *R src, size_t src_len, char separator) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(src_len == 0)) {
		*dst = '\0';
		return dst;
	}
	if (*src == '-') {
		*dst++ = '-';
		++src;
		--src_len;
	}
	if (src_len < 4) {
		while (src_len--)
			*dst++ = *src++;
		*dst = '\0';
		return dst;
	}
	int i = src_len % 3;
	size_t src_left = src_len - (size_t)i;
	int j = i;
	for (; j--; *dst++ = *src++) {}
	if (i) {
		*dst++ = separator;
		i = 0;
	}
	for (; src_left--; ++i) {
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
#else
;
#endif

/* Copy SRC to DST, adding thousand separator.
 * Return value:
 * ptr to '\0' in DST. */
JSTR_FUNC_RET_NONNULL
char *
jstr_thousepcpy_p(char *R dst, const char *R src, char separator) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_thousepcpy_len_p(dst, src, strlen(src), separator);
}
#else
;
#endif

/* Unescape \b, \f, \n, \r, \t, \v, \\, \", \ooo (octal).
 * Otherwise, remove unescaped backslashes.
 * For example: "\q\x" -> "qx".
 * Trailing backslashes are ignored.
 * Returns pointer to first NUL even escaped NUL. */
JSTR_FUNC
char *
jstr_unescapecpy_p(char *dst, const char *src) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	unsigned char *d = (unsigned char *)dst;
	const unsigned char *s = (const unsigned char *)src;
	for (;; ++d) {
		if (jstr_likely(*s != '\\')) {
			if (jstr_unlikely(*s == '\0'))
				break;
			*d = *s++;
		} else {
			switch (s[1]) {
			case '\0':
				goto out;
				break;
				/* clang-format off */
			case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
				/* clang-format on */
				{
					*d = '\0';
					++s;
					for (int o = 3; o-- && *s <= '7' && *s >= '0'; ++s)
						*d = (*d << 3) + (*s - '0');
					/* escaped NUL */
					if (*d == '\0')
						goto out;
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
out:
	*d = '\0';
	return (char *)d;
}
#else
;
#endif

/* Unescape \b, \f, \n, \r, \t, \v, \\, \", \ooo (octal).
 * Otherwise, remove unescaped backslashes.
 * For example: "\q\x" -> "qx".
 * Trailing backslashes are ignored.
 * Returns pointer to first NUL even escaped NUL. */
JSTR_FUNC
char *
jstr_unescape_p(char *s) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_STRCHRNUL
	s = strchrnul(s, '\\');
#	else
	for (; *s && *s != '\\'; ++s) {}
#	endif
	if (*s == '\0')
	    return s;
	return jstr_unescapecpy_p(s, s);
}
#else
;
#endif

JSTR_INTERNAL_END_DECLS

#	undef R

#endif /* JSTR_STRING_H */
