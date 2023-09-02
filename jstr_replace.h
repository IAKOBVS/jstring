#ifndef JSTR_H_REPLACE_DEF
#define JSTR_H_REPLACE_DEF 1

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "jstr_builder.h"
#include "jstr_ctype.h"
#include "jstr_macros.h"
#include "jstr_string.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

JSTR_INLINE
JSTR_NOTHROW
static void
pjstr_rmall_in_place(unsigned char **const _dst,
		     const unsigned char **const _old,
		     const unsigned char **const _p,
		     const size_t _findlen) JSTR_NOEXCEPT
{
	if (jstr_likely(*_dst != *_old))
		memmove(*_dst, *_old, *_p - *_old);
	*_dst += *_p - *_old;
	*_old += *_p - *_old;
	*_old += _findlen;
	*_p += _findlen;
}

JSTR_INLINE
JSTR_NOTHROW
static void
pjstr_rplcall_in_place(unsigned char **const _dst,
		       const unsigned char **const _old,
		       const unsigned char **const _p,
		       const char *JSTR_RST const _rplc,
		       const size_t _rplclen,
		       const size_t _findlen) JSTR_NOEXCEPT
{
	if (jstr_likely(_findlen != _rplclen && *_dst != *_old))
		memmove(*_dst, *_old, *_p - *_old);
	*_dst += *_p - *_old;
	*_old += *_p - *_old;
	*_old += _findlen;
	*_p += _findlen;
	memcpy(*_dst, _rplc, _rplclen);
	*_dst += _rplclen;
}

/*
  Slip SRC into DST[AT].
  Return value:
  pointer to '\0' in S.
  Assumes that S have enough space for SRC.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static char *
pjstr_slip_mem_p_f(char *JSTR_RST const _s,
		   const size_t _at,
		   const char *JSTR_RST const _src,
		   const size_t _sz,
		   const size_t _srclen) JSTR_NOEXCEPT
{
	memmove(_s + _at + _srclen,
		_s + _at,
		_sz - _at + 1);
	memcpy(_s + _at, _src, _srclen);
	return _s + _sz + _srclen;
}

#if JSTR_HAVE_REALLOC_MREMAP

/*
  Slip SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static char *
pjstr_slip_mem_realloc(char **JSTR_RST const _s,
		       size_t *JSTR_RST const _sz,
		       size_t *JSTR_RST const _cap,
		       const size_t _at,
		       const char *JSTR_RST const _rplc,
		       const size_t _rplclen) JSTR_NOEXCEPT
{
	if (*_cap < *_sz + _rplclen)
		PJSTR_REALLOC(*_s, *_cap, *_sz + _rplclen, return NULL);
	return pjstr_slip_mem_p_f(*_s, _at, _rplc, *_sz, _rplclen);
}

/*
  Slip SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static char *
pjstr_slip_mem_malloc(char **JSTR_RST const _s,
		      size_t *JSTR_RST const _sz,
		      size_t *JSTR_RST const _cap,
		      const size_t _at,
		      const char *JSTR_RST const _rplc,
		      const size_t _rplclen) JSTR_NOEXCEPT
{
	if (*_cap > *_sz + _rplclen)
		return pjstr_slip_mem_p_f(*_s, _at, _rplc, *_sz, _rplclen);
	PJSTR_GROW(*_cap, *_sz + _rplclen);
	char *const _tmp = (char *)malloc(*_cap);
	PJSTR_MALLOC_ERR(_tmp, return NULL);
	memcpy(_tmp, *_s, _at);
	memcpy(_tmp + _at, _rplc, _rplclen);
	memcpy(_tmp + _at + _rplclen,
	       *_s + _at,
	       *_sz - _at + 1);
	free(*_s);
	*_s = _tmp;
	*_sz += _rplclen;
	return *_s + _at + _rplclen;
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static char *
pjstr_rplcat_mem_f(char *JSTR_RST const _s,
		   size_t *JSTR_RST const _sz,
		   const size_t _at,
		   const char *JSTR_RST const _rplc,
		   const size_t _rplclen,
		   const size_t _findlen) JSTR_NOEXCEPT
{
	memmove(_s + _at + _rplclen,
		_s + _at + _findlen,
		*_sz - (_at + _findlen) + 1);
	memcpy(_s + _at, _rplc, _rplclen);
	*_sz += _rplclen - _findlen;
	return _s + _at + _rplclen;
}

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static char *
pjstr_rplcat_mem_realloc(char **JSTR_RST const _s,
			 size_t *JSTR_RST const _sz,
			 size_t *JSTR_RST const _cap,
			 const size_t _at,
			 const char *JSTR_RST const _rplc,
			 const size_t _rplclen,
			 const size_t _findlen) JSTR_NOEXCEPT
{
	if (*_cap < *_sz + _rplclen - _findlen)
		PJSTR_REALLOC(*_s, *_cap, *_sz + _rplclen, return NULL);
	return pjstr_rplcat_mem_f(*_s, _sz, _at, _rplc, _rplclen, _findlen);
}

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static char *
pjstr_rplcat_mem_malloc(char **JSTR_RST const _s,
			size_t *JSTR_RST const _sz,
			size_t *JSTR_RST const _cap,
			const size_t _at,
			const char *JSTR_RST const _rplc,
			const size_t _rplclen,
			const size_t _findlen) JSTR_NOEXCEPT
{
	if (*_cap > *_sz + _rplclen - _findlen)
		return pjstr_rplcat_mem_f(*_s, _sz, _at, _rplc, _rplclen, _findlen);
	PJSTR_GROW(*_cap, *_sz + _rplclen - _findlen);
	char *const _tmp = (char *)malloc(*_cap);
	PJSTR_MALLOC_ERR(_tmp, return NULL);
	memcpy(_tmp, *_s, _at);
	memcpy(_tmp + _at, _rplc, _rplclen);
	memcpy(_tmp + _at + _rplclen,
	       *_s + _at + _findlen,
	       *_sz - (_at + _findlen) + 1);
	free(*_s);
	*_sz += _rplclen - _findlen;
	*_s = _tmp;
	return _tmp + _at + _rplclen;
}

#endif /* HAVE_REALLOC_MREMAP */

