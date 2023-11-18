/* See LICENSE file for copyright and license details. */

#ifndef JSTR_STDSTRING_H
#define JSTR_STDSTRING_H 1

#include "jstr-macros.h"
#include "jstr-struct.h"
#include "jstr-word-at-a-time.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

JSTR_FUNC
JSTR_INLINE
static void *
jstr_mempset(void *s,
             const int c,
             const size_t sz)
{
	return (char *)memset(s, c, sz) + sz;
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_stpset_len(char *s,
                const int c,
                const size_t sz)
{
	return (char *)memset(s, c, sz) + sz;
}

JSTR_FUNC_VOID
JSTR_INLINE
static char *
jstr_stpset(char *s,
            const int c)
JSTR_NOEXCEPT
{
	return jstr_stpset_len(s, c, strlen(s));
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_bzero(void *s,
           const size_t sz)
JSTR_NOEXCEPT
{
	memset(s, 0, sz);
}

JSTR_FUNC
JSTR_INLINE
static int
jstr_bcmp(const void *s1,
          const void *s2,
          const size_t n)
JSTR_NOEXCEPT
{
	return memcmp(s1, s2, n);
}

JSTR_FUNC_VOID
JSTR_INLINE
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
JSTR_INLINE
static char *
jstr_stpzero(char *s)
JSTR_NOEXCEPT
{
	const size_t sz = strlen(s);
	return (char *)memset(s, 0, sz) + sz;
}

JSTR_FUNC_PURE
JSTR_INLINE
static size_t
jstr_strnlen(const char *s,
             const size_t maxlen)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNLEN
	return strnlen(s, maxlen);
#else
	const char *const p = (char *)memchr(s, '\0', maxlen);
	return p ? (size_t)(p - s) : maxlen;
#endif
}

#if JSTR_HAVE_MEMRCHR
JSTR_INLINE
#endif
JSTR_FUNC_PURE
static void *
jstr_memrchr(const void *s,
             const int c,
             const size_t sz)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMRCHR
	return (void *)memrchr(s, c, sz);
#elif JSTR_HAVE_WORD_AT_A_TIME
	/* The following is based on the implementation of memrchr() from the GNU C
	   Library released under the terms of the GNU Lesser General Public License.
	   Copyright (C) 1991-2023 Free Software Foundation, Inc. */
	if (jstr_unlikely(sz == 0))
		return NULL;
	const jstr_word_ty *word_ptr = (jstr_word_ty *)JSTR_PTR_ALIGN_UP((const char *)s + sz, sizeof(jstr_word_ty));
	uintptr_t s_int = (uintptr_t)s + sz;
	jstr_word_ty word = jstr_word_toword(--word_ptr);
	jstr_word_ty repeated_c = jstr_word_repeat_bytes(c);
	const jstr_word_ty *const sword = (jstr_word_ty *)JSTR_PTR_ALIGN_DOWN(s, sizeof(jstr_word_ty));
	jstr_word_ty mask = jstr_word_shift_find_last(jstr_word_find_eq_all(word, repeated_c), s_int);
	if (mask != 0) {
		char *ret = (char *)word_ptr + jstr_word_index_last(mask);
		return ret >= (char *)s ? ret : NULL;
	}
	if (word_ptr == sword)
		return NULL;
	word = jstr_word_toword(--word_ptr);
	for (; word_ptr != sword; word = jstr_word_toword(--word_ptr))
		if (jstr_word_has_eq(word, repeated_c))
			return (char *)word_ptr + jstr_word_index_last_eq(word, repeated_c);
	if (jstr_word_has_eq(word, repeated_c)) {
		char *ret = (char *)word_ptr + jstr_word_index_last_eq(word, repeated_c);
		if (ret >= (char *)s)
			return ret;
	}
	return NULL;
#else
	const unsigned char *const start = (const unsigned char *)s;
	const unsigned char *end = (const unsigned char *)s + sz;
	for (; end >= start; --end)
		if (*end == (unsigned char)c)
			return (void *)end;
	return NULL;
#endif
}

JSTR_FUNC_PURE
static char *
jstr_strchrnul(const char *s,
               const int c)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	return (char *)strchrnul(s, c);
#elif JSTR_HAVE_STRCHR_OPTIMIZED
	const char *const start = s;
	s = strchr(s, c);
	return (char *)(s ? s : start + strlen(start));
#else
	for (; *s && *s != (char)c; ++s)
		;
	return (char *)s;
#endif
}

JSTR_FUNC_PURE
JSTR_INLINE
static void *
jstr_memnchr(const void *s,
             const int c,
             const size_t sz,
             const size_t n)
JSTR_NOEXCEPT
{
	return (void *)memchr(s, c, JSTR_MIN(n, sz));
}

JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_strnchr_len(const char *s,
                 const int c,
                 const size_t sz,
                 const size_t n)
JSTR_NOEXCEPT
{
	return (char *)jstr_memnchr(s, c, sz, n);
}

JSTR_INLINE
static const char *
pjstr_sadd(const uintptr_t x,
           const uintptr_t y)
JSTR_NOEXCEPT
{
	return (const char *)(y > UINTPTR_MAX - x ? UINTPTR_MAX : x + y);
}

