#ifndef JSTR_STD_STRING_H
#define JSTR_STD_STRING_H 1

#include "jstr-macros.h"

P_JSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
P_JSTR_END_DECLS

#define R JSTR_RESTRICT

P_JSTR_BEGIN_DECLS

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
	const size_t len = strlen(src);
	memcpy(dst, src, len);
	dst[len] = '\0';
	return dst + len;
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
		memcpy(dst, src, (u *)p - (u *)src);
		*((u *)dst + ((u *)p - (u *)src)) = '\0';
		return (void *)((u *)dst + ((u *)p - (u *)src));
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
	memcpy(p, s, len);
	p[len] = '\0';
	return p;
#endif
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_strdup_len(size_t *R sz,
		const char *R const src,
		const size_t srclen)
{
	char *const p = (char *)malloc(srclen + 1);
	if (jstr_unlikely(p == NULL))
		return NULL;
	memcpy(p, src, srclen);
	p[srclen] = '\0';
	*sz = srclen;
	return p;
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
	return p ? (char *)p : (char *)(s + strlen(s));
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
	const char *const p = JSTR_STRSTR_LEN(hs, hslen, ne, nelen);
	return p ? (char *)p : (char *)hs + hslen;
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
	return p ? (char *)p : (char *)hs + strlen(hs);
}

/*
   Non-destructive strtok.
   Instead of nul-termination, use the save_ptr to know the length of the string.
*/
JSTR_FUNC_PURE
static char *
jstr_strtok_len(const char *R *R const save_ptr,
		const char *R const end,
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
jstr_strtok(const char *R *R const save_ptr,
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

P_JSTR_END_DECLS

#undef R

#endif /* JSTR_STD_STRING_H */
