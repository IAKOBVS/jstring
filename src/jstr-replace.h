#ifndef JSTR_H_REPLACE_DEF
#define JSTR_H_REPLACE_DEF 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-ctype.h"
#include "jstr-macros.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

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
  Assume that S have enough space for SRC.
*/
JSTR_INLINE
JSTR_FUNC_VOID
static void
pjstr_slip_len_p_f(char *R const s,
		   const size_t at,
		   const char *R const src,
		   const size_t sz,
		   const size_t srclen) JSTR_NOEXCEPT
{
	memmove(s + at + srclen,
		s + at,
		sz - at + 1);
	memcpy(s + at, src, srclen);
}

JSTR_INLINE
JSTR_FUNC_RET_NONNULL
static char *
pjstr_rplcat_len_f(char *R const s,
		   size_t *R const sz,
		   const size_t at,
		   const char *R const rplc,
		   const size_t rplclen,
		   const size_t findlen) JSTR_NOEXCEPT
{
	memmove(s + at + rplclen,
		s + at + findlen,
		*sz - (at + findlen) + 1);
	memcpy(s + at, rplc, rplclen);
	*sz += rplclen - findlen;
	return s + at + rplclen;
}

/*
  Slip SRC into DST[AT].
  Return 0 on malloc error;
  otherwise 1.
*/
JSTR_FUNC
static int
jstr_slip_len(char *R *R const s,
	      size_t *R const sz,
	      size_t *R const cap,
	      const size_t at,
	      const char *R const rplc,
	      const size_t rplclen) JSTR_NOEXCEPT
{
	if (*cap < *sz + rplclen)
		PJSTR_REALLOC(*s, *cap, *sz + rplclen, return 0);
	pjstr_slip_len_p_f(*s, at, rplc, *sz, rplclen);
	*sz += rplclen;
	return 1;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_slip(char *R *R const s,
	  size_t *R const sz,
	  size_t *R const cap,
	  const size_t at,
	  const char *R const rplc) JSTR_NOEXCEPT
{
	return jstr_slip_len(s, sz, cap, at, rplc, strlen(rplc));
}

JSTR_FUNC
static char *
pjstr_rplcat_len_may_lower(char *R *R const s,
			   size_t *R const sz,
			   size_t *R const cap,
			   const size_t at,
			   const char *R const rplc,
			   const size_t rplclen,
			   const size_t findlen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(rplclen == findlen)) {
		memcpy(*s + at, rplc, rplclen);
		goto ret;
	} else if (*cap > *sz + rplclen - findlen) {
		return pjstr_rplcat_len_f(*s, sz, at, rplc, rplclen, findlen);
	} else {
		PJSTR_REALLOC(*s, *cap, *sz + rplclen - findlen, return NULL);
		return pjstr_rplcat_len_f(*s, sz, at, rplc, rplclen, findlen);
	}
	*sz += rplclen - findlen;
ret:
	return *s + at + rplclen;
}

JSTR_INLINE
JSTR_FUNC
static char *
pjstr_rplcat_len(char *R *R const s,
		 size_t *R const sz,
		 size_t *R const cap,
		 const size_t at,
		 const char *R const rplc,
		 const size_t rplclen,
		 const size_t findlen) JSTR_NOEXCEPT
{
	if (*cap < *sz + rplclen - findlen)
		PJSTR_REALLOC(*s, *cap, *sz + rplclen, return NULL);
	return pjstr_rplcat_len_f(*s, sz, at, rplc, rplclen, findlen);
}