/*
  Slip SRC into DST[AT].
*/
JSTR_NONNULL_ALL
JSTR_NOTHROW
static char *
jstr_slip_mem(char **JSTR_RST const _s,
	      size_t *JSTR_RST const _sz,
	      size_t *JSTR_RST const _cap,
	      const size_t _at,
	      const char *JSTR_RST const _rplc,
	      const size_t _rplclen) JSTR_NOEXCEPT
{
#if JSTR_HAVE_REALLOC_MREMAP
	if (PJSTR_IS_MMAP(*_cap))
		return pjstr_slip_mem_realloc(_s, _sz, _cap, _at, _rplc, _rplclen);
#endif /* HAVE_REALLOC_MREMAP */
	return pjstr_slip_mem_malloc(_s, _sz, _cap, _at, _rplc, _rplclen);
}

JSTR_NONNULL_ALL
JSTR_NOTHROW
static char *
pjstr_rplcat_mem_may_lower(char **JSTR_RST const _s,
			   size_t *JSTR_RST const _sz,
			   size_t *JSTR_RST const _cap,
			   const size_t _at,
			   const char *JSTR_RST const _rplc,
			   const size_t _rplclen,
			   const size_t _findlen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_rplclen == _findlen)) {
		memcpy(*_s + _at, _rplc, _rplclen);
		goto RET;
	} else if (*_cap > *_sz + _rplclen - _findlen) {
		return pjstr_rplcat_mem_f(*_s, _sz, _at, _rplc, _rplclen, _findlen);

	} else {
#if JSTR_HAVE_REALLOC_MREMAP
		if (PJSTR_IS_MMAP(*_cap)) {
			PJSTR_REALLOC(*_s, *_cap, *_sz + _rplclen - _findlen, return NULL);
			return pjstr_rplcat_mem_f(*_s, _sz, _at, _rplc, _rplclen, _findlen);
		}
#else
		PJSTR_GROW(*_cap, *_sz + _rplclen - _findlen);
		char *const _tmp = (char *)malloc(*_cap);
		PJSTR_MALLOC_ERR(_tmp, return NULL);
		memcpy(_tmp, *_s, _at);
		memcpy(_tmp + _at, _rplc, _rplclen);
		memcpy(_tmp + _at + _rplclen,
		       *_s + _at + _findlen,
		       *_sz - _at + _findlen + 1);
		free(*_s);
		*_s = _tmp;
#endif /* HAVE_REALLOC_MREMAP */
	}
	*_sz += _rplclen - _findlen;
