#ifndef JSTR_STD_STRING_H
#define JSTR_STD_STRING_H 1

#include "jstr-macros.h"

P_JSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
P_JSTR_END_DECLS

#include "string-fza.h"
#include "string-fzb.h"
#include "string-fzc.h"
#include "string-fzi.h"
#include "string-misc.h"
#include "string-opthr.h"
#include "string-optype.h"
#include "string-shift.h"

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

P_JSTR_END_DECLS

#undef R

#endif /* JSTR_STD_STRING_H */
