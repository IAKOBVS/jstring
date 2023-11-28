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

#ifndef JSTR_STDSTRING_H
#define JSTR_STDSTRING_H 1

#include "jstr-ctype-table.h"
#include "jstr-macros.h"
#include "jstr-struct.h"
#include "jstr-word-at-a-time.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#define R JSTR_RESTRICT
#define JSTR_LGPL_IMPL

PJSTR_BEGIN_DECLS

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_strcmpeq_loop(const char *s1,
                   const char *s2)
{
	while ((*s1++ == *s2++))
		;
	return *s2;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static int
jstr_strcasecmpeq_loop(const char *s1,
                       const char *s2)
{
	const unsigned char *p1 = (unsigned char *)s1;
	const unsigned char *p2 = (unsigned char *)s2;
	for (; (jstr_ctype_table_tolower[*p1] == jstr_ctype_table_tolower[*p2++] && *p1++); ++p2)
		;
	return *p2;
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
	for (; n && (*p1++ == *p2++); --n)
		;
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
	for (; n && (jstr_ctype_table_tolower[*p1++] == jstr_ctype_table_tolower[*p2++]); --n)
		;
	return n;
}

JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC
JSTR_ATTR_INLINE
static void *
jstr_mempset(void *s,
             int c,
             size_t sz)
{
	return (char *)memset(s, c, sz) + sz;
}

JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_stpset_len(char *s,
                int c,
                size_t sz)
{
	return (char *)memset(s, c, sz) + sz;
}

JSTR_ATTR_ACCESS((__write_only__, 1, 2))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstr_stpset(char *s,
            int c)
JSTR_NOEXCEPT
{
	return jstr_stpset_len(s, c, strlen(s));
}

JSTR_ATTR_ACCESS((__write_only__, 1, 2))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_bzero(void *s,
           size_t sz)
JSTR_NOEXCEPT
{
	memset(s, 0, sz);
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

/*
   Return pointer to '\0' in S.
*/
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstr_stpzero(char *s)
JSTR_NOEXCEPT
{
	const size_t sz = strlen(s);
	return (char *)memset(s, 0, sz) + sz;
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

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
#if JSTR_HAVE_MEMRCHR
JSTR_ATTR_INLINE
#endif
static void *
jstr_memrchr(const void *s,
             int c,
             size_t sz)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMRCHR
	return (void *)memrchr(s, c, sz);
#elif JSTR_HAVE_WORD_AT_A_TIME && 0 /* Broken? */
#	include "_lgpl-memrchr.h"
#else
	const unsigned char *const start = (const unsigned char *)s;
	const unsigned char *end = (const unsigned char *)s + sz;
	for (; end >= start; --end)
		if (*end == (unsigned char)c)
			return (void *)end;
	return NULL;
#endif
}

#undef JSTR_HAVE_STRCHRNUL
#undef JSTR_HAVE_STRCHR_OPTIMIZED

JSTR_FUNC_PURE
#if JSTR_HAVE_STRCHRNUL
JSTR_ATTR_INLINE
#endif
static char *
jstr_strchrnul(const char *s,
               int c)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	return (char *)strchrnul(s, c);
#elif JSTR_HAVE_STRCHR_OPTIMIZED
	const char *const start = s;
	s = strchr(s, c);
	return (char *)(s ? s : start + strlen(start));
#elif JSTR_HAVE_WORD_AT_A_TIME
#	include "_lgpl-strchrnul.h"
#else
	for (; *s && *s != (char)c; ++s)
		;
	return (char *)s;
#endif
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

/*
   strchr() before s + N.
*/
JSTR_FUNC_PURE
static char *
jstr_strnchr(const char *s,
             int c,
             size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_WORD_AT_A_TIME && 0 /* Broken? */
#	include "_lgpl-strnchr.h"
#else
	for (; n--; ++s) {
		if (*s == (char)c)
			return (char *)s;
		if (jstr_unlikely(*s == '\0'))
			break;
	}
	return NULL;
#endif
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

/*
  Return value:
  ptr to '\0' in DST.
*/
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
              size_t sz)
JSTR_NOEXCEPT
{
	return (char *)memmove(dst, src, sz) + sz;
}

JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_strmove_len(char *dst,
                 const char *src,
                 size_t sz)
JSTR_NOEXCEPT
{
	*(char *)jstr_mempmove(dst, src, sz) = '\0';
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_strmove(char *dst,
             const char *src)
JSTR_NOEXCEPT
{
	*(char *)jstr_mempmove(dst, src, strlen(src)) = '\0';
}
JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_stpmove_len(char *dst,
                 const char *src,
                 size_t sz)
JSTR_NOEXCEPT
{
	jstr_strmove_len(dst, src, sz);
	return (char *)dst + sz;
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
                size_t sz)
JSTR_NOEXCEPT
{
	*(char *)jstr_mempcpy(dst, src, sz) = '\0';
}

JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_ATTR_INLINE
JSTR_FUNC_VOID
static char *
jstr_stpcpy_len(char *R dst,
                const char *R src,
                size_t sz)
JSTR_NOEXCEPT
{
	jstr_strcpy_len(dst, src, sz);
	return (char *)dst + sz;
}

/*
  Return value:
  ptr to '\0' in DST.
*/
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_stpcpy(char *R dst,
            const char *R src)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STPCPY
	return stpcpy(dst, src);
#elif JSTR_HAVE_STRCPY_OPTIMIZED && JSTR_HAVE_STRLEN_OPTIMIZED
	return jstr_stpcpy_len(dst, src, strlen(src));
#else
	while ((*dst++ = *src++))
		;
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
             size_t sz)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMCCPY
	return memccpy(dst, src, c, sz);
#else
	const char *const p = (char *)memchr(src, c, sz);
	if (p != NULL)
		return jstr_stpcpy_len(dst, src, p - (char *)src);
	memcpy(dst, src, sz);
	return NULL;
#endif /* HAVE_MEMCPY */
}

/*
  Return value:
  Pointer to '\0' in DST.
*/
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
            size_t sz)
JSTR_NOEXCEPT
{
	void *p = malloc(sz);
	return (jstr_likely(p != NULL)) ? memcpy(p, s, sz) : NULL;
}

JSTR_ATTR_MALLOC
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_strdup_len(const char *R s,
                size_t sz)
JSTR_NOEXCEPT
{
	char *const p = (char *)malloc(sz + 1);
	if (jstr_likely(p != NULL))
		return jstr_stpcpy_len(p, s, sz);
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

PJSTR_DEFINE_ATOI(int, atoi, strtol(s, NULL, 0))
PJSTR_DEFINE_ATOI(long, atol, strtol(s, NULL, 0))
PJSTR_DEFINE_ATOI(long long, atoll, strtol(s, NULL, 0))
PJSTR_DEFINE_ATOI(double, atod, strtod(s, NULL))
PJSTR_DEFINE_ATOI(float, atof, strtof(s, NULL))

#undef PJSTR_DEFINE_ATOI

PJSTR_END_DECLS

#undef R

#endif /* JSTR_STDSTRING_H */
