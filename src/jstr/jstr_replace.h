#ifndef JSTR_H_REPLACE_DEF
#define JSTR_H_REPLACE_DEF 1

#include "jstr_macros.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr_builder.h"
#include "jstr_ctype.h"
#include "jstr_macros.h"
#include "jstr_string.h"

PJSTR_BEGIN_DECLS

#define PJSTR_RMALL_IN_PLACE(dst, old, p, findlen)  \
	do {                                        \
		if (jstr_likely(dst != old))        \
			memmove(dst, old, p - old); \
		dst += p - old;                     \
		old += p - old;                     \
		old += findlen;                     \
		p += findlen;                       \
	} while (0)
#define PJSTR_RPLCALL_IN_PLACE(dst, old, p, rplc, rplclen, findlen) \
	do {                                                        \
		if (jstr_likely(findlen != rplclen && dst != old))  \
			memmove(dst, old, p - old);                 \
		dst += p - old;                                     \
		old += p - old;                                     \
		old += findlen;                                     \
		p += findlen;                                       \
		memcpy(dst, rplc, rplclen);                         \
		dst += rplclen;                                     \
	} while (0)
/*
  Slip SRC into DST[AT].
  Return value:
  ptr to '\0' in S.
  Assumes that S have enough space for SRC.
*/
JSTR_INLINE
JSTR_FUNC_VOID
static void
pjstr_slip_len_p_f(char *JSTR_RST const _s,
		   const size_t _at,
		   const char *JSTR_RST const _src,
		   const size_t _sz,
		   const size_t _srclen) JSTR_NOEXCEPT
{
	memmove(_s + _at + _srclen,
		_s + _at,
		_sz - _at + 1);
	memcpy(_s + _at, _src, _srclen);
}