/*
   strchr() before s + N.
*/
JSTR_FUNC_PURE
static char *
jstr_strnchr(const char *s,
             const int c,
             size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_WORD_AT_A_TIME
	/* The following is based on the implementation of memrchr() and strchrnul() from the
	   GNU C Library released under the terms of the GNU Lesser General Public License.
	   Copyright (C) 1991-2023 Free Software Foundation, Inc. */
	if (jstr_unlikely(n == 0)
	    || jstr_unlikely(*s == '\0'))
		return NULL;
	const jstr_word_ty *word_ptr = (jstr_word_ty *)JSTR_PTR_ALIGN_DOWN(s, sizeof(jstr_word_ty));
	uintptr_t s_int = (uintptr_t)s;
	jstr_word_ty word = jstr_word_toword(word_ptr);
	jstr_word_ty repeated_c = jstr_word_repeat_bytes(c);
	const char *const lbyte = pjstr_sadd(s_int, n - 1);
	const jstr_word_ty *const lword = (jstr_word_ty *)JSTR_PTR_ALIGN_DOWN(lbyte, sizeof(jstr_word_ty));
	jstr_word_ty mask = jstr_word_shift_find(jstr_word_find_zero_eq_all(word, repeated_c), s_int);
	if (mask != 0) {
		char *ret = (char *)s + jstr_word_index_first(mask);
		return (ret <= lbyte && *ret) ? ret : NULL;
	}
	if (word_ptr == lword)
		return NULL;
	word = jstr_word_toword(++word_ptr);
	for (; word_ptr != lword; word = jstr_word_toword(++word_ptr)) {
		if (jstr_word_has_zero_eq(word, repeated_c)) {
			char *ret = (char *)word_ptr + jstr_word_index_first_zero_eq(word, repeated_c);
			return *ret ? ret : NULL;
		}
	}
	if (jstr_word_has_zero_eq(word, repeated_c)) {
		char *ret = (char *)word_ptr + jstr_word_index_first_zero_eq(word, repeated_c);
		if (ret <= lbyte && *ret)
			return ret;
	}
	return NULL;
#else
	for (; n-- && *s; ++s)
		if (*s == (char)c)
			return (char *)s;
	return NULL;
#endif
}

JSTR_FUNC_PURE
JSTR_INLINE
static void *
jstr_memchrnul(const void *s,
               const int c,
               const size_t sz)
JSTR_NOEXCEPT
{
	const void *const p = jstr_memrchr(s, c, sz);
	return (void *)(p ? p : (char *)s + sz);
}

/*
  Return value:
  ptr to '\0' in DST.
*/
JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static void *
jstr_mempcpy(void *R dst,
             const void *R src,
             const size_t sz)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMPCPY
	return mempcpy(dst, src, sz);
#else
	return (char *)memcpy(dst, src, sz) + sz;
#endif
}

JSTR_FUNC
JSTR_INLINE
static void *
jstr_mempmove(void *dst,
              const void *src,
              const size_t sz)
JSTR_NOEXCEPT
{
	return (char *)memmove(dst, src, sz) + sz;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_strmove_len(char *dst,
                 const char *src,
                 const size_t sz)
JSTR_NOEXCEPT
{
	*(char *)jstr_mempmove(dst, src, sz) = '\0';
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_strmove(char *dst,
             const char *src)
JSTR_NOEXCEPT
{
	*(char *)jstr_mempmove(dst, src, strlen(src)) = '\0';
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_stpmove_len(char *dst,
                 const char *src,
                 const size_t sz)
JSTR_NOEXCEPT
{
	jstr_strmove_len(dst, src, sz);
	return (char *)dst + sz;
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_stpmove(char *dst,
             const char *src)
JSTR_NOEXCEPT
{
	return jstr_stpmove_len(dst, src, strlen((char *)src));
}

JSTR_INLINE
JSTR_FUNC_VOID
static void
jstr_strcpy_len(char *R dst,
                const char *R src,
                const size_t sz)
JSTR_NOEXCEPT
{
	*(char *)jstr_mempcpy(dst, src, sz) = '\0';
}

JSTR_INLINE
JSTR_FUNC_VOID
static char *
jstr_stpcpy_len(char *R dst,
                const char *R src,
                const size_t sz)
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
JSTR_INLINE
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
JSTR_FUNC
JSTR_INLINE
static void *
jstr_memccpy(void *R dst,
             const void *R src,
             const int c,
             const size_t sz)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMCCPY
	return memccpy(dst, src, c, sz);
#else
	char *p = (char *)memchr(src, c, sz);
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
JSTR_INLINE
static char *
jstr_stpcat(char *R dst,
            const char *R src)
JSTR_NOEXCEPT
{
	dst += strlen(dst);
	return jstr_stpcpy(dst, src);
}

/*
   Return ptr to '\0' in *s.
*/
JSTR_MALLOC
JSTR_FUNC
JSTR_INLINE
static char *
jstr_strdup_p(const char *R *R s)
JSTR_NOEXCEPT
{
	size_t sz = strlen(*s);
	char *const p = (char *)malloc(sz + 1);
	return (jstr_likely(p != NULL)) ? jstr_stpcpy_len(p, *s, sz) : NULL;
}

JSTR_MALLOC
JSTR_FUNC
JSTR_INLINE
static char *
jstr_memdup(const char *R s,
            const size_t sz)
JSTR_NOEXCEPT
{
	char *const p = (char *)malloc(sz);
	if (jstr_likely(p != NULL)) {
		memcpy(p, s, sz);
		return p;
	}
	return NULL;
}

JSTR_MALLOC
JSTR_FUNC
JSTR_INLINE
static char *
jstr_strdup_len(const char *R s,
                const size_t sz)
JSTR_NOEXCEPT
{
	char *const p = (char *)malloc(sz + 1);
	if (jstr_likely(p != NULL))
		return jstr_stpcpy_len(p, s, sz);
	return NULL;
}

JSTR_MALLOC
JSTR_FUNC
JSTR_INLINE
static char *
jstr_strdup(const char *R s)
JSTR_NOEXCEPT
{
	return jstr_strdup_len(s, strlen(s));
}

#define PJSTR_DEFINE_ATOI(T, name, func) \
	JSTR_FUNC_PURE                   \
	JSTR_INLINE                      \
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