RET:
	return *_s + _at + _rplclen;
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static char *
pjstr_rplcat_mem(char **JSTR_RST const _s,
		 size_t *JSTR_RST const _sz,
		 size_t *JSTR_RST const _cap,
		 const size_t _at,
		 const char *JSTR_RST const _rplc,
		 const size_t _rplclen,
		 const size_t _findlen) JSTR_NOEXCEPT
{
#if JSTR_HAVE_REALLOC_MREMAP
	if (PJSTR_IS_MMAP(*_cap))
		return pjstr_rplcat_mem_realloc(_s, _sz, _cap, _at, _rplc, _rplclen, _findlen);
#endif /* HAVE_REALLOC_MREMAP */
	return pjstr_rplcat_mem_malloc(_s, _sz, _cap, _at, _rplc, _rplclen, _findlen);
}

/*
  Slip SRC after C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_slipaftchr_mem(char **JSTR_RST const _s,
		    size_t *JSTR_RST const _sz,
		    size_t *JSTR_RST const _cap,
		    const int _c,
		    const char *JSTR_RST const _src,
		    const size_t _srclen) JSTR_NOEXCEPT
{
	const char *const _p = (char *)memchr(*_s, _c, *_sz);
	if (_p != NULL)
		jstr_slip_mem(_s, _sz, _cap, _p - *_s + 1, _src, _srclen);
}

/*
  Slip SRC after all C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static char *
pjstr_slipaftallchr_mem_p_f(char *JSTR_RST const _s,
			    const int _c,
			    const char *JSTR_RST const _src,
			    size_t _sz,
			    const size_t _srclen) JSTR_NOEXCEPT
{
	size_t off = 0;
	const char *_p;
	while ((_p = (char *)memchr(_s + off, _c, _sz - off - *_s))) {
		off = _p - _s;
		_sz = pjstr_slip_mem_p_f(_s, off, _src, _sz, _srclen) - _s;
		off += _srclen + 1;
	}
	return _s + _sz;
}

/*
  Slip SRC after all C in DST.
*/
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_slipaftallchr_mem(char **JSTR_RST const _s,
		       size_t *JSTR_RST const _sz,
		       size_t *JSTR_RST const _cap,
		       const int _c,
		       const char *JSTR_RST const _src,
		       const size_t _srclen) JSTR_NOEXCEPT
{
	size_t off = 0;
	const char *_p;
#if JSTR_HAVE_REALLOC_MREMAP
	const int is_mmap = PJSTR_IS_MMAP(*_cap);
#endif /* HAVE_REALLOC_MREMAP */
	while ((_p = (char *)memchr(*_s + off, _c, *_sz - off))) {
		off = _p - *_s;
#if JSTR_HAVE_REALLOC_MREMAP
		if (jstr_unlikely(is_mmap))
			pjstr_slip_mem_realloc(_s, _sz, _cap, off, _src, _srclen);
		else
#endif /* HAVE_REALLOC_MREMAP */
			pjstr_slip_mem_malloc(_s, _sz, _cap, off, _src, _srclen);
		off += _srclen + 1;
	}
}

/*
  Slip SRC after _end of NE in DST.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_NOTHROW
static void
jstr_slipaft_mem(char **JSTR_RST const _s,
		 size_t *JSTR_RST const _sz,
		 size_t *JSTR_RST const _cap,
		 const char *JSTR_RST const _find,
		 const char *JSTR_RST const _src,
		 const size_t _findlen,
		 const size_t _srclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_findlen == 1)) {
		jstr_slipaftchr_mem(_s, _sz, _cap, *_find, _src, _srclen);
		return;
	}
	if (jstr_unlikely(_findlen == 0))
		return;
	const char *const _p = (char *)PJSTR_MEMMEM(*_s, *_sz, _find, _findlen);
	if (_p != NULL)
		jstr_slip_mem(_s, _sz, _cap, _p - *_s + _findlen, _src, _srclen);
}

/*
  Slip SRC after all _end of NE in DST.
*/
JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_slipaftall_mem(char **JSTR_RST const _s,
		    size_t *JSTR_RST const _sz,
		    size_t *JSTR_RST const _cap,
		    const char *JSTR_RST const _find,
		    const char *JSTR_RST const _src,
		    const size_t _findlen,
		    const size_t _srclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_findlen == 1)) {
		jstr_slipaftallchr_mem(_s, _sz, _cap, *_find, _src, _srclen);
		return;
	}
	if (jstr_unlikely(_findlen == 0))
		return;
	size_t off = 0;
	const char *_p;