/*
  Slip SRC after C in DST.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_INLINE
JSTR_FUNC
static int
jstr_slipafterchr_len(char *R *R const s,
		      size_t *R const sz,
		      size_t *R const cap,
		      const int c,
		      const char *R const src,
		      const size_t srclen) JSTR_NOEXCEPT
{
	const char *const p = (char *)memchr(*s, c, *sz);
	if (p != NULL)
		return jstr_slip_len(s, sz, cap, p - *s + 1, src, srclen);
	return 1;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_slipafterchr(char *R *R const s,
		  size_t *R const sz,
		  size_t *R const cap,
		  const int c,
		  const char *R const src) JSTR_NOEXCEPT
{
	return jstr_slipafterchr_len(s, sz, cap, c, src, strlen(src));
}

/*
  Slip SRC after all C in DST.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_slipafterallchr_len(char *R *R const s,
			 size_t *R const sz,
			 size_t *R const cap,
			 const int c,
			 const char *R const src,
			 const size_t srclen) JSTR_NOEXCEPT
{
	size_t off = 0;
	const char *p;
	while ((p = (char *)memchr(*s + off, c, *sz - off))) {
		off = p - *s;
		if (jstr_unlikely(!jstr_slip_len(s, sz, cap, off, src, srclen)))
			return 0;
		off += srclen + 1;
	}
	return 1;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_slipafterallchr(char *R *R const s,
		     size_t *R const sz,
		     size_t *R const cap,
		     const int c,
		     const char *R const src) JSTR_NOEXCEPT
{
	return jstr_slipafterallchr_len(s, sz, cap, c, src, strlen(src));
}

/*
  Slip SRC after end of NE in DST.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_slipafter_len(char *R *R const s,
		   size_t *R const sz,
		   size_t *R const cap,
		   const char *R const find,
		   const char *R const src,
		   const size_t findlen,
		   const size_t srclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(findlen == 1))
		return jstr_slipafterchr_len(s, sz, cap, *find, src, srclen);
	if (jstr_unlikely(findlen == 0))
		return 1;
	const char *const p = (char *)PJSTR_MEMMEM(*s, *sz, find, findlen);
	if (p != NULL)
		return jstr_slip_len(s, sz, cap, p - *s + findlen, src, srclen);
	return 1;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_slipafter(char *R *R const s,
	       size_t *R const sz,
	       size_t *R const cap,
	       const char *R const find,
	       const char *R const src) JSTR_NOEXCEPT
{
	return jstr_slipafter_len(s, sz, cap, find, src, strlen(find), strlen(src));
}

/*
  Slip SRC after all end of NE in DST.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_slipafterall_len(char *R *R const s,
		      size_t *R const sz,
		      size_t *R const cap,
		      const char *R const find,
		      const char *R const src,
		      const size_t findlen,
		      const size_t srclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(findlen == 1))
		return jstr_slipafterallchr_len(s, sz, cap, *find, src, srclen);
	if (jstr_unlikely(findlen == 0))
		return 1;
	size_t off = 0;
	const char *p;
	while ((p = (char *)PJSTR_MEMMEM(*s + off, *sz - off, find, findlen))) {
		off = p - *s;
		if (jstr_unlikely(!jstr_slip_len(s, sz, cap, p - *s + findlen, src, srclen)))
			return 0;
		off += findlen + srclen;
	}
	return 1;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_slipafterall(char *R *R const s,
		  size_t *R const sz,
		  size_t *R const cap,
		  const char *R const find,
		  const char *R const src) JSTR_NOEXCEPT
{
	return jstr_slipafterall_len(s, sz, cap, find, src, strlen(find), strlen(src));
}

/*
  Remove first C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_INLINE
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmchr_len_p(char *R s,
		 const int c,
		 const size_t sz) JSTR_NOEXCEPT
{
	const char *const start = s;
	s = (char *)memchr(s, c, sz);
	if (jstr_unlikely(s == NULL))
		return s + sz;
	memmove(s, s + 1, sz - (s - start));
	s[sz - (s - start)] = '\0';
	return s + sz - (s - start);
}

/*
  Remove first C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_INLINE
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmchr_p(char *R s,
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
	return jstr_rmchr_len_p(s, c, strlen(s));
#endif /* HAVE_STRCHRNUL */
}