/*
  Slip SRC into DST[AT].
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_INLINE
JSTR_FUNC
static int
pjstr_slip_len_realloc(char **JSTR_RST const _s,
		       size_t *JSTR_RST const _sz,
		       size_t *JSTR_RST const _cap,
		       const size_t _at,
		       const char *JSTR_RST const _rplc,
		       const size_t _rplclen) JSTR_NOEXCEPT
{
	if (*_cap < *_sz + _rplclen)
		PJSTR_REALLOC(*_s, *_cap, *_sz + _rplclen, return 0);
	pjstr_slip_len_p_f(*_s, _at, _rplc, *_sz, _rplclen);
	*_sz += _rplclen;
	return 1;
}

/*
  Slip SRC into DST[AT].
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_INLINE
JSTR_FUNC
static int
pjstr_slip_len_malloc(char **JSTR_RST const _s,
		      size_t *JSTR_RST const _sz,
		      size_t *JSTR_RST const _cap,
		      const size_t _at,
		      const char *JSTR_RST const _rplc,
		      const size_t _rplclen) JSTR_NOEXCEPT
{
	if (*_cap > *_sz + _rplclen) {
		pjstr_slip_len_p_f(*_s, _at, _rplc, *_sz, _rplclen);
	} else {
		PJSTR_GROW(*_cap, *_sz + _rplclen);
		char *const _tmp = (char *)malloc(*_cap);
		PJSTR_MALLOC_ERR(_tmp, return 0);
		memcpy(_tmp, *_s, _at);
		memcpy(_tmp + _at, _rplc, _rplclen);
		memcpy(_tmp + _at + _rplclen,
		       *_s + _at,
		       *_sz - _at + 1);
		free(*_s);
		*_s = _tmp;
	}
	*_sz += _rplclen;
	return 1;
}

JSTR_INLINE
JSTR_FUNC_RET_NONNULL
static char *
pjstr_rplcat_len_f(char *JSTR_RST const _s,
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

JSTR_INLINE
JSTR_FUNC
static char *
pjstr_rplcat_len_realloc(char **JSTR_RST const _s,
			 size_t *JSTR_RST const _sz,
			 size_t *JSTR_RST const _cap,
			 const size_t _at,
			 const char *JSTR_RST const _rplc,
			 const size_t _rplclen,
			 const size_t _findlen) JSTR_NOEXCEPT
{
	if (*_cap < *_sz + _rplclen - _findlen)
		PJSTR_REALLOC(*_s, *_cap, *_sz + _rplclen, return NULL);
	return pjstr_rplcat_len_f(*_s, _sz, _at, _rplc, _rplclen, _findlen);
}

JSTR_INLINE
JSTR_FUNC
static char *
pjstr_rplcat_len_malloc(char **JSTR_RST const _s,
			size_t *JSTR_RST const _sz,
			size_t *JSTR_RST const _cap,
			const size_t _at,
			const char *JSTR_RST const _rplc,
			const size_t _rplclen,
			const size_t _findlen) JSTR_NOEXCEPT
{
	if (*_cap > *_sz + _rplclen - _findlen)
		return pjstr_rplcat_len_f(*_s, _sz, _at, _rplc, _rplclen, _findlen);
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

/*
  Slip SRC into DST[AT].
*/
JSTR_FUNC
static int
jstr_slip_len(char **JSTR_RST const _s,
	      size_t *JSTR_RST const _sz,
	      size_t *JSTR_RST const _cap,
	      const size_t _at,
	      const char *JSTR_RST const _rplc,
	      const size_t _rplclen) JSTR_NOEXCEPT
{
#if JSTR_HAVE_REALLOC_MREMAP
	if (PJSTR_IS_MMAP(*_cap))
		return pjstr_slip_len_realloc(_s, _sz, _cap, _at, _rplc, _rplclen);
#endif /* HAVE_REALLOC_MREMAP */
	return pjstr_slip_len_malloc(_s, _sz, _cap, _at, _rplc, _rplclen);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_slip(char **JSTR_RST const _s,
	      size_t *JSTR_RST const _sz,
	      size_t *JSTR_RST const _cap,
	      const size_t _at,
	      const char *JSTR_RST const _rplc(_s, _sz, _cap, _at, _rplc, strlen(_rplc));
}

JSTR_FUNC
static char *
pjstr_rplcat_len_may_lower(char **JSTR_RST const _s,
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
		return pjstr_rplcat_len_f(*_s, _sz, _at, _rplc, _rplclen, _findlen);
	} else {
#if JSTR_HAVE_REALLOC_MREMAP
		if (PJSTR_IS_MMAP(*_cap)) {
			PJSTR_REALLOC(*_s, *_cap, *_sz + _rplclen - _findlen, return NULL);
			return pjstr_rplcat_len_f(*_s, _sz, _at, _rplc, _rplclen, _findlen);
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
JSTR_FUNC
static char *
pjstr_rplcat_len(char **JSTR_RST const _s,
		 size_t *JSTR_RST const _sz,
		 size_t *JSTR_RST const _cap,
		 const size_t _at,
		 const char *JSTR_RST const _rplc,
		 const size_t _rplclen,
		 const size_t _findlen) JSTR_NOEXCEPT
{
#if JSTR_HAVE_REALLOC_MREMAP
	if (PJSTR_IS_MMAP(*_cap))
		return pjstr_rplcat_len_realloc(_s, _sz, _cap, _at, _rplc, _rplclen, _findlen);
#endif /* HAVE_REALLOC_MREMAP */
	return pjstr_rplcat_len_malloc(_s, _sz, _cap, _at, _rplc, _rplclen, _findlen);
}

/*
  Slip SRC after C in DST.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_INLINE
JSTR_FUNC
static int
jstr_slipaftchr_len(char **JSTR_RST const _s,
		    size_t *JSTR_RST const _sz,
		    size_t *JSTR_RST const _cap,
		    const int _c,
		    const char *JSTR_RST const _src,
		    const size_t _srclen) JSTR_NOEXCEPT
{
	const char *const _p = (char *)memchr(*_s, _c, *_sz);
	if (_p != NULL)
		return jstr_slip_len(_s, _sz, _cap, _p - *_s + 1, _src, _srclen);
	return 1;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_slipaftchr(char **JSTR_RST const _s,
		    size_t *JSTR_RST const _sz,
		    size_t *JSTR_RST const _cap,
		    const int _c,
		    const char *JSTR_RST const _src(_s, _sz, _cap, _c, _src, strlen(_src));
}

/*
  Slip SRC after all C in DST.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_slipaftallchr_len(char **JSTR_RST const _s,
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
		if (jstr_unlikely(is_mmap)) {
			if (jstr_unlikely(!pjstr_slip_len_realloc(_s, _sz, _cap, off, _src, _srclen)))
				return 0;
		} else
#endif /* HAVE_REALLOC_MREMAP */
			if (jstr_unlikely(!pjstr_slip_len_realloc(_s, _sz, _cap, off, _src, _srclen)))
				return 0;
		off += _srclen + 1;
	}
	return 1;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_slipaftallchr(char **JSTR_RST const _s,
		       size_t *JSTR_RST const _sz,
		       size_t *JSTR_RST const _cap,
		       const int _c,
		       const char *JSTR_RST const _src(_s, _sz, _cap, _c, _src, strlen(_src));
}

/*
  Slip SRC after end of NE in DST.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_slipaft_len(char **JSTR_RST const _s,
		 size_t *JSTR_RST const _sz,
		 size_t *JSTR_RST const _cap,
		 const char *JSTR_RST const _find,
		 const char *JSTR_RST const _src,
		 const size_t _findlen,
		 const size_t _srclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_findlen == 1))
		return jstr_slipaftchr_len(_s, _sz, _cap, *_find, _src, _srclen);
	if (jstr_unlikely(_findlen == 0))
		return 1;
	const char *const _p = (char *)PJSTR_MEMMEM(*_s, *_sz, _find, _findlen);
	if (_p != NULL)
		return jstr_slip_len(_s, _sz, _cap, _p - *_s + _findlen, _src, _srclen);
	return 1;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_slipaft(char **JSTR_RST const _s,
		 size_t *JSTR_RST const _sz,
		 size_t *JSTR_RST const _cap,
		 const char *JSTR_RST const _find,
		 const char *JSTR_RST const _src(_s, _sz, _cap, _find, _src, strlen(_find), strlen(_src));
}

/*
  Slip SRC after all _end of NE in DST.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_slipaftall_len(char **JSTR_RST const _s,
		    size_t *JSTR_RST const _sz,
		    size_t *JSTR_RST const _cap,
		    const char *JSTR_RST const _find,
		    const char *JSTR_RST const _src,
		    const size_t _findlen,
		    const size_t _srclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_findlen == 1))
		return jstr_slipaftallchr_len(_s, _sz, _cap, *_find, _src, _srclen);
	if (jstr_unlikely(_findlen == 0))
		return 1;
	size_t off = 0;
	const char *_p;
#if JSTR_HAVE_REALLOC_MREMAP
	const int is_mmap = PJSTR_IS_MMAP(*_cap);
#endif /* HAVE_REALLOC_MREMAP */
	while ((_p = (char *)PJSTR_MEMMEM(*_s + off, *_sz - off, _find, _findlen))) {
		off = _p - *_s;
#if JSTR_HAVE_REALLOC_MREMAP
		if (jstr_unlikely(is_mmap)) {
			if (jstr_unlikely(!pjstr_slip_len_realloc(_s, _sz, _cap, _p - *_s + _findlen, _src, _srclen)))
				return 0;
		} else
#endif /* HAVE_REALLOC_MREMAP */
			if (jstr_unlikely(!pjstr_slip_len_malloc(_s, _sz, _cap, _p - *_s + _findlen, _src, _srclen)))
				return 0;
		off += _findlen + _srclen;
	}
	return 1;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_slipaftall(char **JSTR_RST const _s,
		    size_t *JSTR_RST const _sz,
		    size_t *JSTR_RST const _cap,
		    const char *JSTR_RST const _find,
		    const char *JSTR_RST const _src(_s, _sz, _cap, _find, _src, strlen(_find), strlen(_src));
}

/*
  Remove first C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_INLINE
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmchr_len_p(char *JSTR_RST _s,
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
JSTR_FUNC_RET_NONNULL
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
	return jstr_rmchr_len_p(_s, _c, strlen(_s));
#endif /* HAVE_STRCHRNUL */
}

/*
  Remove all REJECT in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmspn_p(char *JSTR_RST _s,
	     const char *JSTR_RST _reject) JSTR_NOEXCEPT
{
	unsigned char *_dst = (unsigned char *)_s;
	const unsigned char *_old = _dst;
	const unsigned char *_p = _dst;
	size_t _findlen;
	while (jstr_likely(*(_p += strcspn((char *)_p, _reject)))) {
		_findlen = strspn((char *)_p, _reject);
		PJSTR_RMALL_IN_PLACE(_dst, _old, _p, _findlen);
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
JSTR_FUNC_VOID
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
JSTR_FUNC_RET_NONNULL
static char *
pjstr_rmallchr_len_p(const pjstr_flag_use_n_ty _flag,
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
		PJSTR_RMALL_IN_PLACE(_dst, _old, _p, 1);
	memmove(_dst, _old, _end - _old + 1);
	return (char *)_dst + (_end - _old);
}

/*
  Remove all C in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmallchr_len_p(char *JSTR_RST const _s,
		    const int _c,
		    const size_t _sz) JSTR_NOEXCEPT
{
	return pjstr_rmallchr_len_p(PJSTR_FLAG_USE_NOT_N, _s, _c, 0, _sz);
}

/*
  Remove all C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmallchr_p(char *JSTR_RST const _s,
		const int _c) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	unsigned char *_dst = (unsigned char *)_s;
	const unsigned char *_old = _dst;
	const unsigned char *_p = _dst;
	while (*(_p = (unsigned char *)strchrnul((char *)_p, _c)))
		PJSTR_RMALL_IN_PLACE(_dst, _old, _p, 1);
	memmove(_dst, _old, _p - _old + 1);
	return (char *)_dst + (_p - _old);
#else
	return jstr_rmallchr_len_p(_s, _c, strlen(_s));
#endif /* HAVE_STRCHRNUL */
}

