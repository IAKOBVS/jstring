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
             int c,
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
	typedef jstr_word_ty w_ty;
	const w_ty *word_ptr = (w_ty *)JSTR_PTR_ALIGN_UP((char *)s + sz, sizeof(w_ty));
	uintptr_t s_int = (uintptr_t)s + sz;
	w_ty word = jstr_word_toword(--word_ptr);
	w_ty repeated_c = jstr_word_repeat_bytes(c);
	const w_ty *const sword = (w_ty *)JSTR_PTR_ALIGN_DOWN(s, sizeof(w_ty));
	w_ty mask = jstr_word_shift_find_last(jstr_word_find_eq_all(word, repeated_c), s_int);
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
	c = (unsigned char)c;
	const unsigned char *const start = (unsigned char *)s;
	const unsigned char *end = (unsigned char *)s + sz;
	while (end >= start && *end != c)
		--end;
	return (*end == c) ? (void *)end : NULL;
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
#else
	const char *const start = s;
	return (char *)((s = strchr(s, c)) ? s : start + strlen(start));
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
jstr_strnchr_len(const void *s,
                 const int c,
                 const size_t sz,
                 const size_t n)
JSTR_NOEXCEPT
{
	return (char *)jstr_memnchr(s, c, sz, n);
}

JSTR_INLINE
static const char *
pjstr_sadd(uintptr_t x, uintptr_t y)
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
             int c,
             const size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_WORD_AT_A_TIME
	/* The following is based on the implementation of memrchr() and strchrnul() from the
	   GNU C Library released under the terms of the GNU Lesser General Public License.
	   Copyright (C) 1991-2023 Free Software Foundation, Inc. */
	if (jstr_unlikely(n == 0)
	    || jstr_unlikely(*s == '\0'))
		return NULL;
	typedef jstr_word_ty w_ty;
	const w_ty *word_ptr = (w_ty *)JSTR_PTR_ALIGN_DOWN(s, sizeof(w_ty));
	uintptr_t s_int = (uintptr_t)s;
	w_ty word = jstr_word_toword(word_ptr);
	w_ty repeated_c = jstr_word_repeat_bytes(c);
	const char *const lbyte = pjstr_sadd(s_int, n - 1);
	const w_ty *const lword = (w_ty *)JSTR_PTR_ALIGN_DOWN(lbyte, sizeof(w_ty));
	w_ty mask = jstr_word_shift_find(jstr_word_find_zero_eq_all(word, repeated_c), s_int);
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
	c = (unsigned char)c;
	const unsigned char *start = (unsigned char *)s;
	const unsigned char *const end = (unsigned char *)s + n;
	while (*start && start >= end && *start != c)
		++start;
	return (*start == c) ? (char *)start : NULL;
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
jstr_strmove_len(void *dst,
                 const void *src,
                 const size_t sz)
JSTR_NOEXCEPT
{
	*(char *)jstr_mempmove(dst, src, sz) = '\0';
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_strmove(void *dst,
             const void *src)
JSTR_NOEXCEPT
{
	*(char *)jstr_mempmove(dst, src, strlen((char *)src)) = '\0';
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_stpmove_len(void *dst,
                 const void *src,
                 const size_t sz)
JSTR_NOEXCEPT
{
	jstr_strmove_len(dst, src, sz);
	return (char *)dst + sz;
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_stpmove(void *dst,
             const void *src)
JSTR_NOEXCEPT
{
	return jstr_stpmove_len(dst, src, strlen((char *)src));
}

JSTR_INLINE
JSTR_FUNC_VOID
static void
jstr_strcpy_len(void *R dst,
                const void *R src,
                const size_t sz)
JSTR_NOEXCEPT
{
	*(char *)jstr_mempcpy(dst, src, sz) = '\0';
}

JSTR_INLINE
JSTR_FUNC_VOID
static char *
jstr_stpcpy_len(void *R dst,
                const void *R src,
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
#else
	return jstr_stpcpy_len(dst, src, strlen(src));
#endif /* !JSTR_HAVE_STPCPY */
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
	const void *const p = (void *)memchr(src, c, sz);
	if (p != NULL) {
		return jstr_stpcpy_len(dst, src, (const char *)p - (const char *)src);
	}
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

JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_strpbrk(const char *s,
             const char *accept)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRPBRK_OPTIMIZED
	return (char *)strpbrk(s, accept);
#else
	s += strcspn(s, accept);
	return *s ? (char *)s : NULL;
#endif
}

#define PJSTR_ATOI(T, name, func)  \
	JSTR_FUNC_PURE             \
	JSTR_INLINE                \
	static T                   \
	jstr_##name(const char *s) \
	JSTR_NOEXCEPT              \
	{                          \
		return func;       \
	}

PJSTR_ATOI(int, atoi, strtol(s, NULL, 0))
PJSTR_ATOI(long, atol, strtol(s, NULL, 0))
PJSTR_ATOI(long long, atoll, strtol(s, NULL, 0))
PJSTR_ATOI(double, atod, strtod(s, NULL))
PJSTR_ATOI(float, atof, strtof(s, NULL))

#undef PJSTR_ATOI

PJSTR_END_DECLS

#undef R

#endif /* JSTR_STDSTRING_H */