#if JSTR_HAVE_REALLOC_MREMAP
	const int is_mmap = PJSTR_IS_MMAP(*_cap);
#endif /* HAVE_REALLOC_MREMAP */
	while ((_p = (char *)PJSTR_MEMMEM(*_s + off, *_sz - off, _find, _findlen))) {
		off = _p - *_s;
#if JSTR_HAVE_REALLOC_MREMAP
		if (jstr_unlikely(is_mmap))
			pjstr_slip_mem_realloc(_s, _sz, _cap, _p - *_s + _findlen, _src, _srclen);
		else
#endif /* HAVE_REALLOC_MREMAP */
			pjstr_slip_mem_malloc(_s, _sz, _cap, _p - *_s + _findlen, _src, _srclen);
		off += _findlen + _srclen;
	}
}

/*
  Remove first C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static char *
jstr_rmchr_mem_p(char *JSTR_RST _s,
		 const int _c,
		 const size_t _sz) JSTR_NOEXCEPT
{
	const char *const _start = _s;
	_s = (char *)memchr(_s, _c, _sz);
	if (jstr_unlikely(_s == NULL))
		return _s + _sz;
	memmove(_s, _s + 1, _sz - (_s - _start) + 1);
	return _s + _sz - (_s - _start);
}

/*
  Remove first C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static char *
jstr_rmchr_p(char *JSTR_RST _s,
	     const int _c) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	_s = strchrnul(_s, _c);
	if (jstr_unlikely(*_s == '\0'))
		return _s;
	size_t len = strlen(_s);
	memmove(_s, _s + 1, len);
	return _s + len - 1;
#else
	return jstr_rmchr_mem_p(_s, _c, strlen(_s));
#endif /* HAVE_STRCHRNUL */
}

/*
  Remove all REJECT in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static char *
jstr_rmspn_p(char *JSTR_RST _s,
	     const char *JSTR_RST _reject) JSTR_NOEXCEPT
{
	unsigned char *_dst = (unsigned char *)_s;
	const unsigned char *_old = _dst;
	const unsigned char *_p = _dst;
	while (jstr_likely(*(_p += strcspn((char *)_p, _reject)))) {
		pjstr_rmall_in_place(&_dst, &_old, &_p, strspn((char *)_p, _reject));
		if (jstr_unlikely(*_p == '\0'))
			break;
	}
	if (jstr_likely(_dst != _old))
		memmove(_dst, _old, _p - _old + 1);
	return (char *)_dst + (_p - _old);
}

/*
  Remove REJECT in S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_rmspn_j(jstr_ty *JSTR_RST const _j,
	     const char *JSTR_RST _reject) JSTR_NOEXCEPT
{
	_j->size = jstr_rmspn_p(_j->data, _reject) - _j->data;
}

typedef enum {
	PJSTR_FLAG_USE_N = 1,
	PJSTR_FLAG_USE_NOT_N = 1 << 1,
} pjstr_flag_use_n_ty;

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static char *
pjstr_rmallchr_mem_p(const pjstr_flag_use_n_ty _flag,
		     char *JSTR_RST const _s,
		     const int _c,
		     size_t _n,
		     const size_t _sz) JSTR_NOEXCEPT
{
	unsigned char *_dst = (unsigned char *)_s;
	const unsigned char *_old = _dst;
	const unsigned char *_p = _dst;
	const unsigned char *const _end = _dst + _sz;
	while ((_flag & PJSTR_FLAG_USE_N ? jstr_likely(_n--) : 1)
	       && (_p = (unsigned char *)memchr(_p, _c, _end - _p)))
		pjstr_rmall_in_place(&_dst, &_old, &_p, 1);
	memmove(_dst, _old, _end - _old + 1);
	return (char *)_dst + (_end - _old);
}

/*
  Remove all C in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
JSTR_MAYBE_UNUSED
static char *
jstr_rmallchr_mem_p(char *JSTR_RST const _s,
		    const int _c,
		    const size_t _sz) JSTR_NOEXCEPT
{
	return pjstr_rmallchr_mem_p(PJSTR_FLAG_USE_NOT_N, _s, _c, 0, _sz);
}

/*
  Remove all C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static char *
jstr_rmallchr_p(char *JSTR_RST const _s,
		const int _c) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	unsigned char *_dst = (unsigned char *)_s;
	const unsigned char *_old = _dst;
	const unsigned char *_p = _dst;
	while (*(_p = (unsigned char *)strchrnul((char *)_p, _c)))
		pjstr_rmall_in_place(&_dst, &_old, &_p, 1);
	memmove(_dst, _old, _p - _old + 1);
	return (char *)_dst + (_p - _old);
#else
	return jstr_rmallchr_mem_p(_s, _c, strlen(_s));
#endif /* HAVE_STRCHRNUL */
}