/*
  Remove N C in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmnc_len_p(char *JSTR_RST const _s,
		const int _c,
		const size_t _n,
		const size_t _sz) JSTR_NOEXCEPT
{
	return pjstr_rmallchr_len_p(PJSTR_FLAG_USE_N, _s, _c, _n, _sz);
}

/*
  Remove N C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_INLINE
JSTR_FUNC_RET_NONNULL
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
		PJSTR_RMALL_IN_PLACE(_dst, _old, _p, 1);
	memmove(_dst, _old, _p - _old + 1);
	return (char *)_dst + (_p - _old);
#else
	return jstr_rmnc_len_p(_s, _c, _n, strlen(_s));
#endif /* HAVE_STRCHRNUL */
}

/*
  Remove characters in REJECT in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_stripspn_p(char *JSTR_RST const _s,
		const char *JSTR_RST const _rjct) JSTR_NOEXCEPT
{
	unsigned char *_dst = (unsigned char *)_s;
	const unsigned char *_old = _dst;
	const unsigned char *_p = _dst;
	while (*(_p += strcspn((char *)_p, _rjct)))
		PJSTR_RMALL_IN_PLACE(_dst, _old, _p, 1);
	memmove(_dst, _old, _p - _old + 1);
	return (char *)_dst + (_p - _old);
}

/*
  Remove first HS in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_FUNC_RET_NONNULL
static char *
jstr_rm_len_p(char *JSTR_RST const _s,
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
JSTR_FUNC_VOID
static void
jstr_rplcchr_len(char *JSTR_RST _s,
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
JSTR_FUNC_VOID
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
JSTR_FUNC_VOID
static void
jstr_rplcallchr_len(char *JSTR_RST _s,
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
JSTR_FUNC_VOID
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
JSTR_FUNC_VOID
static void
jstr_rplcnc_len(char *JSTR_RST _s,
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
JSTR_FUNC_VOID
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
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_rplc_len(char **JSTR_RST const _s,
	      size_t *JSTR_RST const _sz,
	      size_t *JSTR_RST const _cap,
	      const char *JSTR_RST const _find,
	      const char *JSTR_RST const _rplc,
	      const size_t _findlen,
	      const size_t _rplclen) JSTR_NOEXCEPT
{
	switch (_rplclen) {
	case 0:
		*_sz = jstr_rm_len_p(*_s, _find, *_sz, _findlen) - *_s;
		return 1;
	case 1:
		if (jstr_unlikely(_findlen == 1)) {
			jstr_rplcchr_len(*_s, *_find, *_rplc, *_sz);
			return 1;
		}
		/* fallthrough */
	default: {
		if (jstr_unlikely(_findlen == 0))
			return 1;
		char *_p = (char *)PJSTR_MEMMEM(*_s, *_sz, _find, _findlen);
		if (jstr_unlikely(_p == NULL))
			return 1;
		return jstr_slip_len(_s, _sz, _cap, _p - *_s, _rplc, _rplclen);
	}
	}
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_rplc(char **JSTR_RST const _s,
	      size_t *JSTR_RST const _sz,
	      size_t *JSTR_RST const _cap,
	      const char *JSTR_RST const _find,
	      const char *JSTR_RST const _rplc(_s, _sz, _cap, _find, _rplc, strlen(_find), strlen(_rplc));
}

