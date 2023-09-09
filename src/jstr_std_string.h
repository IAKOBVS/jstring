#ifndef JSTR_STD_STRING_H
#define JSTR_STD_STRING_H 1

#include "jstr_macros.h"

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

PJSTR_BEGIN_DECLS

JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_INLINE
JSTR_NOTHROW
JSTR_PURE
static size_t
jstr_strnlen(const char *JSTR_RST const _s,
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
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static void *
jstr_mempcpy(void *JSTR_RST const _dst,
	     const void *JSTR_RST const _src,
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
#if JSTR_HAVE_STPCPY
JSTR_INLINE
#endif
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static char *
jstr_stpcpy(char *JSTR_RST _dst,
	    const char *JSTR_RST _src) JSTR_NOEXCEPT
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
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_INLINE
JSTR_NOTHROW
static void *
jstr_memccpy(void *JSTR_RST _dst,
	     const void *JSTR_RST _src,
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
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static char *
jstr_stpappendmore(char *JSTR_RST _dst,
	    const char *JSTR_RST _src) JSTR_NOEXCEPT
{
	_dst += strlen(_dst);
	return jstr_stpcpy(_dst, _src);
}

JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_INLINE
JSTR_NOTHROW
static char *
jstr_stpdup(const char *JSTR_RST const _s,
	    size_t *JSTR_RST const _sz)
{
	*_sz = strlen(_s);
	void *_p = malloc(*_sz + 1);
	if (jstr_unlikely(_p == NULL))
		return NULL;
	return (char *)memcpy(_p, _s, *_sz + 1);
}

JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_INLINE
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static char *
jstr_strchrnul(const char *JSTR_RST const _s,
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

#endif /* JSTR_STD_STRING_H */