/*
  Remove N C in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
JSTR_MAYBE_UNUSED
static char *
jstr_rmnc_mem_p(char *JSTR_RST const _s,
		const int _c,
		const size_t _n,
		const size_t _sz) JSTR_NOEXCEPT
{
	return pjstr_rmallchr_mem_p(PJSTR_FLAG_USE_N, _s, _c, _n, _sz);
}

/*
  Remove N C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static char *
jstr_rmnc_p(char *JSTR_RST const _s,
	    const int _c,
	    size_t _n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	unsigned char *_dst = (unsigned char *)_s;
	const unsigned char *_old = _dst;
	const unsigned char *_p = _dst;
	while (jstr_likely(_n--) && *(_p = (unsigned char *)strchrnul((char *)_p, _c)))
		pjstr_rmall_in_place(&_dst, &_old, &_p, 1);
	memmove(_dst, _old, _p - _old + 1);
	return (char *)_dst + (_p - _old);
#else
	return jstr_rmnc_mem_p(_s, _c, _n, strlen(_s));
#endif /* HAVE_STRCHRNUL */
}

/*
  Remove characters in REJECT in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static char *
jstr_stripspn_p(char *JSTR_RST const _s,
		const char *JSTR_RST const _rjct) JSTR_NOEXCEPT
{
	unsigned char *_dst = (unsigned char *)_s;
	const unsigned char *_old = _dst;
	const unsigned char *_p = _dst;
	while (*(_p += strcspn((char *)_p, _rjct)))
		pjstr_rmall_in_place(&_dst, &_old, &_p, 1);
	memmove(_dst, _old, _p - _old + 1);
	return (char *)_dst + (_p - _old);
}

/*
  Remove first HS in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static char *
jstr_rm_mem_p(char *JSTR_RST const _s,
	      const char *JSTR_RST const _find,
	      const size_t _sz,
	      const size_t _findlen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_findlen == 0))
		return _s + _sz;
	char *const _p = (char *)PJSTR_MEMMEM(_s, _sz, _find, _findlen);
	if (jstr_unlikely(_p == NULL))
		return _s + _sz;
	memmove(_p, _p + _findlen, (_s + _sz) - _p);
	return _s + _sz - _findlen;
}

/*
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_rplcchr_mem(char *JSTR_RST _s,
		 const int _find,
		 const int _rplc,
		 const size_t _sz) JSTR_NOEXCEPT
{
	_s = (char *)memchr(_s, _find, _sz);
	if (jstr_unlikely(_s == NULL))
		return;
	*_s = _rplc;
}

/*
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_rplcchr(char *JSTR_RST _s,
	     const int _find,
	     const int _rplc) JSTR_NOEXCEPT
{
	_s = strchr(_s, _find);
	if (jstr_unlikely(_s == NULL))
		return;
	*_s = _rplc;
}

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_NOTHROW
static void
jstr_rplcallchr_mem(char *JSTR_RST _s,
		    const int _find,
		    const int _rplc,
		    const size_t _sz) JSTR_NOEXCEPT
{
	const char *JSTR_RST const _end = _s + _sz;
	while ((_s = (char *)memchr(_s, _find, _end - _s)))
		*_s++ = _rplc;
}

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_NOTHROW
static void
jstr_rplcallchr(char *JSTR_RST _s,
		const int _find,
		const int _rplc) JSTR_NOEXCEPT
{
	while ((_s = (strchr(_s, _find))))
		*_s++ = _rplc;
}

/*
  Replace N SEARCH in REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_NOTHROW
static void
jstr_rplcnc_mem(char *JSTR_RST _s,
		const int _find,
		const int _rplc,
		size_t _n,
		const size_t _sz) JSTR_NOEXCEPT
{
	const char *JSTR_RST const _end = _s + _sz;
	while (jstr_likely(_n--) && (_s = (char *)memchr(_s, _find, _end - _s)))
		*_s++ = _rplc;
}

/*
  Replace N SEARCH in REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_NOTHROW
static void
jstr_rplcnc(char *JSTR_RST _s,
	    const int _find,
	    const int _rplc,
	    size_t _n) JSTR_NOEXCEPT
{
	while (jstr_likely(_n--) && (_s = strchr(_s, _find)))
		*_s++ = _rplc;
}

/*
  Replace first SEARCH in S with REPLACE.
*/
JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_rplc_mem(char **JSTR_RST const _s,
	      size_t *JSTR_RST const _sz,
	      size_t *JSTR_RST const _cap,
	      const char *JSTR_RST const _find,
	      const char *JSTR_RST const _rplc,
	      const size_t _findlen,
	      const size_t _rplclen) JSTR_NOEXCEPT
{
	switch (_rplclen) {
	case 0:
		*_sz = jstr_rm_mem_p(*_s, _find, *_sz, _findlen) - *_s;
		return;
	case 1:
		if (jstr_unlikely(_findlen == 1)) {
			jstr_rplcchr_mem(*_s, *_find, *_rplc, *_sz);
			return;
		}
		/* fallthrough */
	default: {
		if (jstr_unlikely(_findlen == 0))
			return;
		char *_p = (char *)PJSTR_MEMMEM(*_s, *_sz, _find, _findlen);
		if (jstr_unlikely(_p == NULL))
			return;
		jstr_slip_mem(_s, _sz, _cap, _p - *_s, _rplc, _rplclen);
	}
	}
}