/*
  Replace last SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC_VOID
static int
jstr_rplclast_len(char **JSTR_RST const _s,
		  size_t *JSTR_RST const _sz,
		  size_t *JSTR_RST const _cap,
		  const char *JSTR_RST const _find,
		  const char *JSTR_RST const _rplc,
		  const size_t _findlen,
		  const size_t _rplclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_findlen == 0))
		return 1;
	char *_p = (char *)jstr_strrstr_len(*_s, *_sz, _find, _findlen);
	if (jstr_unlikely(_p == NULL))
		return 1;
	return pjstr_rplcat_len_may_lower(_s, _sz, _cap, _p - *_s, _rplc, _rplclen, _findlen) ? 1 : 0;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_rplclast(char **JSTR_RST const _s,
		  size_t *JSTR_RST const _sz,
		  size_t *JSTR_RST const _cap,
		  const char *JSTR_RST const _find,
		  const char *JSTR_RST const _rplc(_s, _sz, _cap, _find, _rplc, strlen(_find), strlen(_rplc));
}

JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
pjstr_rmall_len_p(const pjstr_flag_use_n_ty _flag,
		  char *JSTR_RST const _s,
		  const char *JSTR_RST const _find,
		  size_t _n,
		  size_t _sz,
		  const size_t _findlen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_findlen == 1))
		return pjstr_rmallchr_len_p(_flag, _s, *_find, _n, _sz);
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
		PJSTR_RMALL_IN_PLACE(_dst, _old, _p, _findlen);
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
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmn_len_p(char *JSTR_RST const _s,
	       const char *JSTR_RST const _find,
	       size_t _n,
	       size_t _sz,
	       const size_t _findlen) JSTR_NOEXCEPT
{
	return pjstr_rmall_len_p(PJSTR_FLAG_USE_N, _s, _find, _n, _sz, _findlen);
}

/*
  Remove all SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmall_len_p(char *JSTR_RST const _s,
		 const char *JSTR_RST const _find,
		 size_t _sz,
		 const size_t _findlen) JSTR_NOEXCEPT
{
	return pjstr_rmall_len_p(PJSTR_FLAG_USE_NOT_N, _s, _find, 0, _sz, _findlen);
}

/*
  Remove all SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_WARN_UNUSED
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmall_p(char *JSTR_RST const _s,
	     const char *JSTR_RST const _find,
	     const size_t _findlen) JSTR_NOEXCEPT
{
	return jstr_rmall_len_p(_s, _find, strlen(_s), _findlen);
}

/*
  Remove all SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_FUNC_VOID
static void
jstr_rmall_len_j(jstr_ty *JSTR_RST const _j,
		 const char *JSTR_RST const _find,
		 const size_t _findlen) JSTR_NOEXCEPT
{
	_j->size = jstr_rmall_len_p(_j->data, _find, _j->size, _findlen) - _j->data;
}

/*
  Remove all SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_FUNC_VOID
static void
jstr_rmall_j(jstr_ty *JSTR_RST const _j,
	     const char *JSTR_RST const _find) JSTR_NOEXCEPT
{
	return jstr_rmall_len_j(_j, _find, strlen(_find));
}

JSTR_INLINE
JSTR_FUNC
static int
pjstr_rplcall_len(const pjstr_flag_use_n_ty _flag,
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
		*_sz = pjstr_rmall_len_p(_flag, *_s, _find, _n, *_sz, _findlen) - *_s;
		return 1;
	}
	if (jstr_unlikely(_findlen == 1 && _rplclen == 1)) {
		jstr_rplcchr_len(*_s, *_find, *_rplc, *_sz);
		return 1;
	}
	if (jstr_unlikely(_findlen == 0))
		return 1;
	if (_flag & PJSTR_FLAG_USE_N)
		if (jstr_unlikely(_n == 0))
			return 1;
	typedef unsigned char uc;
	const unsigned char *_p = *(uc **)_s;
	const unsigned char *_old = _p;
	unsigned char *_dst = *(uc **)_s;
#if JSTR_HAVE_REALLOC_MREMAP
	const int is_mmap = PJSTR_IS_MMAP(*_cap);
#endif /* HAVE_REALLOC_MREMAP */
	while ((_p = (uc *)PJSTR_MEMMEM((char *)_p, (*(uc **)_s + *_sz) - _p, _find, _findlen))) {
		if (_rplclen <= _findlen)
			PJSTR_RPLCALL_IN_PLACE(_dst, _old, _p, _rplc, _rplclen, _findlen);
#if JSTR_HAVE_REALLOC_MREMAP
		else if (jstr_unlikely(is_mmap))
			_p = (uc *)pjstr_rplcat_len_realloc(_s, _sz, _cap, _p - *(uc **)_s, _rplc, _rplclen, _findlen);
#endif /* HAVE_REALLOC_MREMAP */
		else
			_p = (uc *)pjstr_rplcat_len_malloc(_s, _sz, _cap, _p - *(uc **)_s, _rplc, _rplclen, _findlen);
		if (jstr_unlikely(_p == NULL))
			return 0;
		if (_flag & PJSTR_FLAG_USE_N)
			if (jstr_unlikely(!--_n))
				break;
	}
	if (_rplclen < _findlen) {
		memmove(_dst, _old, *(uc **)_s + *_sz - _old + 1);
		*_sz = (char *)_dst + (*(uc **)_s + *_sz - _old) - *_s;
	}
	return 1;
}

