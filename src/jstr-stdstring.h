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

#ifndef JSTR_STDSTRING_H
#define JSTR_STDSTRING_H 1

#include "jstr-macros.h"
#include "jstr-ctype.h"
#include "jstr-struct.h"

JSTR__BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
#include <limits.h>
JSTR__END_DECLS

#define R JSTR_RESTRICT

JSTR__BEGIN_DECLS

/* Compare S1 with S2 case-insensitively.
 * Return value:
 * 0 if strings match;
 * non-zero otherwise. */
#if JSTR_HAVE_STRNCASECMP
JSTR_ATTR_INLINE
#endif
JSTR_FUNC_PURE
static int
jstr_strncasecmp(const char *s1, const char *s2, size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNCASECMP && !JSTR_TEST
	return strncasecmp(s1, s2, n);
#else
	int ret;
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	while (n-- && !(ret = jstr_tolower(*p1) - jstr_tolower(*p2++))
	       && *p1++) {}
	return ret;
#endif
}

/* Compare S1 with S2 case-insensitively.
 * Return value:
 * 0 if strings match;
 * non-zero otherwise. */
#if JSTR_HAVE_STRNCASECMP
JSTR_ATTR_INLINE
#endif
JSTR_FUNC_PURE
static int
jstr_strncasecmpeq(const char *s1, const char *s2, size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNCASECMP && !JSTR_TEST
	return strncasecmp(s1, s2, n);
#else
	if (jstr_unlikely(n == 0))
		return 0;
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	for (; jstr_tolower(*p1) == jstr_tolower(*p2++) && *p1 && n;
	     ++p1, --n) {}
	return *p1 && n;
#endif
}

/* Compare S1 with S2 case-insensitively.
 * N must be lower than the length of S1 or S2.
 * Return value:
 * 0 if strings match;
 * non-zero otherwise. */
#if JSTR_HAVE_STRNCASECMP
JSTR_ATTR_INLINE
#endif
JSTR_FUNC_PURE
static int
jstr_strcasecmp_len(const char *s1, const char *s2, size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNCASECMP && !JSTR_TEST
	return strncasecmp(s1, s2, n);
#else
	int ret = 0;
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	while (n-- && !(ret = jstr_tolower(*p1++) - jstr_tolower(*p2++))) {}
	return ret;
#endif
}

/* Compare S1 with S2 case-insensitively.
 * N must be lower than the length of S1 or S2.
 * Return value:
 * 0 if strings match;
 * non-zero otherwise. */
JSTR_FUNC_PURE
static int
jstr_memcasecmpeq(const char *s1, const char *s2, size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	for (; n && jstr_tolower(*p1++) == jstr_tolower(*p2++); --n) {}
	return n ? 1 : 0;
}

/* Compare S1 with S2 case-insensitively.
 * N must be lower than the length of S1 or S2.
 * Return value:
 * 0 if strings match;
 * non-zero otherwise. */
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static int
jstr_strcasecmpeq_len(const char *s1, const char *s2, size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNCASECMP && !JSTR_TEST
	return strncasecmp(s1, s2, n);
#else
	return jstr_memcasecmpeq(s1, s2, n);
#endif
}

/* Compare S1 with S2 case-insensitively.
 * Return value:
 * 0 if strings match;
 * non-zero otherwise. */
JSTR_FUNC_PURE
#if JSTR_HAVE_STRCASECMP
JSTR_ATTR_INLINE
#endif
static int
jstr_strcasecmp(const char *s1, const char *s2)
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
jstr_strcasecmpeq_loop(const char *s1, const char *s2)
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	for (; jstr_tolower(*p1) == jstr_tolower(*p2++) && *p1++;) {}
	return *(p2 - 1);
}

/* Compare S1 with S2 case-insensitively.
 * Return value:
 * 0 if strings match;
 * non-zero otherwise. */