/*
  Remove all REJECT in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmspn_p(char *R s,
	     const char *R reject) JSTR_NOEXCEPT
{
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *old = dst;
	const unsigned char *p = dst;
	size_t findlen;
	while (jstr_likely(*(p += strcspn((char *)p, reject)))) {
		findlen = strspn((char *)p, reject);
		PJSTR_RMALL_IN_PLACE(dst, old, p, findlen);
		if (jstr_unlikely(*p == '\0'))
			break;
	}
	if (jstr_likely(dst != old)) {
		memmove(dst, old, p - old);
		dst[p - old] = '\0';
	}
	return (char *)dst + (p - old);
}

/*
  Remove REJECT in S.
*/
JSTR_INLINE
JSTR_FUNC_VOID
static void
jstr_rmspn_j(jstr_ty *R const j,
	     const char *R reject) JSTR_NOEXCEPT
{
	j->size = jstr_rmspn_p(j->data, reject) - j->data;
}

typedef enum {
	PJSTR_FLAG_USE_N = 1,
	PJSTR_FLAG_USE_NOT_N = 1 << 1,
} pjstr_flag_use_n_ty;

JSTR_INLINE
JSTR_FUNC_RET_NONNULL
static char *
pjstr_rmallchr_len_p(const pjstr_flag_use_n_ty flag,
		     char *R const s,
		     const int c,
		     size_t n,
		     const size_t sz) JSTR_NOEXCEPT
{
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *old = dst;
	const unsigned char *p = dst;
	const unsigned char *const end = dst + sz;
	while ((flag & PJSTR_FLAG_USE_N ? n-- : 1)
	       && (p = (unsigned char *)memchr(p, c, end - p)))
		PJSTR_RMALL_IN_PLACE(dst, old, p, 1);
	if (jstr_unlikely(dst == (unsigned char *)s))
		return s + sz;
	memmove(dst, old, end - old);
	dst[end - old] = '\0';
	return (char *)dst + (end - old);
}

/*
  Remove all C in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmallchr_len_p(char *R const s,
		    const int c,
		    const size_t sz) JSTR_NOEXCEPT
{
	return pjstr_rmallchr_len_p(PJSTR_FLAG_USE_NOT_N, s, c, 0, sz);
}

/*
  Remove all C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmallchr_p(char *R const s,
		const int c) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *old = dst;
	const unsigned char *p = dst;
	while (*(p = (unsigned char *)jstr_strchrnul((char *)p, c)))
		PJSTR_RMALL_IN_PLACE(dst, old, p, 1);
	if (jstr_unlikely(dst == (unsigned char *)s))
		return (char *)p;
	memmove(dst, old, p - old);
	dst[p - old] = '\0';
	return (char *)dst + (p - old);
#else
	return jstr_rmallchr_len_p(s, c, strlen(s));
#endif /* HAVE_STRCHRNUL */
}

/*
  Remove N C in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmnc_len_p(char *R const s,
		const int c,
		const size_t n,
		const size_t sz) JSTR_NOEXCEPT
{
	return pjstr_rmallchr_len_p(PJSTR_FLAG_USE_N, s, c, n, sz);
}

/*
  Remove N C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_INLINE
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmnc_p(char *R const s,
	    const int c,
	    size_t n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *old = dst;
	const unsigned char *p = dst;
	while (n-- && *(p = (unsigned char *)strchrnul((char *)p, c)))
		PJSTR_RMALL_IN_PLACE(dst, old, p, 1);
	if (jstr_unlikely(dst == (unsigned char *)s))
		return s + n;
	memmove(dst, old, p - old);
	dst[p - old] = '\0';
	return (char *)dst + (p - old);
#else
	return jstr_rmnc_len_p(s, c, n, strlen(s));
#endif /* HAVE_STRCHRNUL */
}

