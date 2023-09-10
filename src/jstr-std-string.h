#ifndef JSTR_STD_STRING_H
#define JSTR_STD_STRING_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "string-fza.h"
#include "string-fzb.h"
#include "string-fzc.h"
#include "string-fzi.h"
#include "string-misc.h"
#include "string-opthr.h"
#include "string-optype.h"
#include "string-shift.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

JSTR_FUNC_PURE
JSTR_INLINE
static size_t
jstr_strnlen(const char *R const _s,
	     const size_t _maxlen)
{
#if JSTR_HAVE_STRNLEN
	return strnlen(_s, _maxlen);
#else
	const char *p = (char *)memchr(_s, '\0', _maxlen);
	return p ? (size_t)(p - _s) : _maxlen;
#endif
}

/*
  Return value:
  ptr to '\0' in DST.
*/
JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static void *
jstr_mempcpy(void *R const _dst,
	     const void *R const _src,
	     const size_t _n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMPCPY
	return mempcpy(_dst, _src, _n);
#else
	return (char *)memcpy(_dst, _src, _n) + _n;
#endif /* !JSTR_HAVE_STPCPY */
}

/*
  Return value:
  ptr to '\0' in DST.
*/
JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstr_stpcpy(char *R _dst,
	    const char *R _src) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STPCPY
	return stpcpy(_dst, _src);
#else
	const size_t _len = strlen(_src);
	memcpy(_dst, _src, _len);
	*(_dst + _len) = '\0';
	return _dst + _len;
#endif /* !JSTR_HAVE_STPCPY */
}

/* Copy until either N is 0 or C is found */
JSTR_FUNC
JSTR_INLINE
static void *
jstr_memccpy(void *R _dst,
	     const void *R _src,
	     int _c,
	     const size_t _n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMCCPY
	return memccpy(_dst, _src, _c, _n);
#else
	void *p = (void *)memchr(_src, _c, _n);
	if (p != NULL)
		return jstr_mempcpy(_dst, _src, (unsigned char *)p - (unsigned char *)_src + 1);
	memcpy(_dst, _src, _n);
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
jstr_stpcat(char *R _dst,
	    const char *R _src) JSTR_NOEXCEPT
{
	_dst += strlen(_dst);
	return jstr_stpcpy(_dst, _src);
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_strdup(const char *R const _s)
{
#if JSTR_HAVE_STRDUP
	return strdup(_s);
#else
	const size_t _len = strlen(_s) + 1;
	void *_p = malloc(_len);
	if (jstr_unlikely(_p == NULL))
		return NULL;
	memcpy(_p, _s, _len - 1);
	*((char *)_p + _len - 1) = '\0';
	return _p;
#endif
}

JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstr_strchrnul(const char *R const _s,
	       const int _c)
{
#if JSTR_HAVE_STRCHRNUL
	return (char *)strchrnul(_s, _c);
#else
	const char *const _p = strchr(_s, _c);
	return _p ? (char *)_p : (char *)_s + strlen(_s);
#endif
}

PJSTR_END_DECLS

#undef R

#endif /* JSTR_STD_STRING_H */