/*
  Replace N SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_rplcn_len(char **JSTR_RST const _s,
	       size_t *JSTR_RST const _sz,
	       size_t *JSTR_RST const _cap,
	       const char *JSTR_RST const _find,
	       const char *JSTR_RST const _rplc,
	       size_t _n,
	       const size_t _findlen,
	       const size_t _rplclen) JSTR_NOEXCEPT
{
	return pjstr_rplcall_len(PJSTR_FLAG_USE_N, _s, _sz, _cap, _find, _rplc, _n, _findlen, _rplclen);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_rplcn(char **JSTR_RST const _s,
	       size_t *JSTR_RST const _sz,
	       size_t *JSTR_RST const _cap,
	       const char *JSTR_RST const _find,
	       const char *JSTR_RST const _rplc,
	       size_t _n(_s, _sz, _cap, _find, _rplc, _n, strlen(_find), strlen(_rplc));
}

/*
  Replace all SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_rplcall_len(char **JSTR_RST const _s,
		 size_t *JSTR_RST const _sz,
		 size_t *JSTR_RST const _cap,
		 const char *JSTR_RST const _find,
		 const char *JSTR_RST const _rplc,
		 const size_t _findlen,
		 const size_t _rplclen) JSTR_NOEXCEPT
{
	return pjstr_rplcall_len(PJSTR_FLAG_USE_NOT_N, _s, _sz, _cap, _find, _rplc, 0, _findlen, _rplclen);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_rplcall(char **JSTR_RST const _s,
		 size_t *JSTR_RST const _sz,
		 size_t *JSTR_RST const _cap,
		 const char *JSTR_RST const _find,
		 const char *JSTR_RST const _rplc(_s, _sz, _cap, _find, _rplc, strlen(_find), strlen(_rplc));
}

/*
  Reverse S.
*/
JSTR_FUNC_VOID
static void
jstr_rev_len(char *JSTR_RST _s,
	     size_t _sz) JSTR_NOEXCEPT
{
#if JSTR_HAVE_ATTR_MAY_ALIAS
	pjstr_op_ty *_sw = (pjstr_op_ty *)_s;
	char _c0;
	size_t _i = 0;
	switch (--_sz % 4) {
	case 7:
		_c0 = _s[_i];
		_s[_i] = _s[_sz];
		_s[_sz] = _c0;
		--_i, --_sz;
		/* fallthrough */
	case 6:
		_c0 = _s[_i];
		_s[_i] = _s[_sz];
		_s[_sz] = _c0;
		--_i, --_sz;
		/* fallthrough */
	case 5:
		_c0 = _s[_i];
		_s[_i] = _s[_sz];
		_s[_sz] = _c0;
		--_i, --_sz;
		/* fallthrough */
	case 4:
		_c0 = _s[_i];
		_s[_i] = _s[_sz];
		_s[_sz] = _c0;
		--_i, --_sz;
		/* fallthrough */
	case 3:
		_c0 = _s[_i];
		_s[_i] = _s[_sz];
		_s[_sz] = _c0;
		--_i, --_sz;
		/* fallthrough */
	case 2:
		_c0 = _s[_i];
		_s[_i] = _s[_sz];
		_s[_sz] = _c0;
		--_i, --_sz;
		/* fallthrough */
	case 1:
		_c0 = _s[_i];
		_s[_i] = _s[_sz];
		_s[_sz] = _c0;
		--_i, --_sz;
		/* fallthrough */
	case 0: break;
	}
	pjstr_op_ty *_ew = (pjstr_op_ty *)(_s + _sz);
	pjstr_op_ty _c;
	while (_sw < _ew) {
		_c = *_sw;
		*_sw++ = *_ew;
		*_ew-- = _c;
	}
#else
#	if 0
	char _c0, _c1, _c2, _c3;
	size_t _i = 0;
	switch (--_sz % 4) {
	case 3:
		_c0 = _s[_i];
		_s[_i] = _s[_sz];
		_s[_sz] = _c0;
		--_i, --_sz;
		/* fallthrough */
	case 2:
		_c0 = _s[_i];
		_s[_i] = _s[_sz];
		_s[_sz] = _c0;
		--_i, --_sz;
		/* fallthrough */
	case 1:
		_c0 = _s[_i];
		_s[_i] = _s[_sz];
		_s[_sz] = _c0;
		--_i, --_sz;
		/* fallthrough */
	case 0: break;
	}
	for (; jstr_likely(_i + 4 <= _sz); _i += 4, _sz -= 4) {
		_c0 = _s[_i];
		_c1 = _s[_i + 1];
		_c2 = _s[_i + 2];
		_c3 = _s[_i + 3];
		_s[_i] = _s[_sz];
		_s[_i + 1] = _s[_sz - 1];
		_s[_i + 2] = _s[_sz - 2];
		_s[_i + 2] = _s[_sz - 2];
		_s[_i + 3] = _s[_sz - 3];
		_s[_sz] = _c0;
		_s[_sz - 1] = _c1;
		_s[_sz - 2] = _c2;
		_s[_sz - 3] = _c3;
	}
#	else /* It seems that unrolling is slower */
	char _c0;
	for (size_t _i = 0; jstr_likely(_i < _sz); _i += 4, _sz -= 4) {
		_c0 = _s[_i];
		_s[_i] = _s[_sz];
		_s[_sz] = _c0;
	}
#	endif
#endif
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_rev(char *JSTR_RST _s) JSTR_NOEXCEPT
{
	jstr_rev_len(_s, strlen(_s));
}

/*
  Reverse S.
  Return value:
  ptr to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rev_p(char *JSTR_RST _s) JSTR_NOEXCEPT
{
	const size_t _len = strlen(_s);
	jstr_rev_len(_s, _len);
	return _s + _len;
}

/*
  Trim leading and trailing [\n\t\v\r ] in S.
  Return value:
  ptr to '\0' in S;
  S if SLEN is 0.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_trim_len_p(char *JSTR_RST const _s,
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
  ptr to '\0' in S;
  S if SLEN is 0.
*/
JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstr_trim_p(char *JSTR_RST const _s) JSTR_NOEXCEPT
{
	return jstr_trim_len_p(_s, strlen(_s));
}

/*
  Trim leading and trailing [\n\t\v\r ] in S.
  Return value:
  ptr to '\0' in S;
  S if SLEN is 0.
*/
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_trim_j(jstr_ty *JSTR_RST const _j) JSTR_NOEXCEPT
{
	_j->size = jstr_trim_len_p(_j->data, _j->size) - _j->data;
}

/*
  Insert SRC into DST[AT].
  Assumes that S have enough space for SRC.
  Return value:
*/
JSTR_INLINE
JSTR_FUNC_VOID
static void
pjstr_insert_len_f(char *JSTR_RST const _s,
		   const size_t _at,
		   const char *JSTR_RST const _src,
		   const size_t _srclen) JSTR_NOEXCEPT
{
	memcpy(_s + _at, _src, _srclen);
}

/*
  Insert SRC into DST[AT].
  Return 0 on malloc error;
  otherwise 1.
*/
JSTR_INLINE
JSTR_FUNC
static int
jstr_insert_len(char **JSTR_RST const _s,
		size_t *JSTR_RST const _sz,
		size_t *JSTR_RST const _cap,
		const size_t _at,
		const char *JSTR_RST const _src,
		const size_t _srclen) JSTR_NOEXCEPT
{
	if (_at + _srclen > *_sz) {
		PJSTR_REALLOC(*_s, *_cap, _at + _srclen + 1, return 0);
		*_sz = _at + _srclen;
		*(*_s + *_sz) = '\0';
	}
	pjstr_insert_len_f(*_s, _at, _src, _srclen);
	return 1;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_insert(char **JSTR_RST const _s,
		size_t *JSTR_RST const _sz,
		size_t *JSTR_RST const _cap,
		const size_t _at,
		const char *JSTR_RST const _src(_s, _sz, _cap, _at, _src, strlen(_src));
}

/*
  Insert SRC after C in DST.
  Return 0 on malloc error;
  otherwise 1.
*/
JSTR_INLINE
JSTR_FUNC
static int
jstr_insertaftchr_len(char **JSTR_RST const _s,
		      size_t *JSTR_RST const _sz,
		      size_t *JSTR_RST const _cap,
		      const int _c,
		      const char *JSTR_RST const _src,
		      const size_t _srclen) JSTR_NOEXCEPT
{
	const char *const _p = (char *)memchr(*_s, _c, *_sz);
	if (_p != NULL)
		return jstr_insert_len(_s, _sz, _cap, _p - *_s + 1, _src, _srclen);
	return 1;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_insertaftchr(char **JSTR_RST const _s,
		      size_t *JSTR_RST const _sz,
		      size_t *JSTR_RST const _cap,
		      const int _c,
		      const char *JSTR_RST const _src(_s, _sz, _cap, _c, _src, strlen(_src));
}

/*
  Insert SRC after end of NE in DST.
  Return 0 on malloc error;
  otherwise 1.
*/
JSTR_FUNC
static int
jstr_insertaft_len(char **JSTR_RST const _s,
		   size_t *JSTR_RST const _sz,
		   size_t *JSTR_RST const _cap,
		   const char *JSTR_RST const _find,
		   const char *JSTR_RST const _src,
		   const size_t _findlen,
		   const size_t _srclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_findlen == 1))
		return jstr_insertaftchr_len(_s, _sz, _cap, *_find, _src, _srclen);
	if (jstr_unlikely(_findlen == 0))
		return 1;
	const char *const _p = (char *)PJSTR_MEMMEM(*_s, *_sz, _find, _findlen);
	if (_p != NULL)
		return jstr_insert_len(_s, _sz, _cap, _p - *_s + _findlen, _src, _srclen);
	return 1;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_insertaft(char **JSTR_RST const _s,
		   size_t *JSTR_RST const _sz,
		   size_t *JSTR_RST const _cap,
		   const char *JSTR_RST const _find,
		   const char *JSTR_RST const _src(_s, _sz, _cap, _find, _src, strlen(_find), strlen(_src));
}

PJSTR_END_DECLS

#endif /* JSTR_H_REPLACE_DEF */