/*
  Remove characters in REJECT in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_stripspn_p(char *R const s,
		const char *R const rjct) JSTR_NOEXCEPT
{
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *old = dst;
	const unsigned char *p = dst;
	while (*(p += strcspn((char *)p, rjct)))
		PJSTR_RMALL_IN_PLACE(dst, old, p, 1);
	if (jstr_unlikely(dst == (unsigned char *)s))
		return (char *)p;
	memmove(dst, old, p - old);
	dst[p - old] = '\0';
	return (char *)dst + (p - old);
}

/*
  Remove first HS in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_FUNC_RET_NONNULL
static char *
jstr_rm_len_p(char *R const s,
	      const char *R const find,
	      const size_t sz,
	      const size_t findlen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(findlen == 0))
		return s + sz;
	char *const p = (char *)PJSTR_MEMMEM(s, sz, find, findlen);
	if (jstr_unlikely(p == NULL))
		return s + sz;
	memmove(p, p + findlen, (s + sz) - p);
	return s + sz - findlen;
}

/*
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_FUNC_VOID
static void
jstr_rplcchr_len(char *R s,
		 const int find,
		 const int rplc,
		 const size_t sz) JSTR_NOEXCEPT
{
	s = (char *)memchr(s, find, sz);
	if (jstr_unlikely(s == NULL))
		return;
	*s = rplc;
}

/*
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_FUNC_VOID
static void
jstr_rplcchr(char *R s,
	     const int find,
	     const int rplc) JSTR_NOEXCEPT
{
	s = strchr(s, find);
	if (jstr_unlikely(s == NULL))
		return;
	*s = rplc;
}

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_FUNC_VOID
static void
jstr_rplcallchr_len(char *R s,
		    const int find,
		    const int rplc,
		    const size_t sz) JSTR_NOEXCEPT
{
	const char *R const end = s + sz;
	while ((s = (char *)memchr(s, find, end - s)))
		*s++ = rplc;
}

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_FUNC_VOID
static void
jstr_rplcallchr(char *R s,
		const int find,
		const int rplc) JSTR_NOEXCEPT
{
	while ((s = (strchr(s, find))))
		*s++ = rplc;
}

/*
  Replace N SEARCH in REPLACE.
*/
JSTR_FUNC_VOID
static void
jstr_rplcnc_len(char *R s,
		const int find,
		const int rplc,
		size_t n,
		const size_t sz) JSTR_NOEXCEPT
{
	const char *R const end = s + sz;
	while (n-- && (s = (char *)memchr(s, find, end - s)))
		*s++ = rplc;
}

/*
  Replace N SEARCH in REPLACE.
*/
JSTR_FUNC_VOID
static void
jstr_rplcnc(char *R s,
	    const int find,
	    const int rplc,
	    size_t n) JSTR_NOEXCEPT
{
	while (n-- && (s = strchr(s, find)))
		*s++ = rplc;
}

