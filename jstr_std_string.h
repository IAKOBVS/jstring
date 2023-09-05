#ifndef JSTR_STD_STRING_DEF_H
#define JSTR_STD_STRING_DEF_H 1

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "jstr_macros.h"

#include "string-fza.h"
#include "string-fzb.h"
#include "string-fzc.h"
#include "string-fzi.h"
#include "string-misc.h"
#include "string-opthr.h"
#include "string-optype.h"
#include "string-shift.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

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
	return p ? p - _s : _maxlen;
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
#	if 0 && JSTR_HAVE_ATTR_MAY_ALIAS
	size_t _len = (-(uintptr_t)_dst) % sizeof(pjstr_op_ty);
	for (; _len--; ++_dst)
		if (jstr_unlikely((*_dst++ = *_src++) == '\0'))
			return _dst - 1;
	if ((uintptr_t)_src % sizeof(pjstr_op_ty)) {
		_len = strlen(_src);
		memcpy(_dst, _src, _len);
		return _dst + _len;
	}
	pjstr_op_ty *_dw = (pjstr_op_ty *)_dst;
	pjstr_op_ty *_sw = (pjstr_op_ty *)_src;
	while (!pjstr_has_zero(*_sw))
		*_dw++ = *_sw++;
	_len = pjstr_index_first_zero(*_sw);
	_dst = (char *)_dw;
	_src = (char *)_sw;
	while ((*_dst++ = *_src++))
		;
	return _dst - 1;
#	else
	/* It seems that memcpy() + strlen() is still faster. */
	const size_t _len = strlen(_src);
	memcpy(_dst, _src, _len);
	*(_dst + _len) = '\0';
	return _dst + _len;
#	endif
#endif /* !JSTR_HAVE_STPCPY */
}

/* Copy no more than N bytes of SRC to DEST, stopping when C is found.
   Return the position in DEST one byte past where C was copied, or
   NULL if C was not found in the first N bytes of SRC.  */
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
jstr_stpcat(char *JSTR_RST _dst,
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
#	if 0 && JSTR_HAVE_ATTR_MAY_ALIAS
	pjstr_op_ty *_sw = (pjstr_op_ty *)PJSTR_PTR_ALIGN_DOWN(_s, PJSTR_OPSIZ);
	pjstr_op_ty _cc = pjstr_repeat_bytes(_c);
	pjstr_op_ty _mask = pjstr_shift_find(pjstr_find_zero_eq_all(*_sw, _cc), (uintptr_t)_s);
	if (_mask)
		return (char *)_sw + pjstr_index_first(_mask);
	while ((!pjstr_has_zero_eq(*++_sw, _cc)))
		;
	return (char *)(_sw - 1) + pjstr_index_first_zero_eq(*(_sw - 1), _cc);
#	else
	/* It seems that strchr() + strlen() is still faster. */
	const char *const _p = strchr(_s, _c);
	return _p ? (char *)_p : (char *)_s + strlen(_s);
#	endif
#endif
}

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#endif /* JSTR_STD_STRING_DEF_H */
