#ifndef JSTR_STD_STRING_H
#define JSTR_STD_STRING_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

JSTR_FUNC
JSTR_INLINE
static char *
jstr_strstr_len(const char *R const hs,
		const size_t hslen,
		const char *R const ne,
		const size_t nelen)
{
#if !JSTR_HAVE_MEMMEM
	(void)hslen;
	(void)nelen;
#endif
	return (char *)JSTR_MEMMEM(hs, hslen, ne, nelen);
}

JSTR_FUNC_PURE
JSTR_INLINE
static size_t
jstr_strnlen(const char *R const s,
	     const size_t maxlen)
{
#if JSTR_HAVE_STRNLEN
	return strnlen(s, maxlen);
#else
	const char *const p = (char *)memchr(s, '\0', maxlen);
	return p ? (size_t)(p - s) : maxlen;
#endif
}

/*
  Return value:
  ptr to '\0' in DST.
*/
JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static void *
jstr_mempcpy(void *R const dst,
	     const void *R const src,
	     const size_t n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMPCPY
	return mempcpy(dst, src, n);
#else
	return (char *)memcpy(dst, src, n) + n;
#endif /* !JSTR_HAVE_STPCPY */
}

JSTR_FUNC
JSTR_INLINE
static void *
jstr_mempmove(void *R dst,
	      const void *R src,
	      const size_t n)
{
	return (char *)memmove(dst, src, n) + n;
}

JSTR_FUNC_NOWARN
JSTR_INLINE
static char *
jstr_strmove_len(char *R dst,
		 const char *R src,
		 const size_t n)
{
	*(char *)jstr_mempmove(dst, src, n) = '\0';
	return dst;
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_stpmove_len(char *R dst,
		 const char *R src,
		 const size_t n)
{
	return jstr_strmove_len(dst, src, n) + n;
}

/*
   Avoid memmove if DST == SRC.
*/
JSTR_FUNC
JSTR_INLINE
static char *
jstr_stpmove_len_may_eq(char *R dst,
			const char *R src,
			const size_t n)
{
	return ((dst != src) ? jstr_stpmove_len(dst, src, n) : dst) + n;
}

JSTR_FUNC_NOWARN
JSTR_INLINE
static char *
jstr_strmove(char *R dst,
	     const char *R src)
{
	*(char *)jstr_mempmove(dst, src, strlen(src)) = '\0';
	return dst;
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_stpmove(char *R dst,
	     const char *R src)
{
	return jstr_stpmove_len(dst, src, strlen(src));
}

JSTR_INLINE
JSTR_FUNC_NOWARN
static char *
jstr_strcpy_len(char *R const dst,
		const char *R const src,
		const size_t n)
{
	*(char *)jstr_mempcpy(dst, src, n) = '\0';
	return dst;
}

JSTR_INLINE
JSTR_FUNC_NOWARN
static char *
jstr_stpcpy_len(char *R const dst,
		const char *R const src,
		const size_t n)
{
	return jstr_strcpy_len(dst, src, n) + n;
}

/*
  Return value:
  ptr to '\0' in DST.
*/
JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstr_stpcpy(char *R dst,
	    const char *R src) JSTR_NOEXCEPT
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
	     const size_t n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMCCPY
	return memccpy(dst, src, c, n);
#else
	const void *const p = (void *)memchr(src, c, n);
	if (p != NULL) {
		typedef unsigned char u;
		return jstr_mempcpy(dst, src, (u *)p - (u *)src + 1);
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
	    const char *R src) JSTR_NOEXCEPT
{
	dst += strlen(dst);
	return jstr_stpcpy(dst, src);
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_strdup(const char *R const s)
{
#if JSTR_HAVE_STRDUP
	return strdup(s);
#else
	const size_t len = strlen(s);
	char *const p = (char *)malloc(len + 1);
	if (jstr_unlikely(p == NULL))
		return NULL;
	jstr_strcpy_len(p, s, len);
	return p;
#endif
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_memdup(const char *R const src,
	    const size_t srclen)
{
	char *const p = (char *)malloc(srclen);
	if (jstr_unlikely(p == NULL))
		return NULL;
	memcpy(p, src, srclen);
	return p;
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_strdup_len(const char *R const src,
		const size_t srclen)
{
	return jstr_memdup(src, srclen + 1);
}

JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstr_strchrnul(const char *R const s,
	       const int c)
{
#if JSTR_HAVE_STRCHRNUL
	return (char *)strchrnul(s, c);
#else
	const char *const p = strchr(s, c);
	return (char *)(p ? p : s + strlen(s));
#endif
}

#if defined __GNUC__ || defined __clang__
#	pragma GCC diagnostic ignored "-Wunused-parameter"
#	pragma GCC diagnostic push
#endif

JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstr_strstrnul_len(const char *R const hs,
		   const size_t hslen,
		   const char *R const ne,
		   const size_t nelen)
{
	const char *const p = jstr_strstr_len(hs, hslen, ne, nelen);
	return (char *)(p ? p : hs + hslen);
}

#if defined __GNUC__ || defined __clang__
#	pragma GCC diagnostic pop
#endif

JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstr_strstrnul(const char *R const hs,
	       const char *R const ne)
{
	const char *const p = strstr(hs, ne);
	return (char *)(p ? p : hs + strlen(hs));
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
		   const char *R const ne,
		   const size_t nelen) JSTR_NOEXCEPT
{
	const char *s = *save_ptr;
	if (jstr_unlikely(*s == '\0')) {
		*save_ptr = s;
		return NULL;
	}
	if (!strncmp(s, ne, nelen))
		s += nelen;
	if (jstr_unlikely(*s == '\0')) {
		*save_ptr = s;
		return NULL;
	}
	*save_ptr = jstr_strstrnul_len(s, end - s, ne, nelen);
	return (char *)s;
}

/*
   Non-destructive strtok.
   Instead of nul-termination, use the save_ptr to know the length of the string.
*/
JSTR_FUNC_PURE
static char *
jstr_strtok_ne(const char **const save_ptr,
	       const char *R const ne) JSTR_NOEXCEPT
{
	const char *s = *save_ptr;
	if (jstr_unlikely(*s == '\0')) {
		*save_ptr = s;
		return NULL;
	}
	const size_t nelen = strlen(ne);
	if (!strncmp(s, ne, nelen))
		s += nelen;
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
jstr_strtok(const char *R *R const save_ptr,
	    const char *R const delim) JSTR_NOEXCEPT
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
jstr_atoi(const char *R const s)
{
	return strtol(s, NULL, 0);
}

JSTR_FUNC_PURE
JSTR_INLINE
static long
jstr_atol(const char *R const s)
{
	return strtol(s, NULL, 0);
}

JSTR_FUNC_PURE
JSTR_INLINE
static long long
jstr_atoll(const char *R const s)
{
	return strtoll(s, NULL, 0);
}

JSTR_FUNC_PURE
JSTR_INLINE
static double
jstr_atod(const char *R const s)
{
	return strtod(s, NULL);
}

JSTR_FUNC_PURE
JSTR_INLINE
static float
jstr_atof(const char *R const s)
{
	return strtof(s, NULL);
}

PJSTR_END_DECLS

#undef R

#endif /* JSTR_STD_STRING_H */
