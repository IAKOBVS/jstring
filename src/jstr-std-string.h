#ifndef JSTR_STD_STRING_H
#define JSTR_STD_STRING_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr-string-fza.h"
#include "jstr-string-fzb.h"
#include "jstr-string-fzc.h"
#include "jstr-string-fzi.h"
#include "jstr-string-misc.h"
#include "jstr-string-opthr.h"
#include "jstr-string-optype.h"
#include "jstr-string-shift.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

JSTR_FUNC
JSTR_INLINE
static void *
jstr_mempset(void *s,
	     const int c,
	     const size_t n)
{
	return (char *)memset(s, c, n) + n;
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_stpset_len(char *s,
		const int c,
		const size_t n)
{
	return (char *)memset(s, c, n) + n;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_bzero(void *R s,
	   const size_t n)
JSTR_NOEXCEPT
{
	memset(s, 0, n);
}

JSTR_FUNC
JSTR_INLINE
static int
jstr_bcmp(const void *R s1,
	  const void *R s2,
	  const size_t n)
JSTR_NOEXCEPT
{
	return memcmp(s1, s2, n);
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_strzero(char *R s)
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
jstr_stpzero(char *R s)
JSTR_NOEXCEPT
{
	const size_t n = strlen(s);
	return (char *)memset(s, 0, n) + n;
}

JSTR_FUNC_PURE
JSTR_INLINE
static size_t
jstr_strnlen(const char *R s,
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
jstr_memrchr(const void *R s,
	     const int c,
	     const size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMRCHR
	return (void *)memrchr(s, c, n);
#else
	/* Taken from glibc memrchr released under the terms of the GNU Lesser General Public License.
	   Copyright (C) 1991-2023 Free Software Foundation, Inc. */
	if (jstr_unlikely(n == 0))
		return NULL;
	typedef jstr_word_ty w_ty;
	const w_ty *word_ptr = (w_ty *)JSTR_PTR_ALIGN_UP(s + n, sizeof(w_ty));
	uintptr_t s_int = (uintptr_t)s + n;
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
#endif
}

JSTR_FUNC_PURE
static char *
jstr_strchrnul(const char *R s,
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
jstr_memnchr(const void *R s,
	     const int c,
	     const size_t sz,
	     const size_t n)
JSTR_NOEXCEPT
{
	return (void *)memchr(s, c, JSTR_MIN(n, sz));
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
jstr_strnchr(const char *R s,
	     const int c,
	     const size_t n)
JSTR_NOEXCEPT
{
	/* Based on glibc memchr and strchrnul released under the terms of the GNU Lesser General Public License.
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
}

JSTR_FUNC_PURE
JSTR_INLINE
static void *
jstr_memchrnul(const void *R s,
	       const int c,
	       const size_t n)
JSTR_NOEXCEPT
{
	const void *const p = jstr_memrchr(s, c, n);
	return (void *)(p ? p : (char *)s + n);
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_strstr_len(const void *R hs,
		const size_t hs_len,
		const void *R ne,
		const size_t ne_len)
JSTR_NOEXCEPT
{
	return (char *)JSTR_MEMMEM((char *)hs, hs_len, (char *)ne, ne_len);
	(void)hs_len;
	(void)ne_len;
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_strnstr_len(const void *R hs,
		 const size_t hs_len,
		 const void *R ne,
		 const size_t ne_len,
		 const size_t n)
JSTR_NOEXCEPT
{
	return jstr_strstr_len(hs, JSTR_MIN(hs_len, n), ne, ne_len);
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
	     const size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMPCPY
	return mempcpy(dst, src, n);
#else
	return (char *)memcpy(dst, src, n) + n;
#endif
}

JSTR_FUNC
JSTR_INLINE
static void *
jstr_mempmove(void *dst,
	      const void *src,
	      const size_t n)
JSTR_NOEXCEPT
{
	return (char *)memmove(dst, src, n) + n;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_strmove_len(void *dst,
		 const void *src,
		 const size_t n)
JSTR_NOEXCEPT
{
	*(char *)jstr_mempmove(dst, src, n) = '\0';
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
		 const size_t n)
JSTR_NOEXCEPT
{
	jstr_strmove_len(dst, src, n);
	return (char *)dst + n;
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
		const size_t n)
JSTR_NOEXCEPT
{
	*(char *)jstr_mempcpy(dst, src, n) = '\0';
}

JSTR_INLINE
JSTR_FUNC_VOID
static char *
jstr_stpcpy_len(void *R dst,
		const void *R src,
		const size_t n)
JSTR_NOEXCEPT
{
	jstr_strcpy_len(dst, src, n);
	return (char *)dst + n;
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
	     const size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMCCPY
	return memccpy(dst, src, c, n);
#else
	const void *const p = (void *)memchr(src, c, n);
	if (p != NULL) {
		return jstr_stpcpy_len(dst, src, (char *)p - (char *)src);
	}
	memcpy(dst, src, n);
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
	size_t n = strlen(*s);
	char *const p = (char *)malloc(n + 1);
	return (jstr_likely(p != NULL)) ? jstr_stpcpy_len(p, *s, n) : NULL;
}

JSTR_MALLOC
JSTR_FUNC
JSTR_INLINE
static char *
jstr_memdup(const char *R s,
	    const size_t n)
JSTR_NOEXCEPT
{
	char *const p = (char *)malloc(n);
	if (jstr_likely(p != NULL)) {
		memcpy(p, s, n);
		return p;
	}
	return NULL;
}

JSTR_MALLOC
JSTR_FUNC
JSTR_INLINE
static char *
jstr_strdup_len(const char *R s,
		const size_t n)
JSTR_NOEXCEPT
{
	char *const p = (char *)malloc(n + 1);
	if (jstr_likely(p != NULL))
		return jstr_stpcpy_len(p, s, n);
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
jstr_strstrnul_len(const char *R hs,
		   const size_t hs_len,
		   const char *R ne,
		   const size_t ne_len)
JSTR_NOEXCEPT
{
	const char *const p = jstr_strstr_len(hs, hs_len, ne, ne_len);
	return (char *)(p ? p : hs + hs_len);
	(void)ne_len;
}

JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_strstrnul(const char *R hs,
	       const char *R ne)
JSTR_NOEXCEPT
{
	const char *const p = strstr(hs, ne);
	return (char *)(p ? p : hs + strlen(hs));
}

JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_strpbrk(const char *R s,
	     const char *R accept)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRPBRK_OPTIMIZED
	return (char *)strpbrk(s, accept);
#else
	s += strcspn(s, accept);
	return *s ? (char *)s : NULL;
#endif
}

/*
   Non-destructive strtok.
   END must be NUL terminated.
   Instead of nul-termination, use the save_ptr to know the length of the string.
*/
JSTR_FUNC_PURE
static char *
jstr_strtok_ne_len(const char **const save_ptr,
		   const char *const end,
		   const char *R ne,
		   const size_t ne_len)
JSTR_NOEXCEPT
{
	const char *s = *save_ptr;
	if (jstr_unlikely(*s == '\0')) {
		*save_ptr = s;
		return NULL;
	}
	if (!strncmp(s, ne, ne_len))
		s += ne_len;
	if (jstr_unlikely(*s == '\0')) {
		*save_ptr = s;
		return NULL;
	}
	*save_ptr = jstr_strstrnul_len(s, end - s, ne, ne_len);
	return (char *)s;
}

/*
   Non-destructive strtok.
   Instead of nul-termination, use the save_ptr to know the length of the string.
*/
JSTR_FUNC_PURE
static char *
jstr_strtok_ne(const char **const save_ptr,
	       const char *R ne)
JSTR_NOEXCEPT
{
	const char *s = *save_ptr;
	if (jstr_unlikely(*s == '\0')) {
		*save_ptr = s;
		return NULL;
	}
	const size_t ne_len = strlen(ne);
	if (!strncmp(s, ne, ne_len))
		s += ne_len;
	if (jstr_unlikely(*s == '\0')) {
		*save_ptr = s;
		return NULL;
	}
	*save_ptr = jstr_strstrnul(s, ne);
	return (char *)s;
}

/*
   Non-destructive strtok.
   Instead of nul-termination, use the save_ptr to know the length of the string.
*/
JSTR_FUNC_PURE
static char *
jstr_strtok(const char *R *R save_ptr,
	    const char *R delim)
JSTR_NOEXCEPT
{
	const char *s = *save_ptr;
	if (jstr_unlikely(*s == '\0')) {
		*save_ptr = s;
		return NULL;
	}
	s += strspn(s, delim);
	if (jstr_unlikely(*s == '\0')) {
		*save_ptr = s;
		return NULL;
	}
	*save_ptr = s + strcspn(s, delim);
	return (char *)s;
}

JSTR_FUNC_PURE
JSTR_INLINE
static int
jstr_atoi(const char *R s)
JSTR_NOEXCEPT
{
	return strtol(s, NULL, 0);
}

JSTR_FUNC_PURE
JSTR_INLINE
static long
jstr_atol(const char *R s)
JSTR_NOEXCEPT
{
	return strtol(s, NULL, 0);
}

JSTR_FUNC_PURE
JSTR_INLINE
static long long
jstr_atoll(const char *R s)
JSTR_NOEXCEPT
{
	return strtoll(s, NULL, 0);
}

JSTR_FUNC_PURE
JSTR_INLINE
static double
jstr_atod(const char *R s)
JSTR_NOEXCEPT
{
	return strtod(s, NULL);
}

JSTR_FUNC_PURE
JSTR_INLINE
static float
jstr_atof(const char *R s)
JSTR_NOEXCEPT
{
	return strtof(s, NULL);
}

PJSTR_END_DECLS

#undef R

#endif /* JSTR_STD_STRING_H */