/*
  Replace first SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_rplc_len(char *R *R const s,
	      size_t *R const sz,
	      size_t *R const cap,
	      const char *R const find,
	      const char *R const rplc,
	      const size_t findlen,
	      const size_t rplclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(rplclen == 0)) {
		*sz = jstr_rm_len_p(*s, find, *sz, findlen) - *s;
		return 1;
	}
	if (jstr_unlikely(rplclen == 1)) {
		if (jstr_unlikely(findlen == 1)) {
			jstr_rplcchr_len(*s, *find, *rplc, *sz);
			return 1;
		}
	} else if (jstr_unlikely(findlen == 0))
		return 1;
	char *p = (char *)PJSTR_MEMMEM(*s, *sz, find, findlen);
	if (jstr_unlikely(p == NULL))
		return 1;
	return jstr_slip_len(s, sz, cap, p - *s, rplc, rplclen);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_rplc(char *R *R const s,
	  size_t *R const sz,
	  size_t *R const cap,
	  const char *R const find,
	  const char *R const rplc) JSTR_NOEXCEPT
{
	return jstr_rplc_len(s, sz, cap, find, rplc, strlen(find), strlen(rplc));
}

/*
  Replace last SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC_VOID
static int
jstr_rplclast_len(char *R *R const s,
		  size_t *R const sz,
		  size_t *R const cap,
		  const char *R const find,
		  const char *R const rplc,
		  const size_t findlen,
		  const size_t rplclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(findlen == 0))
		return 1;
	char *p = (char *)jstr_strrstr_len(*s, *sz, find, findlen);
	if (jstr_unlikely(p == NULL))
		return 1;
	return pjstr_rplcat_len_may_lower(s, sz, cap, p - *s, rplc, rplclen, findlen) ? 1 : 0;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_rplclast(char *R *R const s,
	      size_t *R const sz,
	      size_t *R const cap,
	      const char *R const find,
	      const char *R const rplc) JSTR_NOEXCEPT
{
	return jstr_rplclast_len(s, sz, cap, find, rplc, strlen(find), strlen(rplc));
}

JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
pjstr_rmall_len_p(const pjstr_flag_use_n_ty flag,
		  char *R const s,
		  const char *R const find,
		  size_t n,
		  size_t sz,
		  const size_t findlen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(findlen == 1))
		return pjstr_rmallchr_len_p(flag, s, *find, n, sz);
	if (jstr_unlikely(findlen == 0))
		return s + sz;
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *old = dst;
	const unsigned char *p = dst;
	const unsigned char *const end = dst + sz;
	while (((flag & PJSTR_FLAG_USE_N) ? n-- : 1)
	       && (p = (unsigned char *)PJSTR_MEMMEM((char *)p, end - p, find, findlen)))
		PJSTR_RMALL_IN_PLACE(dst, old, p, findlen);
	if (jstr_unlikely(dst == (unsigned char *)s))
		return s + sz;
	memmove(dst, old, end - old);
	dst[end - old] = '\0';
	return (char *)dst + (end - old);
}

/*
  Remove N SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmn_len_p(char *R const s,
	       const char *R const find,
	       size_t n,
	       size_t sz,
	       const size_t findlen) JSTR_NOEXCEPT
{
	return pjstr_rmall_len_p(PJSTR_FLAG_USE_N, s, find, n, sz, findlen);
}

/*
  Remove all SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmall_len_p(char *R const s,
		 const char *R const find,
		 size_t sz,
		 const size_t findlen) JSTR_NOEXCEPT
{
	return pjstr_rmall_len_p(PJSTR_FLAG_USE_NOT_N, s, find, 0, sz, findlen);
}

/*
  Remove all SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_WARN_UNUSED
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmall_p(char *R const s,
	     const char *R const find,
	     const size_t findlen) JSTR_NOEXCEPT
{
	return jstr_rmall_len_p(s, find, strlen(s), findlen);
}

/*
  Remove all SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_FUNC_VOID
static void
jstr_rmall_len_j(jstr_ty *R const j,
		 const char *R const find,
		 const size_t findlen) JSTR_NOEXCEPT
{
	j->size = jstr_rmall_len_p(j->data, find, j->size, findlen) - j->data;
}

/*
  Remove all SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_FUNC_VOID
static void
jstr_rmall_j(jstr_ty *R const j,
	     const char *R const find) JSTR_NOEXCEPT
{
	return jstr_rmall_len_j(j, find, strlen(find));
}

JSTR_INLINE
JSTR_FUNC
static int
pjstr_rplcall_len(const pjstr_flag_use_n_ty flag,
		  char *R *R const s,
		  size_t *R const sz,
		  size_t *R const cap,
		  const char *R const find,
		  const char *R const rplc,
		  size_t n,
		  const size_t findlen,
		  const size_t rplclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(rplclen == 0)) {
		*sz = pjstr_rmall_len_p(flag, *s, find, n, *sz, findlen) - *s;
		return 1;
	}
	if (jstr_unlikely(findlen == 1)) {
		if (jstr_unlikely(rplclen == 1)) {
			jstr_rplcchr_len(*s, *find, *rplc, *sz);
			return 1;
		}
	} else if (jstr_unlikely(findlen == 0))
		return 1;
	typedef unsigned char u;
	const unsigned char *p = *(u **)s;
	const unsigned char *old = p;
	unsigned char *dst = *(u **)s;
	while (((flag & PJSTR_FLAG_USE_N) ? n-- : 1)
	       && (p = (u *)PJSTR_MEMMEM((char *)p, (*(u **)s + *sz) - p, find, findlen))) {
		if (rplclen <= findlen)
			PJSTR_RPLCALL_IN_PLACE(dst, old, p, rplc, rplclen, findlen);
		else
			p = (u *)pjstr_rplcat_len(s, sz, cap, p - *(u **)s, rplc, rplclen, findlen);
		if (jstr_unlikely(p == NULL))
			return 0;
	}
	if (jstr_unlikely(dst == (unsigned char *)s))
		return 1;
	if (rplclen < findlen) {
		memmove(dst, old, *(u **)s + *sz - old);
		dst[*(u **)s + *sz - old] = '\0';
		*sz = (char *)dst + (*(u **)s + *sz - old) - *s;
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
jstr_rplcn_len(char *R *R const s,
	       size_t *R const sz,
	       size_t *R const cap,
	       const char *R const find,
	       const char *R const rplc,
	       size_t n,
	       const size_t findlen,
	       const size_t rplclen) JSTR_NOEXCEPT
{
	return pjstr_rplcall_len(PJSTR_FLAG_USE_N, s, sz, cap, find, rplc, n, findlen, rplclen);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_rplcn(char *R *R const s,
	   size_t *R const sz,
	   size_t *R const cap,
	   const char *R const find,
	   const char *R const rplc,
	   size_t n) JSTR_NOEXCEPT
{
	return jstr_rplcn_len(s, sz, cap, find, rplc, n, strlen(find), strlen(rplc));
}

/*
  Replace all SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_rplcall_len(char *R *R const s,
		 size_t *R const sz,
		 size_t *R const cap,
		 const char *R const find,
		 const char *R const rplc,
		 const size_t findlen,
		 const size_t rplclen) JSTR_NOEXCEPT
{
	return pjstr_rplcall_len(PJSTR_FLAG_USE_NOT_N, s, sz, cap, find, rplc, 0, findlen, rplclen);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_rplcall(char *R *R const s,
	     size_t *R const sz,
	     size_t *R const cap,
	     const char *R const find,
	     const char *R const rplc) JSTR_NOEXCEPT
{
	return jstr_rplcall_len(s, sz, cap, find, rplc, strlen(find), strlen(rplc));
}

/*
  Reverse S.
*/
JSTR_FUNC_VOID
static void
jstr_rev_len(char *R s,
	     const size_t sz) JSTR_NOEXCEPT
{
	int c;
	char *e = s + sz - 1;
	while (s < e) {
		c = *s;
		*s++ = *e;
		*e-- = c;
	}
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_rev(char *R s) JSTR_NOEXCEPT
{
	jstr_rev_len(s, strlen(s));
}

/*
  Reverse S.
  Return value:
  ptr to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rev_p(char *R s) JSTR_NOEXCEPT
{
	const size_t len = strlen(s);
	jstr_rev_len(s, len);
	return s + len;
}

/*
  Trim leading and trailing [\n\t\v\r ] in S.
  Return value:
  ptr to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_trim_len_p(char *R s,
		const size_t sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return s + sz;
	typedef unsigned char u;
	unsigned char *end = (u *)s + sz - 1;
	unsigned char *const start = (u *)s - 1;
	while (jstr_isspace(*end)
	       && --end != start)
		;
	*++end = '\0';
	while (jstr_isspace(*s++))
		;
	--s;
	memmove(start + 1, s, end - (u *)s);
	(start + 1)[end - (u *)s] = '\0';
	return (char *)start + (end - (u *)s);
}

/*
  Trim leading and trailing [\n\t\v\r ] in S.
  Return value:
  ptr to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstr_trim_p(char *R const s) JSTR_NOEXCEPT
{
	return jstr_trim_len_p(s, strlen(s));
}

/*
  Trim leading and trailing [\n\t\v\r ] in S.
  Return value:
  ptr to '\0' in S;
*/
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_trim_j(jstr_ty *R const j) JSTR_NOEXCEPT
{
	j->size = jstr_trim_len_p(j->data, j->size) - j->data;
}

/*
  Insert SRC into DST[AT].
  Assume that S have enough space for SRC.
  Return value:
*/
JSTR_INLINE
JSTR_FUNC_VOID
static void
pjstr_insert_len_f(char *R const s,
		   const size_t at,
		   const char *R const src,
		   const size_t srclen) JSTR_NOEXCEPT
{
	memcpy(s + at, src, srclen);
}

/*
  Insert SRC into DST[AT].
  Return 0 on malloc error;
  otherwise 1.
*/
JSTR_INLINE
JSTR_FUNC
static int
jstr_insert_len(char *R *R const s,
		size_t *R const sz,
		size_t *R const cap,
		const size_t at,
		const char *R const src,
		const size_t srclen) JSTR_NOEXCEPT
{
	if (at + srclen > *sz) {
		PJSTR_REALLOC(*s, *cap, at + srclen + 1, return 0);
		*sz = at + srclen;
		(*s)[*sz] = '\0';
	}
	pjstr_insert_len_f(*s, at, src, srclen);
	return 1;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_insert(char *R *R const s,
	    size_t *R const sz,
	    size_t *R const cap,
	    const size_t at,
	    const char *R const src) JSTR_NOEXCEPT
{
	return jstr_insert_len(s, sz, cap, at, src, strlen(src));
}

/*
  Insert SRC after C in DST.
  Return 0 on malloc error;
  otherwise 1.
*/
JSTR_INLINE
JSTR_FUNC
static int
jstr_insertafterchr_len(char *R *R const s,
			size_t *R const sz,
			size_t *R const cap,
			const int c,
			const char *R const src,
			const size_t srclen) JSTR_NOEXCEPT
{
	const char *const p = (char *)memchr(*s, c, *sz);
	if (p != NULL)
		return jstr_insert_len(s, sz, cap, p - *s + 1, src, srclen);
	return 1;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_insertafterchr(char *R *R const s,
		    size_t *R const sz,
		    size_t *R const cap,
		    const int c,
		    const char *R const src) JSTR_NOEXCEPT
{
	return jstr_insertafterchr_len(s, sz, cap, c, src, strlen(src));
}

/*
  Insert SRC after end of NE in DST.
  Return 0 on malloc error;
  otherwise 1.
*/
JSTR_FUNC
static int
jstr_insertafter_len(char *R *R const s,
		     size_t *R const sz,
		     size_t *R const cap,
		     const char *R const find,
		     const char *R const src,
		     const size_t findlen,
		     const size_t srclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(findlen == 1))
		return jstr_insertafterchr_len(s, sz, cap, *find, src, srclen);
	if (jstr_unlikely(findlen == 0))
		return 1;
	const char *const p = (char *)PJSTR_MEMMEM(*s, *sz, find, findlen);
	if (p != NULL)
		return jstr_insert_len(s, sz, cap, p - *s + findlen, src, srclen);
	return 1;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_insertafter(char *R *R const s,
		 size_t *R const sz,
		 size_t *R const cap,
		 const char *R const find,
		 const char *R const src) JSTR_NOEXCEPT
{
	return jstr_insertafter_len(s, sz, cap, find, src, strlen(find), strlen(src));
}

PJSTR_END_DECLS

#undef R

#endif /* JSTR_H_REPLACE_DEF */