JSTR_FUNC_PURE
#if JSTR_HAVE_STRCASECMP
JSTR_ATTR_INLINE
#endif
static int
jstr_strcasecmpeq(const char *s1, const char *s2)
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
jstr_strcmpeq_loop(const char *s1, const char *s2)
{
	while ((*s1 == *s2++) && *s1++) {}
	return *(s2 - 1);
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_ATTR_ACCESS((__read_only__, 2, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_memcmpeq_loop(const void *s1, const void *s2, size_t n)
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	for (; n && (*p1++ == *p2++); --n) {}
	return n ? 1 : 0;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_strcasecmpeq_len_loop(const void *s1, const void *s2, size_t n)
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	for (; n && (jstr_tolower(*p1++) == jstr_tolower(*p2++)); --n) {}
	return n ? 1 : 0;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_strnlen(const char *s, size_t maxlen)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNLEN
	return strnlen(s, maxlen);
#else
	const char *const p = (char *)memchr(s, '\0', maxlen);
	return p ? (size_t)(p - s) : maxlen;
#endif
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_strnlen_loop(const char *s, size_t maxlen)
{
	size_t n = maxlen;
	for (; n && *s; --n, ++s) {}
	return maxlen - n;
}

JSTR_ATTR_ACCESS((__read_write__, 1, 3))
JSTR_FUNC
JSTR_ATTR_INLINE
static void *
jstr_mempset(void *s, int c, size_t n)
{
	return (char *)memset(s, c, n) + n;
}

JSTR_ATTR_ACCESS((__read_write__, 1, 3))
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_stpset_len(char *s, int c, size_t n)
{
	return (char *)memset(s, c, n) + n;
}

JSTR_ATTR_ACCESS((__read_write__, 1, 2))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstr_stpset(char *s, int c)
JSTR_NOEXCEPT
{
	return jstr_stpset_len(s, c, strlen(s));
}

JSTR_ATTR_ACCESS((__read_write__, 1, 2))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_bzero(void *s, size_t n)
JSTR_NOEXCEPT
{
	memset(s, 0, n);
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
JSTR_ATTR_INLINE
static void *
jstr_memnchr(const void *s, int c, size_t sz, size_t n)
JSTR_NOEXCEPT
{
	return (void *)memchr(s, c, JSTR_MIN(n, sz));
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_strnchr_len(const char *s, int c, size_t sz, size_t n)
JSTR_NOEXCEPT
{
	return (char *)jstr_memnchr(s, c, sz, n);
}

/* Return value:
 * ptr to '\0' in DST. */
JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static void *
jstr_mempcpy(void *R dst, const void *R src, size_t sz)
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
jstr_mempmove(void *dst, const void *src, size_t n)
JSTR_NOEXCEPT
{
	return (char *)memmove(dst, src, n) + n;
}

JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_strmove_len(char *dst, const char *src, size_t n)
JSTR_NOEXCEPT
{
	*(char *)jstr_mempmove(dst, src, n) = '\0';
}

JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_stpmove_len(char *dst, const char *src, size_t n)
JSTR_NOEXCEPT
{
	jstr_strmove_len(dst, src, n);
	return (char *)dst + n;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_stpmove(char *dst, const char *src)
JSTR_NOEXCEPT
{
	return jstr_stpmove_len(dst, src, strlen((const char *)src));
}

JSTR_ATTR_INLINE
JSTR_FUNC_VOID
static void
jstr_strcpy_len(char *R dst, const char *R src, size_t n)
JSTR_NOEXCEPT
{
	*(char *)jstr_mempcpy(dst, src, n) = '\0';
}

JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_ATTR_INLINE
JSTR_FUNC_VOID
static char *
jstr_stpcpy_len(char *R dst, const char *R src, size_t n)
JSTR_NOEXCEPT
{
	jstr_strcpy_len(dst, src, n);
	return (char *)dst + n;
}

/* Copy until either N is 0 or C is found */
JSTR_ATTR_ACCESS((__write_only__, 1, 4))
JSTR_FUNC
JSTR_ATTR_INLINE
static void *
jstr_memccpy(void *R dst, const void *R src, int c, size_t n)
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

JSTR_ATTR_MALLOC
JSTR_FUNC
JSTR_ATTR_INLINE
static void *
jstr_memdup(const void *R s, size_t n)
JSTR_NOEXCEPT
{
	void *p = malloc(n);
	return (jstr_likely(p != NULL)) ? memcpy(p, s, n) : NULL;
}

JSTR_ATTR_MALLOC
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_strdup_len(const char *R s, size_t n)
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

#define JSTR__DEFINE_ATOI(T, name, func) \
	JSTR_FUNC_PURE                   \
	JSTR_ATTR_INLINE                 \
	static T jstr_##name(const char *s) JSTR_NOEXCEPT { return func; }

JSTR__DEFINE_ATOI(unsigned int, atou, strtoul(s, NULL, 0))
JSTR__DEFINE_ATOI(unsigned long, atoul, strtoul(s, NULL, 0))
JSTR__DEFINE_ATOI(unsigned long long, atoull, strtoull(s, NULL, 0))
JSTR__DEFINE_ATOI(int, atoi, strtol(s, NULL, 0))
JSTR__DEFINE_ATOI(long, atol, strtol(s, NULL, 0))
JSTR__DEFINE_ATOI(long long, atoll, strtol(s, NULL, 0))
JSTR__DEFINE_ATOI(double, atod, strtod(s, NULL))
JSTR__DEFINE_ATOI(float, atof, strtof(s, NULL))

#undef JSTR__DEFINE_ATOI

JSTR__END_DECLS

#undef R

#endif /* JSTR_STDSTRING_H */
