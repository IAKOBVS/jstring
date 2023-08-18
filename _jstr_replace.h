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
priv_jstr_rmall_in_place(unsigned char **const dst,
			 const unsigned char **const old,
			 const unsigned char **const p,
			 const size_t _searclen) JSTR_NOEXCEPT
{
	if (jstr_likely(*dst != *old))
		memmove(*dst, *old, *p - *old);
	*dst += (*p - *old);
	*old += (*p - *old);
	*old += _searclen;
	*p += _searclen;
}

JSTR_INLINE
static void
priv_jstr_rplcall_in_place(unsigned char **const dst,
			   const unsigned char **const old,
			   const unsigned char **const p,
			   const char *JSTR_RST const _rplc,
			   const size_t _rplclen,
			   const size_t _searclen) JSTR_NOEXCEPT
{
	if (jstr_likely(_searclen != _rplclen && *dst != *old))
		memmove(*dst, *old, *p - *old);
	*dst += (*p - *old);
	*old += (*p - *old);
	*old += _searclen;
	*p += _searclen;
	memcpy(*dst, _rplc, _rplclen);
	*dst += _rplclen;
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
jstr_slip_mem_p_f(char *JSTR_RST const s,
		  const size_t at,
		  const char *JSTR_RST const _src,
		  const size_t sz,
		  const size_t _srclen) JSTR_NOEXCEPT
{
	memmove(s + at + _srclen,
		s + at,
		sz - at + 1);
	memcpy(s + at, _src, _srclen);
	return s + sz + _srclen;
}

#if JSTR_HAVE_REALLOC_MREMAP

/*
  Slip SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static char *
priv_jstr_slip_mem_realloc(char **JSTR_RST const s,
			   size_t *JSTR_RST const sz,
			   size_t *JSTR_RST const cap,
			   const size_t at,
			   const char *JSTR_RST const _rplc,
			   const size_t _rplclen) JSTR_NOEXCEPT
{
	if (*cap < *sz + _rplclen)
		JSTR_REALLOC(*s, *cap, *sz + _rplclen, return NULL);
	return jstr_slip_mem_p_f(*s, at, _rplc, *sz, _rplclen);
}

/*
  Slip SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static char *
priv_jstr_slip_mem_malloc(char **JSTR_RST const s,
			  size_t *JSTR_RST const sz,
			  size_t *JSTR_RST const cap,
			  const size_t at,
			  const char *JSTR_RST const _rplc,
			  const size_t _rplclen) JSTR_NOEXCEPT
{
	if (*cap > *sz + _rplclen)
		return jstr_slip_mem_p_f(*s, at, _rplc, *sz, _rplclen);
	JSTR_GROW(*cap, *sz + _rplclen);
	char *const tmp = (char *)malloc(*cap);
	JSTR_MALLOC_ERR(tmp, return NULL);
	memcpy(tmp, *s, at);
	memcpy(tmp + at, _rplc, _rplclen);
	memcpy(tmp + at + _rplclen,
	       *s + at,
	       *sz - at + 1);
	free(*s);
	*s = tmp;
	*sz += _rplclen;
	return *s + at + _rplclen;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static char *
jstr_rplcat_mem_f(char *JSTR_RST const s,
		  size_t *JSTR_RST const sz,
		  const size_t at,
		  const char *JSTR_RST const _rplc,
		  const size_t _rplclen,
		  const size_t _searclen) JSTR_NOEXCEPT
{
	memmove(s + at + _rplclen,
		s + at + _searclen,
		*sz - (at + _searclen) + 1);
	memcpy(s + at, _rplc, _rplclen);
	*sz += _rplclen - _searclen;
	return s + at + _rplclen;
}

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_NONNULL_ALL
static char *
priv_jstr_rplcat_mem_realloc(char **JSTR_RST const s,
			     size_t *JSTR_RST const sz,
			     size_t *JSTR_RST const cap,
			     const size_t at,
			     const char *JSTR_RST const _rplc,
			     const size_t _rplclen,
			     const size_t _searclen) JSTR_NOEXCEPT
{
	if (*cap > *sz + _rplclen - _searclen)
		JSTR_REALLOC(*s, *cap, *sz + _rplclen, return NULL);
	return jstr_rplcat_mem_f(*s, sz, at, _rplc, _rplclen, _searclen);
}

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_NONNULL_ALL
static char *
priv_jstr_rplcat_mem_malloc(char **JSTR_RST const s,
			    size_t *JSTR_RST const sz,
			    size_t *JSTR_RST const cap,
			    const size_t at,
			    const char *JSTR_RST const _rplc,
			    const size_t _rplclen,
			    const size_t _searclen) JSTR_NOEXCEPT
{
	if (*cap > *sz + _rplclen - _searclen)
		return jstr_rplcat_mem_f(*s, sz, at, _rplc, _rplclen, _searclen);
	JSTR_GROW(*cap, *sz + _rplclen - _searclen);
	char *const tmp = (char *)malloc(*cap);
	JSTR_MALLOC_ERR(tmp, return NULL);
	memcpy(tmp, *s, at);
	memcpy(tmp + at, _rplc, _rplclen);
	memcpy(tmp + at + _rplclen,
	       *s + at + _searclen,
	       *sz - (at + _searclen) + 1);
	free(*s);
	*sz += _rplclen - _searclen;
	*s = tmp;
	return tmp + at + _rplclen;
}

#endif /* JSTR_HAVE_REALLOC_MREMAP */

/*
  Slip SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static char *
jstr_slip_mem(char **JSTR_RST const s,
	      size_t *JSTR_RST const sz,
	      size_t *JSTR_RST const cap,
	      const size_t at,
	      const char *JSTR_RST const _rplc,
	      const size_t _rplclen) JSTR_NOEXCEPT
{
#if JSTR_HAVE_REALLOC_MREMAP
	if (JSTR_IS_MMAP(*cap))
		return priv_jstr_slip_mem_realloc(s, sz, cap, at, _rplc, _rplclen);
#endif /* JSTR_HAVE_REALLOC_MREMAP */
	return priv_jstr_slip_mem_malloc(s, sz, cap, at, _rplc, _rplclen);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static char *
priv_jstr_rplcat_mem_may_lower(char **JSTR_RST const s,
			       size_t *JSTR_RST const sz,
			       size_t *JSTR_RST const cap,
			       const size_t at,
			       const char *JSTR_RST const _rplc,
			       const size_t _rplclen,
			       const size_t _searclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_rplclen == _searclen)) {
		memcpy(*s + at, _rplc, _rplclen);
		goto RET;
	} else if (*cap > *sz + _rplclen - _searclen) {
		return jstr_rplcat_mem_f(*s, sz, at, _rplc, _rplclen, _searclen);

	} else {
#if JSTR_HAVE_REALLOC_MREMAP
		if (JSTR_IS_MMAP(*cap)) {
			JSTR_REALLOC(*s, *cap, *sz + _rplclen - _searclen, return NULL);
			return jstr_rplcat_mem_f(*s, sz, at, _rplc, _rplclen, _searclen);
		}
#else
		JSTR_GROW(*cap, *sz + _rplclen - _searclen);
		char *const tmp = (char *)malloc(*cap);
		JSTR_MALLOC_ERR(tmp, return NULL);
		memcpy(tmp, *s, at);
		memcpy(tmp + at, _rplc, _rplclen);
		memcpy(tmp + at + _rplclen,
		       *s + at + _searclen,
		       *sz - at + _searclen + 1);
		free(*s);
		*s = tmp;
#endif /* JSTR_HAVE_REALLOC_MREMAP */
	}
	*sz += _rplclen - _searclen;
