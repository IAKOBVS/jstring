#ifndef JSTR_H_REPLACE_DEF
#define JSTR_H_REPLACE_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "_jstr_builder.h"
#include "_jstr_macros.h"
#include "_jstr_memmem.h"
#include "_jstr_string.h"

#ifdef __cplusplus
extern "C" {
#endif /* JSTR_NAMESPACE */

#define JSTR_ASCII_SIZE 256

typedef enum {
	PRIVATE_JSTR_FLAG_USE_N = 1,
	PRIVATE_JSTR_FLAG_USE_NOT_N = 1 << 1,
} priv_jstr_flag_ty;

JSTR_INLINE
static void
priv_jstr_rmall_in_place(unsigned char **const _dst,
			 const unsigned char **const _old,
			 const unsigned char **const _p,
			 const size_t _searclen) JSTR_NOEXCEPT
{
	if (jstr_likely(*_dst != *_old))
		memmove(*_dst, *_old, *_p - *_old);
	*_dst += (*_p - *_old);
	*_old += (*_p - *_old);
	*_old += _searclen;
	*_p += _searclen;
}

JSTR_INLINE
static void
priv_jstr_rplcall_in_place(unsigned char **const _dst,
			   const unsigned char **const _old,
			   const unsigned char **const _p,
			   const char *JSTR_RST const _rplc,
			   const size_t _rplclen,
			   const size_t _searclen) JSTR_NOEXCEPT
{
	if (jstr_likely(_searclen != _rplclen && *_dst != *_old))
		memmove(*_dst, *_old, *_p - *_old);
	*_dst += (*_p - *_old);
	*_old += (*_p - *_old);
	*_old += _searclen;
	*_p += _searclen;
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
static char *
jstr_slip_mem_p_f(char *JSTR_RST const _s,
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
static char *
priv_jstr_slip_mem_realloc(char **JSTR_RST const _s,
			   size_t *JSTR_RST const _sz,
			   size_t *JSTR_RST const _cap,
			   const size_t _at,
			   const char *JSTR_RST const _rplc,
			   const size_t _rplclen) JSTR_NOEXCEPT
{
	if (*_cap < *_sz + _rplclen)
		JSTR_REALLOC(*_s, *_cap, *_sz + _rplclen, return NULL);
	return jstr_slip_mem_p_f(*_s, _at, _rplc, *_sz, _rplclen);
}

/*
  Slip SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static char *
priv_jstr_slip_mem_malloc(char **JSTR_RST const _s,
			  size_t *JSTR_RST const _sz,
			  size_t *JSTR_RST const _cap,
			  const size_t _at,
			  const char *JSTR_RST const _rplc,
			  const size_t _rplclen) JSTR_NOEXCEPT
{
	if (*_cap > *_sz + _rplclen)
		return jstr_slip_mem_p_f(*_s, _at, _rplc, *_sz, _rplclen);
	JSTR_GROW(*_cap, *_sz + _rplclen);
	char *const tmp = (char *)malloc(*_cap);
	JSTR_MALLOC_ERR(tmp, return NULL);
	memcpy(tmp, *_s, _at);
	memcpy(tmp + _at, _rplc, _rplclen);
	memcpy(tmp + _at + _rplclen,
	       *_s + _at,
	       *_sz - _at + 1);
	free(*_s);
	*_s = tmp;
	*_sz += _rplclen;
	return *_s + _at + _rplclen;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static char *
jstr_rplcat_mem_f(char *JSTR_RST const _s,
		  size_t *JSTR_RST const _sz,
		  const size_t _at,
		  const char *JSTR_RST const _rplc,
		  const size_t _rplclen,
		  const size_t _searclen) JSTR_NOEXCEPT
{
	memmove(_s + _at + _rplclen,
		_s + _at + _searclen,
		*_sz - (_at + _searclen) + 1);
	memcpy(_s + _at, _rplc, _rplclen);
	*_sz += _rplclen - _searclen;
	return _s + _at + _rplclen;
}

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_NONNULL_ALL
static char *
priv_jstr_rplcat_mem_realloc(char **JSTR_RST const _s,
			     size_t *JSTR_RST const _sz,
			     size_t *JSTR_RST const _cap,
			     const size_t _at,
			     const char *JSTR_RST const _rplc,
			     const size_t _rplclen,
			     const size_t _searclen) JSTR_NOEXCEPT
{
	if (*_cap > *_sz + _rplclen - _searclen)
		JSTR_REALLOC(*_s, *_cap, *_sz + _rplclen, return NULL);
	return jstr_rplcat_mem_f(*_s, _sz, _at, _rplc, _rplclen, _searclen);
}

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_NONNULL_ALL
static char *
priv_jstr_rplcat_mem_malloc(char **JSTR_RST const _s,
			    size_t *JSTR_RST const _sz,
			    size_t *JSTR_RST const _cap,
			    const size_t _at,
			    const char *JSTR_RST const _rplc,
			    const size_t _rplclen,
			    const size_t _searclen) JSTR_NOEXCEPT
{
	if (*_cap > *_sz + _rplclen - _searclen)
		return jstr_rplcat_mem_f(*_s, _sz, _at, _rplc, _rplclen, _searclen);
	JSTR_GROW(*_cap, *_sz + _rplclen - _searclen);
	char *const tmp = (char *)malloc(*_cap);
	JSTR_MALLOC_ERR(tmp, return NULL);
	memcpy(tmp, *_s, _at);
	memcpy(tmp + _at, _rplc, _rplclen);
	memcpy(tmp + _at + _rplclen,
	       *_s + _at + _searclen,
	       *_sz - (_at + _searclen) + 1);
	free(*_s);
	*_sz += _rplclen - _searclen;
	*_s = tmp;
	return tmp + _at + _rplclen;
}

#endif /* JSTR_HAVE_REALLOC_MREMAP */

/*
  Slip SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static char *
jstr_slip_mem(char **JSTR_RST const _s,
	      size_t *JSTR_RST const _sz,
	      size_t *JSTR_RST const _cap,
	      const size_t _at,
	      const char *JSTR_RST const _rplc,
	      const size_t _rplclen) JSTR_NOEXCEPT
{
#if JSTR_HAVE_REALLOC_MREMAP
	if (JSTR_IS_MMAP(*_cap))
		return priv_jstr_slip_mem_realloc(_s, _sz, _cap, _at, _rplc, _rplclen);
#endif /* JSTR_HAVE_REALLOC_MREMAP */
	return priv_jstr_slip_mem_malloc(_s, _sz, _cap, _at, _rplc, _rplclen);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static char *
priv_jstr_rplcat_mem_may_lower(char **JSTR_RST const _s,
			       size_t *JSTR_RST const _sz,
			       size_t *JSTR_RST const _cap,
			       const size_t _at,
			       const char *JSTR_RST const _rplc,
			       const size_t _rplclen,
			       const size_t _searclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_rplclen == _searclen)) {
		memcpy(*_s + _at, _rplc, _rplclen);
		goto RET;
	} else if (*_cap > *_sz + _rplclen - _searclen) {
		return jstr_rplcat_mem_f(*_s, _sz, _at, _rplc, _rplclen, _searclen);

	} else {
#if JSTR_HAVE_REALLOC_MREMAP
		if (JSTR_IS_MMAP(*_cap)) {
			JSTR_REALLOC(*_s, *_cap, *_sz + _rplclen - _searclen, return NULL);
			return jstr_rplcat_mem_f(*_s, _sz, _at, _rplc, _rplclen, _searclen);
		}
#else
		JSTR_GROW(*_cap, *_sz + _rplclen - _searclen);
		char *const tmp = (char *)malloc(*_cap);
		JSTR_MALLOC_ERR(tmp, return NULL);
		memcpy(tmp, *_s, _at);
		memcpy(tmp + _at, _rplc, _rplclen);
		memcpy(tmp + _at + _rplclen,
		       *_s + _at + _searclen,
		       *_sz - _at + _searclen + 1);
		free(*_s);
		*_s = tmp;
#endif /* JSTR_HAVE_REALLOC_MREMAP */
	}
	*_sz += _rplclen - _searclen;
RET:
	return *_s + _at + _rplclen;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static char *
jstr_rplcat_mem(char **JSTR_RST const _s,
		size_t *JSTR_RST const _sz,
		size_t *JSTR_RST const _cap,
		const size_t _at,
		const char *JSTR_RST const _rplc,
		const size_t _rplclen,
		const size_t _searclen) JSTR_NOEXCEPT
{
#if JSTR_HAVE_REALLOC_MREMAP
	if (JSTR_IS_MMAP(*_cap))
		return priv_jstr_rplcat_mem_realloc(_s, _sz, _cap, _at, _rplc, _rplclen, _searclen);
#endif /* JSTR_HAVE_REALLOC_MREMAP */
	return priv_jstr_rplcat_mem_malloc(_s, _sz, _cap, _at, _rplc, _rplclen, _searclen);
}

/*
  Slip SRC after C in DST.
  Return value:
  pointer to '\0' in S.
  Assumes that S have enough space for SRC.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
jstr_slipaftc_mem_p_f(char *JSTR_RST const _s,
		      const int _c,
		      const char *JSTR_RST const _src,
		      const size_t _sz,
		      const size_t _srclen) JSTR_NOEXCEPT
{
	const char *const _p = (char *)memchr(_s, _c, _sz);
	if (_p)
		return jstr_slip_mem_p_f(_s, _p - _s, _src, _sz - (_p - _s), _srclen);
	return _s + _sz;
}

/*
  Slip SRC after C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_slipaftc_mem(char **JSTR_RST const _s,
		  size_t *JSTR_RST const _sz,
		  size_t *JSTR_RST const _cap,
		  const int _c,
		  const char *JSTR_RST const _src,
		  const size_t _srclen) JSTR_NOEXCEPT
{
	const char *const _p = (char *)memchr(*_s, _c, *_sz);
	if (_p)
		jstr_slip_mem(_s, _sz, _cap, _p - *_s + 1, _src, _srclen);
}

/*
  Slip SRC after all C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
static char *
jstr_slipaftallc_mem_p_f(char *JSTR_RST const _s,
			 const int _c,
			 const char *JSTR_RST const _src,
			 size_t _sz,
			 const size_t _srclen) JSTR_NOEXCEPT
{
	size_t off = 0;
	const char *_p;
	while ((_p = (char *)memchr(_s + off, _c, _sz - off - *_s))) {
		off = _p - _s;
		_sz = jstr_slip_mem_p_f(_s, off, _src, _sz, _srclen) - _s;
		off += _srclen + 1;
	}
	return _s + _sz;
}

/*
  Slip SRC after all C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_slipaftallc_mem(char **JSTR_RST const _s,
		     size_t *JSTR_RST const _sz,
		     size_t *JSTR_RST const _cap,
		     const int _c,
		     const char *JSTR_RST const _src,
		     const size_t _srclen) JSTR_NOEXCEPT
{
	size_t off = 0;
	const char *_p;
#if JSTR_HAVE_REALLOC_MREMAP
	const int is_mmap = JSTR_IS_MMAP(*_cap);
#endif /* JSTR_HAVE_REALLOC_MREMAP */
	while ((_p = (char *)memchr(*_s + off, _c, *_sz - off))) {
		off = _p - *_s;
#if JSTR_HAVE_REALLOC_MREMAP
		if (jstr_unlikely(is_mmap))
			priv_jstr_slip_mem_realloc(_s, _sz, _cap, off, _src, _srclen);
		else
#endif /* JSTR_HAVE_REALLOC_MREMAP */
			priv_jstr_slip_mem_malloc(_s, _sz, _cap, off, _src, _srclen);
		off += _srclen + 1;
	}
}

/*
  Slip SRC after end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
jstr_slipaft_mem_f(char *JSTR_RST const _s,
		   const char *JSTR_RST const _searc,
		   const char *JSTR_RST const _src,
		   const size_t _sz,
		   const size_t _searclen,
		   const size_t _srclen) JSTR_NOEXCEPT
{
	switch (_searclen) {
	case 0: return _s + _sz;
	case 1: return jstr_slipaftc_mem_p_f(_s, *_searc, _src, _sz, _srclen);
	default: {
		const char *const _p = (char *)PRIVATE_JSTR_MEMMEM(_s, _sz, _searc, _searclen);
		if (_p)
			return jstr_slip_mem_p_f(_s, _p - _s, _src, _sz, _srclen);
		return _s + _sz;
	}
	}
}

/*
  Slip SRC after end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_slipaft_mem(char **JSTR_RST const _s,
		 size_t *JSTR_RST const _sz,
		 size_t *JSTR_RST const _cap,
		 const char *JSTR_RST const _searc,
		 const char *JSTR_RST const _src,
		 const size_t _searclen,
		 const size_t _srclen) JSTR_NOEXCEPT
{
	switch (_searclen) {
	case 0: return;
	case 1:
		jstr_slipaftc_mem(_s, _sz, _cap, *_searc, _src, _srclen);
		return;
	default: {
		const char *const _p = (char *)PRIVATE_JSTR_MEMMEM(*_s, *_sz, _searc, _searclen);
		if (_p)
			jstr_slip_mem(_s, _sz, _cap, _p - *_s + _searclen, _src, _srclen);
		return;
	}
	}
}

/*
  Slip SRC after all end of NE in DST.
*/
#if JSTR_HAVE_ALLOCA
JSTR_NOINLINE
#else
JSTR_INLINE
#endif /* HAVE_ALLOCA */
JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
static char *
jstr_slipaftall_mem_p_f(char *JSTR_RST const _s,
			const char *JSTR_RST const _searc,
			const char *JSTR_RST const _src,
			size_t _sz,
			const size_t _searclen,
			const size_t _srclen) JSTR_NOEXCEPT
{
	switch (_searclen) {
	case 0: return _s + _sz;
	case 1: return jstr_slipaftallc_mem_p_f(_s, *_searc, _src, _sz, _srclen);
	default: {
		size_t off = 0;
		const char *_p;
		jstr_memmem_table t;
		jstr_memmem_init(&t);
		jstr_memmem_comp_mem(&t, _searc, _searclen);
		while ((_p = (char *)jstr_memmem_exec(&t, _s + off, _sz - off))) {
			off = _p - _s;
			_sz = jstr_slip_mem_p_f(_s, off, _src, _sz, _srclen) - _s;
			off += _searclen + _srclen;
		}
		return _s + _sz;
	}
	}
}

/*
  Slip SRC after all end of NE in DST.
*/
#if JSTR_HAVE_ALLOCA
JSTR_NOINLINE
#else
JSTR_INLINE
#endif /* HAVE_ALLOCA */
JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
static void
jstr_slipaftall_mem(char **JSTR_RST const _s,
		    size_t *JSTR_RST const _sz,
		    size_t *JSTR_RST const _cap,
		    const char *JSTR_RST const _searc,
		    const char *JSTR_RST const _src,
		    const size_t _searclen,
		    const size_t _srclen) JSTR_NOEXCEPT
{
	switch (_searclen) {
	case 0:
		return;
	case 1:
		jstr_slipaftallc_mem(_s, _sz, _cap, *_searc, _src, _srclen);
		return;
	default: {
		size_t off = 0;
		const char *_p;
#if JSTR_HAVE_REALLOC_MREMAP
		const int is_mmap = JSTR_IS_MMAP(*_cap);
#endif /* JSTR_HAVE_REALLOC_MREMAP */
		jstr_memmem_table t;
		jstr_memmem_init(&t);
		jstr_memmem_comp_mem(&t, _searc, _searclen);
		while ((_p = (char *)jstr_memmem_exec(&t, *_s + off, *_sz - off))) {
			off = _p - *_s;
#if JSTR_HAVE_REALLOC_MREMAP
			if (jstr_unlikely(is_mmap))
				priv_jstr_slip_mem_realloc(_s, _sz, _cap, _p - *_s + _searclen, _src, _srclen);
			else
#endif /* JSTR_HAVE_REALLOC_MREMAP */
				priv_jstr_slip_mem_malloc(_s, _sz, _cap, _p - *_s + _searclen, _src, _srclen);
			off += _searclen + _srclen;
		}
	}
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
static char *
jstr_rmc_mem_p(char *JSTR_RST _s,
	       const int _c,
	       const size_t _sz) JSTR_NOEXCEPT
{
	const char *const sstart = _s;
	_s = (char *)memchr(_s, _c, _sz);
	if (jstr_unlikely(_s == NULL))
		return _s + _sz;
	memmove(_s, _s + 1, _sz - (_s - sstart) + 1);
	return _s + _sz - (_s - sstart);
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
static char *
jstr_rmc_p(char *JSTR_RST _s,
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
	return jstr_rmc_mem_p(_s, _c, strlen(_s));
#endif /* HAS_STRCHRNUL */
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
priv_jstr_rmallc_mem_p(const priv_jstr_flag_ty flag,
		       char *JSTR_RST const _s,
		       const int _c,
		       size_t _n,
		       const size_t _sz) JSTR_NOEXCEPT
{
	unsigned char *_dst = (unsigned char *)_s;
	const unsigned char *_old = _dst;
	const unsigned char *_p = _dst;
	const unsigned char *const end = _dst + _sz;
	while ((flag & PRIVATE_JSTR_FLAG_USE_N ? jstr_likely(_n--) : 1)
	       && (_p = (unsigned char *)memchr(_p, _c, end - _p)))
		priv_jstr_rmall_in_place(&_dst, &_old, &_p, 1);
	memmove(_dst, _old, end - _old + 1);
	return (char *)_dst + (end - _old);
}

/*
  Remove all C in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
jstr_rmallc_mem_p(char *JSTR_RST const _s,
		  const int _c,
		  const size_t _sz) JSTR_NOEXCEPT
{
	return priv_jstr_rmallc_mem_p(PRIVATE_JSTR_FLAG_USE_NOT_N, _s, _c, 0, _sz);
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
static char *
jstr_rmallc_p(char *JSTR_RST const _s,
	      const int _c) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	unsigned char *_dst = (unsigned char *)_s;
	const unsigned char *_old = _dst;
	const unsigned char *_p = _dst;
	while (*(_p = (unsigned char *)strchrnul((char *)_p, _c)))
		priv_jstr_rmall_in_place(&_dst, &_old, &_p, 1);
	memmove(_dst, _old, _p - _old + 1);
	return (char *)_dst + (_p - _old);
#else
	return jstr_rmallc_mem_p(_s, _c, strlen(_s));
#endif /* HAS_STRCHRNUL */
}

/*
  Remove N C in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
jstr_rmnc_mem_p(char *JSTR_RST const _s,
		const int _c,
		const size_t _n,
		const size_t _sz) JSTR_NOEXCEPT
{
	return priv_jstr_rmallc_mem_p(PRIVATE_JSTR_FLAG_USE_N, _s, _c, _n, _sz);
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
		priv_jstr_rmall_in_place(&_dst, &_old, &_p, 1);
	memmove(_dst, _old, _p - _old + 1);
	return (char *)_dst + (_p - _old);
#else
	return jstr_rmnc_mem_p(_s, _c, _n, strlen(_s));
#endif /* HAS_STRCHRNUL */
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
static char *
jstr_stripspn_p(char *JSTR_RST const _s,
		const char *JSTR_RST const _rjct) JSTR_NOEXCEPT
{
	unsigned char *_dst = (unsigned char *)_s;
	const unsigned char *_old = _dst;
	const unsigned char *_p = _dst;
	while (*(_p += strcspn((char *)_p, _rjct)))
		priv_jstr_rmall_in_place(&_dst, &_old, &_p, 1);
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
static char *
jstr_rm_mem_p(char *JSTR_RST const _s,
	      const char *JSTR_RST const _searc,
	      const size_t _sz,
	      const size_t _searclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_searclen == 0))
		return _s + _sz;
	char *const _p = (char *)PRIVATE_JSTR_MEMMEM(_s, _sz, _searc, _searclen);
	if (jstr_unlikely(_p == NULL))
		return _s + _sz;
	memmove(_p, _p + _searclen, (_s + _sz) - _p);
	return _s + _sz - _searclen;
}

/*
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_rplcc_mem(char *JSTR_RST _s,
	       const int _searc,
	       const int _rplc,
	       const size_t _sz) JSTR_NOEXCEPT
{
	_s = (char *)memchr(_s, _searc, _sz);
	if (jstr_unlikely(_s == NULL))
		return;
	*_s = _rplc;
}

/*
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_rplcc(char *JSTR_RST _s,
	   const int _searc,
	   const int _rplc) JSTR_NOEXCEPT
{
	_s = strchr(_s, _searc);
	if (jstr_unlikely(_s == NULL))
		return;
	*_s = _rplc;
}

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_rplcallc_mem(char *JSTR_RST _s,
		  const int _searc,
		  const int _rplc,
		  const size_t _sz) JSTR_NOEXCEPT
{
	const char *JSTR_RST const end = _s + _sz;
	while ((_s = (char *)memchr(_s, _searc, end - _s)))
		*_s++ = _rplc;
}

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_rplcallc(char *JSTR_RST _s,
	      const int _searc,
	      const int _rplc) JSTR_NOEXCEPT
{
	while ((_s = (strchr(_s, _searc))))
		*_s++ = _rplc;
}

/*
  Replace N SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_rplcnc_mem(char *JSTR_RST _s,
		const int _searc,
		const int _rplc,
		size_t _n,
		const size_t _sz) JSTR_NOEXCEPT
{
	const char *JSTR_RST const end = _s + _sz;
	while (jstr_likely(--_n) && (_s = (char *)memchr(_s, _searc, end - _s)))
		*_s++ = _rplc;
}

/*
  Replace N SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_rplcnc(char *JSTR_RST _s,
	    const int _searc,
	    const int _rplc,
	    size_t _n) JSTR_NOEXCEPT
{
	while ((_s = strchr(_s, _searc)))
		*_s++ = _rplc;
}

/*
  Replace first SEARCH in S with REPLACE.
  Assumes that SEARCHLEN > REPLACELEN.
*/
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
JSTR_RETURNS_NONNULL
static char *
jstr_rplc_mem_p_f(char *JSTR_RST const _s,
		  const char *JSTR_RST const _searc,
		  const char *JSTR_RST const _rplc,
		  const size_t _sz,
		  const size_t _searclen,
		  const size_t _rplclen) JSTR_NOEXCEPT
{
	switch (_rplclen) {
	case 0:
		return jstr_rm_mem_p(_s, _searc, _sz, _searclen);
	case 1:
		if (jstr_unlikely(_searclen == 1)) {
			jstr_rplcc_mem(_s, *_searc, *_rplc, _sz);
			return _s + _sz;
		}
		/* FALLTHROUGH */
	default: {
		if (jstr_unlikely(_searclen == 0))
			return _s + _sz;
		char *_p = (char *)PRIVATE_JSTR_MEMMEM(_s, _sz, _searc, _searclen);
		if (jstr_unlikely(_p == NULL))
			return _s + _sz;
		memmove(_p + _rplclen,
			_p + _searclen,
			(_s + _sz + 1) - _p + _searclen);
		memcpy(_p, _rplc, _rplclen);
		return _s + _sz - _rplclen - _searclen;
	}
	}
}

/*
  Replace first SEARCH in S with REPLACE.
*/
JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
static void
jstr_rplc_mem(char **JSTR_RST const _s,
	      size_t *JSTR_RST const _sz,
	      size_t *JSTR_RST const _cap,
	      const char *JSTR_RST const _searc,
	      const char *JSTR_RST const _rplc,
	      const size_t _searclen,
	      const size_t _rplclen) JSTR_NOEXCEPT
{
	switch (_rplclen) {
	case 0:
		*_sz = jstr_rm_mem_p(*_s, _searc, *_sz, _searclen) - *_s;
		return;
	case 1:
		if (jstr_unlikely(_searclen == 1)) {
			jstr_rplcc_mem(*_s, *_searc, *_rplc, *_sz);
			return;
		}
		/* FALLTHROUGH */
	default: {
		if (jstr_unlikely(_searclen == 0))
			return;
		char *_p = (char *)PRIVATE_JSTR_MEMMEM(*_s, *_sz, _searc, _searclen);
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
static void
jstr_rplclast_mem(char **JSTR_RST const _s,
		  size_t *JSTR_RST const _sz,
		  size_t *JSTR_RST const _cap,
		  const char *JSTR_RST const _searc,
		  const char *JSTR_RST const _rplc,
		  const size_t _searclen,
		  const size_t _rplclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_searclen == 0))
		return;
	char *_p = (char *)jstr_memrmem(*_s, *_sz, _searc, _searclen);
	if (jstr_unlikely(_p == NULL))
		return;
	priv_jstr_rplcat_mem_may_lower(_s, _sz, _cap, _p - *_s, _rplc, _rplclen, _searclen);
}

#if JSTR_HAVE_ALLOCA
JSTR_NOINLINE
#else
JSTR_INLINE
#endif /* HAVE_ALLOCA */
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
JSTR_MAYBE_UNUSED
static char *
priv_jstr_rmall_mem_p(const priv_jstr_flag_ty flag,
		      char *JSTR_RST const _s,
		      const char *JSTR_RST const _searc,
		      size_t _n,
		      size_t _sz,
		      const size_t _searclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_searclen == 1))
		return priv_jstr_rmallc_mem_p(flag, _s, *_searc, _n, _sz);
	if (jstr_unlikely(_searclen == 0))
		return _s + _sz;
	if (flag & PRIVATE_JSTR_FLAG_USE_N)
		if (jstr_unlikely(_n == 0))
			return _s + _sz;
	unsigned char *_dst = (unsigned char *)_s;
	const unsigned char *_old = _dst;
	const unsigned char *_p = _dst;
	const unsigned char *const end = _dst + _sz;
	jstr_memmem_table t;
	jstr_memmem_init(&t);
	jstr_memmem_comp_mem(&t, _searc, _searclen);
	while ((_p = (unsigned char *)jstr_memmem_exec(&t, (char *)_p, end - _p))) {
		priv_jstr_rmall_in_place(&_dst, &_old, &_p, _searclen);
		if (flag & PRIVATE_JSTR_FLAG_USE_N)
			if (jstr_unlikely(!--_n))
				break;
	}
	memmove(_dst, _old, end - _old + 1);
	return (char *)_dst + (end - _old);
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
static char *
jstr_rmn_mem_p(char *JSTR_RST const _s,
	       const char *JSTR_RST const _searc,
	       size_t _n,
	       size_t _sz,
	       const size_t _searclen) JSTR_NOEXCEPT
{
	return priv_jstr_rmall_mem_p(PRIVATE_JSTR_FLAG_USE_N, _s, _searc, _n, _sz, _searclen);
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
static char *
jstr_rmall_mem_p(char *JSTR_RST const _s,
		 const char *JSTR_RST const _searc,
		 size_t _sz,
		 const size_t _searclen) JSTR_NOEXCEPT
{
	return priv_jstr_rmall_mem_p(PRIVATE_JSTR_FLAG_USE_NOT_N, _s, _searc, 0, _sz, _searclen);
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
static char *
jstr_rmall_p(char *JSTR_RST const _s,
	     const char *JSTR_RST const _searc,
	     const size_t _searclen) JSTR_NOEXCEPT
{
	return jstr_rmall_mem_p(_s, _searc, strlen(_s), _searclen);
}

/*
  Remove all SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
static void
jstr_rmall_mem_j(jstr_ty *JSTR_RST const j,
		 const char *JSTR_RST const _searc,
		 const size_t _searclen) JSTR_NOEXCEPT
{
	j->size = jstr_rmall_mem_p(j->data, _searc, j->size, _searclen) - j->data;
}

/*
  Remove all SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
static void
jstr_rmall_j(jstr_ty *JSTR_RST const j,
	     const char *JSTR_RST const _searc) JSTR_NOEXCEPT
{
	return jstr_rmall_mem_j(j, _searc, strlen(_searc));
}

#if JSTR_HAVE_ALLOCA
JSTR_NOINLINE
#else
JSTR_INLINE
#endif /* HAVE_ALLOCA */
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
static void
priv_jstr_rplcall_mem(const priv_jstr_flag_ty flag,
		      char **JSTR_RST const _s,
		      size_t *JSTR_RST const _sz,
		      size_t *JSTR_RST const _cap,
		      const char *JSTR_RST const _searc,
		      const char *JSTR_RST const _rplc,
		      size_t _n,
		      const size_t _searclen,
		      const size_t _rplclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_rplclen == 0)) {
		*_sz = priv_jstr_rmall_mem_p(flag, *_s, _searc, _n, *_sz, _searclen) - *_s;
		return;
	}
	if (jstr_unlikely(_searclen == 1 && _rplclen == 1)) {
		jstr_rplcc_mem(*_s, *_searc, *_rplc, *_sz);
		return;
	}
	if (jstr_unlikely(_searclen == 0))
		return;
	if (flag & PRIVATE_JSTR_FLAG_USE_N)
		if (jstr_unlikely(_n == 0))
			return;
	jstr_memmem_table t;
	jstr_memmem_init(&t);
	jstr_memmem_comp_mem(&t, _searc, _searclen);
	typedef unsigned char uc;
	const unsigned char *_p = *(uc **)_s;
	const unsigned char *_old = _p;
	unsigned char *_dst = *(uc **)_s;
#if JSTR_HAVE_REALLOC_MREMAP
	const int is_mmap = JSTR_IS_MMAP(*_cap);
#endif /* JSTR_HAVE_REALLOC_MREMAP */
	while ((_p = (uc *)jstr_memmem_exec(&t, (char *)_p, (*(uc **)_s + *_sz) - _p))) {
		if (_rplclen <= _searclen)
			priv_jstr_rplcall_in_place(&_dst, &_old, &_p, _rplc, _rplclen, _searclen);
#if JSTR_HAVE_REALLOC_MREMAP
		else if (jstr_unlikely(is_mmap))
			_p = (uc *)priv_jstr_rplcat_mem_realloc(_s, _sz, _cap, _p - *(uc **)_s, _rplc, _rplclen, _searclen);
#endif /* JSTR_HAVE_REALLOC_MREMAP */
		else
			_p = (uc *)priv_jstr_rplcat_mem_malloc(_s, _sz, _cap, _p - *(uc **)_s, _rplc, _rplclen, _searclen);
		if (flag & PRIVATE_JSTR_FLAG_USE_N)
			if (jstr_unlikely(!--_n))
				break;
	}
	if (_rplclen < _searclen) {
		memmove(_dst, _old, *(uc **)_s + *_sz - _old + 1);
		*_sz = (char *)_dst + (*(uc **)_s + *_sz - _old) - *_s;
	}
}

/*
  Replace N SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
static void
jstr_rplcn_mem(char **JSTR_RST const _s,
	       size_t *JSTR_RST const _sz,
	       size_t *JSTR_RST const _cap,
	       const char *JSTR_RST const _searc,
	       const char *JSTR_RST const _rplc,
	       size_t _n,
	       const size_t _searclen,
	       const size_t _rplclen) JSTR_NOEXCEPT
{
	priv_jstr_rplcall_mem(PRIVATE_JSTR_FLAG_USE_N, _s, _sz, _cap, _searc, _rplc, _n, _searclen, _rplclen);
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
static void
jstr_rplcall_mem(char **JSTR_RST const _s,
		 size_t *JSTR_RST const _sz,
		 size_t *JSTR_RST const _cap,
		 const char *JSTR_RST const _searc,
		 const char *JSTR_RST const _rplc,
		 const size_t _searclen,
		 const size_t _rplclen) JSTR_NOEXCEPT
{
	priv_jstr_rplcall_mem(PRIVATE_JSTR_FLAG_USE_NOT_N, _s, _sz, _cap, _searc, _rplc, 0, _searclen, _rplclen);
}

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#endif /* JSTR_H_REPLACE_DEF */