/*
  Replace last SEARCH in S with REPLACE.
*/
JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_rplclast_mem(char **JSTR_RST const _s,
		  size_t *JSTR_RST const _sz,
		  size_t *JSTR_RST const _cap,
		  const char *JSTR_RST const _find,
		  const char *JSTR_RST const _rplc,
		  const size_t _findlen,
		  const size_t _rplclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_findlen == 0))
		return;
	char *_p = (char *)jstr_strrstr_mem(*_s, *_sz, _find, _findlen);
	if (jstr_unlikely(_p == NULL))
		return;
	pjstr_rplcat_mem_may_lower(_s, _sz, _cap, _p - *_s, _rplc, _rplclen, _findlen);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
JSTR_INLINE
JSTR_NOTHROW
static char *
pjstr_rmall_mem_p(const pjstr_flag_use_n_ty _flag,
		  char *JSTR_RST const _s,
		  const char *JSTR_RST const _find,
		  size_t _n,
		  size_t _sz,
		  const size_t _findlen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_findlen == 1))
		return pjstr_rmallchr_mem_p(_flag, _s, *_find, _n, _sz);
	if (jstr_unlikely(_findlen == 0))
		return _s + _sz;
	if (_flag & PJSTR_FLAG_USE_N)
		if (jstr_unlikely(_n == 0))
			return _s + _sz;
	unsigned char *_dst = (unsigned char *)_s;
	const unsigned char *_old = _dst;
	const unsigned char *_p = _dst;
	const unsigned char *const _end = _dst + _sz;
	while ((_p = (unsigned char *)PJSTR_MEMMEM((char *)_p, _end - _p, _find, _findlen))) {
		pjstr_rmall_in_place(&_dst, &_old, &_p, _findlen);
		if (_flag & PJSTR_FLAG_USE_N)
			if (jstr_unlikely(!--_n))
				break;
	}
	memmove(_dst, _old, _end - _old + 1);
	return (char *)_dst + (_end - _old);
}