RET:
	return *s + at + _rplclen;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static char *
jstr_rplcat_mem(char **JSTR_RST const s,
		size_t *JSTR_RST const sz,
		size_t *JSTR_RST const cap,
		const size_t at,
		const char *JSTR_RST const _rplc,
		const size_t _rplclen,
		const size_t _searclen) JSTR_NOEXCEPT
{
#if JSTR_HAVE_REALLOC_MREMAP
	if (JSTR_IS_MMAP(*cap))
		return priv_jstr_rplcat_mem_realloc(s, sz, cap, at, _rplc, _rplclen, _searclen);
#endif /* JSTR_HAVE_REALLOC_MREMAP */
	return priv_jstr_rplcat_mem_malloc(s, sz, cap, at, _rplc, _rplclen, _searclen);
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
jstr_slipaftc_mem_p_f(char *JSTR_RST const s,
		      const int c,
		      const char *JSTR_RST const _src,
		      const size_t sz,
		      const size_t _srclen) JSTR_NOEXCEPT
{
	const char *const p = (char *)memchr(s, c, sz);
	if (p)
		return jstr_slip_mem_p_f(s, p - s, _src, sz - (p - s), _srclen);
	return s + sz;
}

/*
  Slip SRC after C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_slipaftc_mem(char **JSTR_RST const s,
		  size_t *JSTR_RST const sz,
		  size_t *JSTR_RST const cap,
		  const int c,
		  const char *JSTR_RST const _src,
		  const size_t _srclen) JSTR_NOEXCEPT
{
	const char *const p = (char *)memchr(*s, c, *sz);
	if (p)
		jstr_slip_mem(s, sz, cap, p - *s + 1, _src, _srclen);
}

/*
  Slip SRC after all C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
static char *
jstr_slipaftallc_mem_p_f(char *JSTR_RST const s,
			 const int c,
			 const char *JSTR_RST const _src,
			 size_t sz,
			 const size_t _srclen) JSTR_NOEXCEPT
{
	size_t off = 0;
	const char *p;
	while ((p = (char *)memchr(s + off, c, sz - off - *s))) {
		off = p - s;
		sz = jstr_slip_mem_p_f(s, off, _src, sz, _srclen) - s;
		off += _srclen + 1;
	}
	return s + sz;
}

/*
  Slip SRC after all C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_slipaftallc_mem(char **JSTR_RST const s,
		     size_t *JSTR_RST const sz,
		     size_t *JSTR_RST const cap,
		     const int c,
		     const char *JSTR_RST const _src,
		     const size_t _srclen) JSTR_NOEXCEPT
{
	size_t off = 0;
	const char *p;
#if JSTR_HAVE_REALLOC_MREMAP
	const int is_mmap = JSTR_IS_MMAP(*cap);
#endif /* JSTR_HAVE_REALLOC_MREMAP */
	while ((p = (char *)memchr(*s + off, c, *sz - off))) {
		off = p - *s;
#if JSTR_HAVE_REALLOC_MREMAP
		if (jstr_unlikely(is_mmap))
			priv_jstr_slip_mem_realloc(s, sz, cap, off, _src, _srclen);
		else
#endif /* JSTR_HAVE_REALLOC_MREMAP */
			priv_jstr_slip_mem_malloc(s, sz, cap, off, _src, _srclen);
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
jstr_slipaft_mem_f(char *JSTR_RST const s,
		   const char *JSTR_RST const _searc,
		   const char *JSTR_RST const _src,
		   const size_t sz,
		   const size_t _searclen,
		   const size_t _srclen) JSTR_NOEXCEPT
{
	switch (_searclen) {
	case 0: return s + sz;
	case 1: return jstr_slipaftc_mem_p_f(s, *_searc, _src, sz, _srclen);
	default: {
		const char *const p = (char *)PRIVATE_JSTR_MEMMEM(s, sz, _searc, _searclen);
		if (p)
			return jstr_slip_mem_p_f(s, p - s, _src, sz, _srclen);
		return s + sz;
	}
	}
}

/*
  Slip SRC after end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_slipaft_mem(char **JSTR_RST const s,
		 size_t *JSTR_RST const sz,
		 size_t *JSTR_RST const cap,
		 const char *JSTR_RST const _searc,
		 const char *JSTR_RST const _src,
		 const size_t _searclen,
		 const size_t _srclen) JSTR_NOEXCEPT
{
	switch (_searclen) {
	case 0: return;
	case 1:
		jstr_slipaftc_mem(s, sz, cap, *_searc, _src, _srclen);
		return;
	default: {
		const char *const p = (char *)PRIVATE_JSTR_MEMMEM(*s, *sz, _searc, _searclen);
		if (p)
			jstr_slip_mem(s, sz, cap, p - *s + _searclen, _src, _srclen);
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
jstr_slipaftall_mem_p_f(char *JSTR_RST const s,
			const char *JSTR_RST const _searc,
			const char *JSTR_RST const _src,
			size_t sz,
			const size_t _searclen,
			const size_t _srclen) JSTR_NOEXCEPT
{
	switch (_searclen) {
	case 0: return s + sz;
	case 1: return jstr_slipaftallc_mem_p_f(s, *_searc, _src, sz, _srclen);
	default: {
		size_t off = 0;
		const char *p;
		jstr_memmem_table t;
		jstr_memmem_init(&t);
		jstr_memmem_comp_mem(&t, _searc, _searclen);
		while ((p = (char *)jstr_memmem_exec(&t, s + off, sz - off))) {
			off = p - s;
			sz = jstr_slip_mem_p_f(s, off, _src, sz, _srclen) - s;
			off += _searclen + _srclen;
		}
		return s + sz;
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
jstr_slipaftall_mem(char **JSTR_RST const s,
		    size_t *JSTR_RST const sz,
		    size_t *JSTR_RST const cap,
		    const char *JSTR_RST const _searc,
		    const char *JSTR_RST const _src,
		    const size_t _searclen,
		    const size_t _srclen) JSTR_NOEXCEPT
{
	switch (_searclen) {
	case 0:
		return;
	case 1:
		jstr_slipaftallc_mem(s, sz, cap, *_searc, _src, _srclen);
		return;
	default: {
		size_t off = 0;
		const char *p;
#if JSTR_HAVE_REALLOC_MREMAP
		const int is_mmap = JSTR_IS_MMAP(*cap);
#endif /* JSTR_HAVE_REALLOC_MREMAP */
		jstr_memmem_table t;
		jstr_memmem_init(&t);
		jstr_memmem_comp_mem(&t, _searc, _searclen);
		while ((p = (char *)jstr_memmem_exec(&t, *s + off, *sz - off))) {
			off = p - *s;
#if JSTR_HAVE_REALLOC_MREMAP
			if (jstr_unlikely(is_mmap))
				priv_jstr_slip_mem_realloc(s, sz, cap, p - *s + _searclen, _src, _srclen);
			else
#endif /* JSTR_HAVE_REALLOC_MREMAP */
				priv_jstr_slip_mem_malloc(s, sz, cap, p - *s + _searclen, _src, _srclen);
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
jstr_rmc_mem_p(char *JSTR_RST s,
	       const int c,
	       const size_t sz) JSTR_NOEXCEPT
{
	const char *const sstart = s;
	s = (char *)memchr(s, c, sz);
	if (jstr_unlikely(s == NULL))
		return s + sz;
	memmove(s, s + 1, sz - (s - sstart) + 1);
	return s + sz - (s - sstart);
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
jstr_rmc_p(char *JSTR_RST s,
	   const int c) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	s = strchrnul(s, c);
	if (jstr_unlikely(*s == '\0'))
		return s;
	size_t len = strlen(s);
	memmove(s, s + 1, len);
	return s + len - 1;
#else
	return jstr_rmc_mem_p(s, c, strlen(s));
#endif /* HAS_STRCHRNUL */
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
priv_jstr_rmallc_mem_p(const priv_jstr_flag_ty flag,
		       char *JSTR_RST const s,
		       const int c,
		       size_t n,
		       const size_t sz) JSTR_NOEXCEPT
{
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *old = dst;
	const unsigned char *p = dst;
	const unsigned char *const end = dst + sz;
	while ((flag & PRIVATE_JSTR_FLAG_USE_N ? jstr_likely(n--) : 1)
	       && (p = (unsigned char *)memchr(p, c, end - p)))
		priv_jstr_rmall_in_place(&dst, &old, &p, 1);
	memmove(dst, old, end - old + 1);
	return (char *)dst + (end - old);
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
jstr_rmallc_mem_p(char *JSTR_RST const s,
		  const int c,
		  const size_t sz) JSTR_NOEXCEPT
{
	return priv_jstr_rmallc_mem_p(PRIVATE_JSTR_FLAG_USE_NOT_N, s, c, 0, sz);
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
jstr_rmallc_p(char *JSTR_RST const s,
	      const int c) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *old = dst;
	const unsigned char *p = dst;
	while (*(p = (unsigned char *)strchrnul((char *)p, c)))
		priv_jstr_rmall_in_place(&dst, &old, &p, 1);
	memmove(dst, old, p - old + 1);
	return (char *)dst + (p - old);
#else
	return jstr_rmallc_mem_p(s, c, strlen(s));
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
jstr_rmnc_mem_p(char *JSTR_RST const s,
		const int c,
		const size_t n,
		const size_t sz) JSTR_NOEXCEPT
{
	return priv_jstr_rmallc_mem_p(PRIVATE_JSTR_FLAG_USE_N, s, c, n, sz);
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
jstr_rmnc_p(char *JSTR_RST const s,
	    const int c,
	    size_t n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *old = dst;
	const unsigned char *p = dst;
	while (jstr_likely(n--) && *(p = (unsigned char *)strchrnul((char *)p, c)))
		priv_jstr_rmall_in_place(&dst, &old, &p, 1);
	memmove(dst, old, p - old + 1);
	return (char *)dst + (p - old);
#else
	return jstr_rmnc_mem_p(s, c, n, strlen(s));
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
jstr_stripspn_p(char *JSTR_RST const s,
		const char *JSTR_RST const _rjct) JSTR_NOEXCEPT
{
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *old = dst;
	const unsigned char *p = dst;
	while (*(p += strcspn((char *)p, _rjct)))
		priv_jstr_rmall_in_place(&dst, &old, &p, 1);
	memmove(dst, old, p - old + 1);
	return (char *)dst + (p - old);
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
jstr_rm_mem_p(char *JSTR_RST const s,
	      const char *JSTR_RST const _searc,
	      const size_t sz,
	      const size_t _searclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_searclen == 0))
		return s + sz;
	char *const p = (char *)PRIVATE_JSTR_MEMMEM(s, sz, _searc, _searclen);
	if (jstr_unlikely(p == NULL))
		return s + sz;
	memmove(p, p + _searclen, (s + sz) - p);
	return s + sz - _searclen;
}

/*
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_rplcc_mem(char *JSTR_RST s,
	       const int _searc,
	       const int _rplc,
	       const size_t sz) JSTR_NOEXCEPT
{
	s = (char *)memchr(s, _searc, sz);
	if (jstr_unlikely(s == NULL))
		return;
	*s = _rplc;
}

/*
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_rplcc(char *JSTR_RST s,
	   const int _searc,
	   const int _rplc) JSTR_NOEXCEPT
{
	s = strchr(s, _searc);
	if (jstr_unlikely(s == NULL))
		return;
	*s = _rplc;
}

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_rplcallc_mem(char *JSTR_RST s,
		  const int _searc,
		  const int _rplc,
		  const size_t sz) JSTR_NOEXCEPT
{
	const char *JSTR_RST const end = s + sz;
	while ((s = (char *)memchr(s, _searc, end - s)))
		*s++ = _rplc;
}

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_rplcallc(char *JSTR_RST s,
	      const int _searc,
	      const int _rplc) JSTR_NOEXCEPT
{
	while ((s = (strchr(s, _searc))))
		*s++ = _rplc;
}

/*
  Replace N SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_rplcnc_mem(char *JSTR_RST s,
		const int _searc,
		const int _rplc,
		size_t n,
		const size_t sz) JSTR_NOEXCEPT
{
	const char *JSTR_RST const end = s + sz;
	while (jstr_likely(--n) && (s = (char *)memchr(s, _searc, end - s)))
		*s++ = _rplc;
}

/*
  Replace N SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_rplcnc(char *JSTR_RST s,
	    const int _searc,
	    const int _rplc,
	    size_t n) JSTR_NOEXCEPT
{
	while ((s = strchr(s, _searc)))
		*s++ = _rplc;
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
jstr_rplc_mem_p_f(char *JSTR_RST const s,
		  const char *JSTR_RST const _searc,
		  const char *JSTR_RST const _rplc,
		  const size_t sz,
		  const size_t _searclen,
		  const size_t _rplclen) JSTR_NOEXCEPT
{
	switch (_rplclen) {
	case 0:
		return jstr_rm_mem_p(s, _searc, sz, _searclen);
	case 1:
		if (jstr_unlikely(_searclen == 1)) {
			jstr_rplcc_mem(s, *_searc, *_rplc, sz);
			return s + sz;
		}
		/* FALLTHROUGH */
	default: {
		if (jstr_unlikely(_searclen == 0))
			return s + sz;
		char *p = (char *)PRIVATE_JSTR_MEMMEM(s, sz, _searc, _searclen);
		if (jstr_unlikely(p == NULL))
			return s + sz;
		memmove(p + _rplclen,
			p + _searclen,
			(s + sz + 1) - p + _searclen);
		memcpy(p, _rplc, _rplclen);
		return s + sz - _rplclen - _searclen;
	}
	}
}

/*
  Replace first SEARCH in S with REPLACE.
*/
JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
static void
jstr_rplc_mem(char **JSTR_RST const s,
	      size_t *JSTR_RST const sz,
	      size_t *JSTR_RST const cap,
	      const char *JSTR_RST const _searc,
	      const char *JSTR_RST const _rplc,
	      const size_t _searclen,
	      const size_t _rplclen) JSTR_NOEXCEPT
{
	switch (_rplclen) {
	case 0:
		*sz = jstr_rm_mem_p(*s, _searc, *sz, _searclen) - *s;
		return;
	case 1:
		if (jstr_unlikely(_searclen == 1)) {
			jstr_rplcc_mem(*s, *_searc, *_rplc, *sz);
			return;
		}
		/* FALLTHROUGH */
	default: {
		if (jstr_unlikely(_searclen == 0))
			return;
		char *p = (char *)PRIVATE_JSTR_MEMMEM(*s, *sz, _searc, _searclen);
		if (jstr_unlikely(p == NULL))
			return;
		jstr_slip_mem(s, sz, cap, p - *s, _rplc, _rplclen);
	}
	}
}

/*
  Replace last SEARCH in S with REPLACE.
*/
JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
static void
jstr_rplclast_mem(char **JSTR_RST const s,
		  size_t *JSTR_RST const sz,
		  size_t *JSTR_RST const cap,
		  const char *JSTR_RST const _searc,
		  const char *JSTR_RST const _rplc,
		  const size_t _searclen,
		  const size_t _rplclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_searclen == 0))
		return;
	char *p = (char *)jstr_memrmem(*s, *sz, _searc, _searclen);
	if (jstr_unlikely(p == NULL))
		return;
	priv_jstr_rplcat_mem_may_lower(s, sz, cap, p - *s, _rplc, _rplclen, _searclen);
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
		      char *JSTR_RST const s,
		      const char *JSTR_RST const _searc,
		      size_t n,
		      size_t sz,
		      const size_t _searclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_searclen == 1))
		return priv_jstr_rmallc_mem_p(flag, s, *_searc, n, sz);
	if (jstr_unlikely(_searclen == 0))
		return s + sz;
	if (flag & PRIVATE_JSTR_FLAG_USE_N)
		if (jstr_unlikely(n == 0))
			return s + sz;
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *old = dst;
	const unsigned char *p = dst;
	const unsigned char *const end = dst + sz;
	jstr_memmem_table t;
	jstr_memmem_init(&t);
	jstr_memmem_comp_mem(&t, _searc, _searclen);
	while ((p = (unsigned char *)jstr_memmem_exec(&t, (char *)p, end - p))) {
		priv_jstr_rmall_in_place(&dst, &old, &p, _searclen);
		if (flag & PRIVATE_JSTR_FLAG_USE_N)
			if (jstr_unlikely(!--n))
				break;
	}
	memmove(dst, old, end - old + 1);
	return (char *)dst + (end - old);
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
jstr_rmn_mem_p(char *JSTR_RST const s,
	       const char *JSTR_RST const _searc,
	       size_t n,
	       size_t sz,
	       const size_t _searclen) JSTR_NOEXCEPT
{
	return priv_jstr_rmall_mem_p(PRIVATE_JSTR_FLAG_USE_N, s, _searc, n, sz, _searclen);
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
jstr_rmall_mem_p(char *JSTR_RST const s,
		 const char *JSTR_RST const _searc,
		 size_t sz,
		 const size_t _searclen) JSTR_NOEXCEPT
{
	return priv_jstr_rmall_mem_p(PRIVATE_JSTR_FLAG_USE_NOT_N, s, _searc, 0, sz, _searclen);
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
jstr_rmall_p(char *JSTR_RST const s,
	     const char *JSTR_RST const _searc,
	     const size_t _searclen) JSTR_NOEXCEPT
{
	return jstr_rmall_mem_p(s, _searc, strlen(s), _searclen);
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
		      char **JSTR_RST const s,
		      size_t *JSTR_RST const sz,
		      size_t *JSTR_RST const cap,
		      const char *JSTR_RST const _searc,
		      const char *JSTR_RST const _rplc,
		      size_t n,
		      const size_t _searclen,
		      const size_t _rplclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_rplclen == 0)) {
		*sz = priv_jstr_rmall_mem_p(flag, *s, _searc, n, *sz, _searclen) - *s;
		return;
	}
	if (jstr_unlikely(_searclen == 1 && _rplclen == 1)) {
		jstr_rplcc_mem(*s, *_searc, *_rplc, *sz);
		return;
	}
	if (jstr_unlikely(_searclen == 0))
		return;
	if (flag & PRIVATE_JSTR_FLAG_USE_N)
		if (jstr_unlikely(n == 0))
			return;
	jstr_memmem_table t;
	jstr_memmem_init(&t);
	jstr_memmem_comp_mem(&t, _searc, _searclen);
	typedef unsigned char uc;
	const unsigned char *p = *(uc **)s;
	const unsigned char *old = p;
	unsigned char *dst = *(uc **)s;
#if JSTR_HAVE_REALLOC_MREMAP
	const int is_mmap = JSTR_IS_MMAP(*cap);
#endif /* JSTR_HAVE_REALLOC_MREMAP */
	while ((p = (uc *)jstr_memmem_exec(&t, (char *)p, (*(uc **)s + *sz) - p))) {
		if (_rplclen <= _searclen)
			priv_jstr_rplcall_in_place(&dst, &old, &p, _rplc, _rplclen, _searclen);
#if JSTR_HAVE_REALLOC_MREMAP
		else if (jstr_unlikely(is_mmap))
			p = (uc *)priv_jstr_rplcat_mem_realloc(s, sz, cap, p - *(uc **)s, _rplc, _rplclen, _searclen);
#endif /* JSTR_HAVE_REALLOC_MREMAP */
		else
			p = (uc *)priv_jstr_rplcat_mem_malloc(s, sz, cap, p - *(uc **)s, _rplc, _rplclen, _searclen);
		if (flag & PRIVATE_JSTR_FLAG_USE_N)
			if (jstr_unlikely(!--n))
				break;
	}
	if (_rplclen < _searclen) {
		memmove(dst, old, *(uc **)s + *sz - old + 1);
		*sz = (char *)dst + (*(uc **)s + *sz - old) - *s;
	}
}

/*
  Replace N SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
static void
jstr_rplcn_mem(char **JSTR_RST const s,
	       size_t *JSTR_RST const sz,
	       size_t *JSTR_RST const cap,
	       const char *JSTR_RST const _searc,
	       const char *JSTR_RST const _rplc,
	       size_t n,
	       const size_t _searclen,
	       const size_t _rplclen) JSTR_NOEXCEPT
{
	priv_jstr_rplcall_mem(PRIVATE_JSTR_FLAG_USE_N, s, sz, cap, _searc, _rplc, n, _searclen, _rplclen);
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
static void
jstr_rplcall_mem(char **JSTR_RST const s,
		 size_t *JSTR_RST const sz,
		 size_t *JSTR_RST const cap,
		 const char *JSTR_RST const _searc,
		 const char *JSTR_RST const _rplc,
		 const size_t _searclen,
		 const size_t _rplclen) JSTR_NOEXCEPT
{
	priv_jstr_rplcall_mem(PRIVATE_JSTR_FLAG_USE_NOT_N, s, sz, cap, _searc, _rplc, 0, _searclen, _rplclen);
}

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#endif /* JSTR_H_REPLACE_DEF */