/*
  Remove N SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
JSTR_MAYBE_UNUSED
JSTR_NOTHROW
static char *
jstr_rmn_mem_p(char *JSTR_RST const _s,
	       const char *JSTR_RST const _find,
	       size_t _n,
	       size_t _sz,
	       const size_t _findlen) JSTR_NOEXCEPT
{
	return pjstr_rmall_mem_p(PJSTR_FLAG_USE_N, _s, _find, _n, _sz, _findlen);
}

/*
  Remove all SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
JSTR_MAYBE_UNUSED
JSTR_NOTHROW
static char *
jstr_rmall_mem_p(char *JSTR_RST const _s,
		 const char *JSTR_RST const _find,
		 size_t _sz,
		 const size_t _findlen) JSTR_NOEXCEPT
{
	return pjstr_rmall_mem_p(PJSTR_FLAG_USE_NOT_N, _s, _find, 0, _sz, _findlen);
}

/*
  Remove all SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
JSTR_INLINE
JSTR_NOTHROW
static char *
jstr_rmall_p(char *JSTR_RST const _s,
	     const char *JSTR_RST const _find,
	     const size_t _findlen) JSTR_NOEXCEPT
{
	return jstr_rmall_mem_p(_s, _find, strlen(_s), _findlen);
}

/*
  Remove all SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_NOTHROW
static void
jstr_rmall_mem_j(jstr_ty *JSTR_RST const _j,
		 const char *JSTR_RST const _find,
		 const size_t _findlen) JSTR_NOEXCEPT
{
	_j->size = jstr_rmall_mem_p(_j->data, _find, _j->size, _findlen) - _j->data;
}

/*
  Remove all SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_NOTHROW
static void
jstr_rmall_j(jstr_ty *JSTR_RST const _j,
	     const char *JSTR_RST const _find) JSTR_NOEXCEPT
{
	return jstr_rmall_mem_j(_j, _find, strlen(_find));
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
pjstr_rplcall_mem(const pjstr_flag_use_n_ty _flag,
		  char **JSTR_RST const _s,
		  size_t *JSTR_RST const _sz,
		  size_t *JSTR_RST const _cap,
		  const char *JSTR_RST const _find,
		  const char *JSTR_RST const _rplc,
		  size_t _n,
		  const size_t _findlen,
		  const size_t _rplclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_rplclen == 0)) {
		*_sz = pjstr_rmall_mem_p(_flag, *_s, _find, _n, *_sz, _findlen) - *_s;
		return;
	}
	if (jstr_unlikely(_findlen == 1 && _rplclen == 1)) {
		jstr_rplcchr_mem(*_s, *_find, *_rplc, *_sz);
		return;
	}
	if (jstr_unlikely(_findlen == 0))
		return;
	if (_flag & PJSTR_FLAG_USE_N)
		if (jstr_unlikely(_n == 0))
			return;
	typedef unsigned char uc;
	const unsigned char *_p = *(uc **)_s;
	const unsigned char *_old = _p;
	unsigned char *_dst = *(uc **)_s;
#if JSTR_HAVE_REALLOC_MREMAP
	const int is_mmap = PJSTR_IS_MMAP(*_cap);
#endif /* HAVE_REALLOC_MREMAP */
	while ((_p = (uc *)PJSTR_MEMMEM((char *)_p, (*(uc **)_s + *_sz) - _p, _find, _findlen))) {
		if (_rplclen <= _findlen)
			pjstr_rplcall_in_place(&_dst, &_old, &_p, _rplc, _rplclen, _findlen);
#if JSTR_HAVE_REALLOC_MREMAP
		else if (jstr_unlikely(is_mmap))
			_p = (uc *)pjstr_rplcat_mem_realloc(_s, _sz, _cap, _p - *(uc **)_s, _rplc, _rplclen, _findlen);
#endif /* HAVE_REALLOC_MREMAP */
		else
			_p = (uc *)pjstr_rplcat_mem_malloc(_s, _sz, _cap, _p - *(uc **)_s, _rplc, _rplclen, _findlen);
		if (_flag & PJSTR_FLAG_USE_N)
			if (jstr_unlikely(!--_n))
				break;
	}
	if (_rplclen < _findlen) {
		memmove(_dst, _old, *(uc **)_s + *_sz - _old + 1);
		*_sz = (char *)_dst + (*(uc **)_s + *_sz - _old) - *_s;
	}
}

/*
  Replace N SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_NOTHROW
static void
jstr_rplcn_mem(char **JSTR_RST const _s,
	       size_t *JSTR_RST const _sz,
	       size_t *JSTR_RST const _cap,
	       const char *JSTR_RST const _find,
	       const char *JSTR_RST const _rplc,
	       size_t _n,
	       const size_t _findlen,
	       const size_t _rplclen) JSTR_NOEXCEPT
{
	pjstr_rplcall_mem(PJSTR_FLAG_USE_N, _s, _sz, _cap, _find, _rplc, _n, _findlen, _rplclen);
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_NOTHROW
static void
jstr_rplcall_mem(char **JSTR_RST const _s,
		 size_t *JSTR_RST const _sz,
		 size_t *JSTR_RST const _cap,
		 const char *JSTR_RST const _find,
		 const char *JSTR_RST const _rplc,
		 const size_t _findlen,
		 const size_t _rplclen) JSTR_NOEXCEPT
{
	pjstr_rplcall_mem(PJSTR_FLAG_USE_NOT_N, _s, _sz, _cap, _find, _rplc, 0, _findlen, _rplclen);
}

/*
  Reverse S.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_NOTHROW
static void
jstr_rev_mem(char *JSTR_RST _s,
	     const size_t _sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_s == '\0'))
		return;
	unsigned char *_end = (unsigned char *)_s + _sz - 1;
	unsigned char *_p = (unsigned char *)_s;
	unsigned char _tmp;
	do {
		_tmp = *_p;
		*_p = *_end;
		*_end = _tmp;
	} while (jstr_likely(++_p < --_end));
}

/*
  Trim leading and trailing [\n\t\v\r ] in S.
  Return value:
  pointer to '\0' in S;
  S if SLEN is 0.
*/
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static char *
jstr_trim_mem_p(char *JSTR_RST const _s,
		const size_t _sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_s == '\0'))
		return _s + _sz;
	const size_t _mv = strspn(_s, " \n\t\v\r");
	if (_mv)
		memmove(_s, _s + _mv, (_sz - _mv) + 1);
	unsigned char *_end = (unsigned char *)_s + (_sz - _mv) - 1;
	const unsigned char *const _start = (unsigned char *)_s - 1;
	while (jstr_isspace(*_end)
	       && --_end != _start)
		;
	*++_end = '\0';
	return (char *)_end;
}

/*
  Trim leading and trailing [\n\t\v\r ] in S.
  Return value:
  pointer to '\0' in S;
  S if SLEN is 0.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_NOTHROW
static char *
jstr_trim_p(char *JSTR_RST const _s) JSTR_NOEXCEPT
{
	return jstr_trim_mem_p(_s, strlen(_s));
}

/*
  Trim leading and trailing [\n\t\v\r ] in S.
  Return value:
  pointer to '\0' in S;
  S if SLEN is 0.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_trim_j(jstr_ty *JSTR_RST const _j) JSTR_NOEXCEPT
{
	_j->size = jstr_trim_mem_p(_j->data, _j->size) - _j->data;
}

/*
  Insert SRC into DST[AT].
  Assumes that S have enough space for SRC.
  Return value:
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
pjstr_insert_mem_f(char *JSTR_RST const _s,
		   const size_t _at,
		   const char *JSTR_RST const _src,
		   const size_t _srclen) JSTR_NOEXCEPT
{
	memcpy(_s + _at, _src, _srclen);
}

/*
  Insert SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_insert_mem(char **JSTR_RST const _s,
		size_t *JSTR_RST const _sz,
		size_t *JSTR_RST const _cap,
		const size_t _at,
		const char *JSTR_RST const _src,
		const size_t _srclen) JSTR_NOEXCEPT
{
	if (_at + _srclen > *_sz) {
		PJSTR_REALLOC(*_s, *_cap, _at + _srclen + 1, return);
		*_sz = _at + _srclen;
		*(*_s + *_sz) = '\0';
	}
	pjstr_insert_mem_f(*_s, _at, _src, _srclen);
}

/*
  Insert SRC after C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_insertaftchr_mem(char **JSTR_RST const _s,
		      size_t *JSTR_RST const _sz,
		      size_t *JSTR_RST const _cap,
		      const int _c,
		      const char *JSTR_RST const _src,
		      const size_t _srclen) JSTR_NOEXCEPT
{
	const char *const _p = (char *)memchr(*_s, _c, *_sz);
	if (_p != NULL)
		jstr_insert_mem(_s, _sz, _cap, _p - *_s + 1, _src, _srclen);
}

/*
  Insert SRC after _end of NE in DST.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_NOTHROW
static void
jstr_insertaft_mem(char **JSTR_RST const _s,
		   size_t *JSTR_RST const _sz,
		   size_t *JSTR_RST const _cap,
		   const char *JSTR_RST const _find,
		   const char *JSTR_RST const _src,
		   const size_t _findlen,
		   const size_t _srclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_findlen == 1)) {
		jstr_insertaftchr_mem(_s, _sz, _cap, *_find, _src, _srclen);
		return;
	}
	if (jstr_unlikely(_findlen == 0))
		return;
	const char *const _p = (char *)PJSTR_MEMMEM(*_s, *_sz, _find, _findlen);
	if (_p != NULL)
		jstr_insert_mem(_s, _sz, _cap, _p - *_s + _findlen, _src, _srclen);
}

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#endif /* JSTR_H_REPLACE_DEF */
