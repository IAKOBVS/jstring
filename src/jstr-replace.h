#ifndef JSTR_H_REPLACE_DEF
#define JSTR_H_REPLACE_DEF 1

#include "jstr-macros.h"

P_JSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
P_JSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-ctype.h"
#include "jstr-macros.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

P_JSTR_BEGIN_DECLS

#define P_JSTR_RMALL_IN_PLACE(dst, oldp, p, findlen)  \
	do {                                          \
		if (jstr_likely(dst != oldp))         \
			memmove(dst, oldp, p - oldp); \
		dst += p - oldp;                      \
		oldp += p - oldp;                     \
		oldp += findlen;                      \
		p += findlen;                         \
	} while (0)
#define P_JSTR_RPLCALL_IN_PLACE(dst, oldp, p, rplc, rplclen, findlen) \
	do {                                                          \
		if (jstr_likely(findlen != rplclen && dst != oldp))   \
			memmove(dst, oldp, p - oldp);                 \
		dst += p - oldp;                                      \
		oldp += p - oldp;                                     \
		oldp += findlen;                                      \
		p += findlen;                                         \
		memcpy(dst, rplc, rplclen);                           \
		dst += rplclen;                                       \
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
jstr_slip_unsafe(char *R const s,
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
jstr_replaceat_len_unsafe(char *R const s,
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
		P_JSTR_REALLOC(*s, *cap, *sz + rplclen, goto err);
	jstr_slip_unsafe(*s, at, rplc, *sz, rplclen);
	*sz += rplclen;
	return 1;
err:
	P_JSTR_NULLIFY_MEMBERS(*sz, *cap);
	return 0;
}

JSTR_FUNC
static char *
jstr_replaceat_len_may_lower(char *R *R const s,
			     size_t *R const sz,
			     size_t *R const cap,
			     const size_t at,
			     const char *R const rplc,
			     const size_t rplclen,
			     const size_t findlen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(rplclen == findlen)) {
		memcpy(*s + at, rplc, rplclen);
		*sz += rplclen - findlen;
		return *s + at + rplclen;
	}
	if (*cap <= *sz + rplclen - findlen)
		P_JSTR_REALLOC(*s, *cap, *sz + rplclen - findlen, goto err);
	return jstr_replaceat_len_unsafe(*s, sz, at, rplc, rplclen, findlen);
err:
	P_JSTR_NULLIFY_MEMBERS(*sz, *cap);
	return NULL;
}

JSTR_INLINE
JSTR_FUNC
static char *
jstr_replaceat_len(char *R *R const s,
		   size_t *R const sz,
		   size_t *R const cap,
		   const size_t at,
		   const char *R const rplc,
		   const size_t rplclen,
		   const size_t findlen) JSTR_NOEXCEPT
{
	if (*cap < *sz + rplclen - findlen)
		P_JSTR_REALLOC(*s, *cap, *sz + rplclen, return NULL);
	return jstr_replaceat_len_unsafe(*s, sz, at, rplc, rplclen, findlen);
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
	const char *const p = JSTR_STRSTR_LEN(*s, *sz, find, findlen);
	if (p != NULL)
		return jstr_slip_len(s, sz, cap, p - *s + findlen, src, srclen);
	return 1;
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
	while ((p = JSTR_STRSTR_LEN(*s + off, *sz - off, find, findlen))) {
		off = p - *s;
		if (jstr_unlikely(!jstr_slip_len(s, sz, cap, p - *s + findlen, src, srclen)))
			return 0;
		off += findlen + srclen;
	}
	return 1;
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
	memmove(s, s + 1, sz - (s - start) + 1);
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
		P_JSTR_RMALL_IN_PLACE(dst, old, p, findlen);
		if (jstr_unlikely(*p == '\0'))
			break;
	}
	if (jstr_likely(dst != old))
		memmove(dst, old, p - old + 1);
	return (char *)(dst + (p - old));
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
	P_JSTR_FLAG_USE_N = (1),
	P_JSTR_FLAG_USE_NOT_N = (P_JSTR_FLAG_USE_N << 1)
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
	while ((flag & P_JSTR_FLAG_USE_N ? n-- : 1)
	       && (p = (unsigned char *)memchr(p, c, end - p)))
		P_JSTR_RMALL_IN_PLACE(dst, old, p, 1);
	if (jstr_unlikely(dst == (unsigned char *)s))
		return s + sz;
	memmove(dst, old, end - old + 1);
	return (char *)(dst + (end - old));
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
	return pjstr_rmallchr_len_p(P_JSTR_FLAG_USE_NOT_N, s, c, 0, sz);
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
		P_JSTR_RMALL_IN_PLACE(dst, old, p, 1);
	if (jstr_unlikely(dst == (unsigned char *)s))
		return (char *)p;
	memmove(dst, old, p - old + 1);
	return (char *)(dst + (p - old));
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
	return pjstr_rmallchr_len_p(P_JSTR_FLAG_USE_N, s, c, n, sz);
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
		P_JSTR_RMALL_IN_PLACE(dst, old, p, 1);
	if (jstr_unlikely(dst == (unsigned char *)s))
		return s + n;
	memmove(dst, old, p - old + 1);
	return (char *)(dst + (p - old));
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
		P_JSTR_RMALL_IN_PLACE(dst, old, p, 1);
	if (jstr_unlikely(dst == (unsigned char *)s))
		return (char *)p;
	memmove(dst, old, p - old + 1);
	return (char *)(dst + (p - old));
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
	char *const p = JSTR_STRSTR_LEN(s, sz, find, findlen);
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
jstr_replacechr_len(char *R s,
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
jstr_replacechr(char *R s,
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
jstr_replaceallchr_len(char *R s,
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
jstr_replaceallchr(char *R s,
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
jstr_replacenc_len(char *R s,
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
jstr_replacenc(char *R s,
	       const int find,
	       const int rplc,
	       size_t n) JSTR_NOEXCEPT
{
	while (n-- && (s = strchr(s, find)))
		*s++ = rplc;
}

JSTR_FUNC
static int
pjstr_replace_len(char *R *R const s,
		  size_t *R const sz,
		  size_t *R const cap,
		  const size_t start_idx,
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
			jstr_replacechr_len(*s, *find, *rplc, *sz);
			return 1;
		}
	} else if (jstr_unlikely(findlen == 0))
		return 1;
	char *p = JSTR_STRSTR_LEN(*s, *sz, find, findlen);
	if (jstr_unlikely(p == NULL))
		return 1;
	return jstr_slip_len(s, sz, cap, p - *s, rplc, rplclen);
}

/*
  Replace first SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_replace_len(char *R *R const s,
		 size_t *R const sz,
		 size_t *R const cap,
		 const char *R const find,
		 const char *R const rplc,
		 const size_t findlen,
		 const size_t rplclen) JSTR_NOEXCEPT
{
	return pjstr_replace_len(s, sz, cap, 0, find, rplc, findlen, rplclen);
}

/*
  Replace first SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_replace_len_from_unsafe(char *R *R const s,
			     size_t *R const sz,
			     size_t *R const cap,
			     const size_t start_idx,
			     const char *R const find,
			     const char *R const rplc,
			     const size_t findlen,
			     const size_t rplclen) JSTR_NOEXCEPT
{
	return pjstr_replace_len(s, sz, cap, start_idx, find, rplc, findlen, rplclen);
}

/*
  Replace last SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC_VOID
static int
jstr_replacelast_len(char *R *R const s,
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
	return jstr_replaceat_len_may_lower(s, sz, cap, p - *s, rplc, rplclen, findlen) ? 1 : 0;
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
	while (((flag & P_JSTR_FLAG_USE_N) ? n-- : 1)
	       && (p = (unsigned char *)JSTR_MEMMEM((char *)p, end - p, find, findlen)))
		P_JSTR_RMALL_IN_PLACE(dst, old, p, findlen);
	if (jstr_unlikely(dst == (unsigned char *)s))
		return s + sz;
	memmove(dst, old, end - old + 1);
	return (char *)(dst + (end - old));
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
	return pjstr_rmall_len_p(P_JSTR_FLAG_USE_N, s, find, n, sz, findlen);
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
	return pjstr_rmall_len_p(P_JSTR_FLAG_USE_NOT_N, s, find, 0, sz, findlen);
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
pjstr_replaceall_len(const pjstr_flag_use_n_ty flag,
		     char *R *R const s,
		     size_t *R const sz,
		     size_t *R const cap,
		     const size_t start_idx,
		     const char *R const find,
		     const char *R const rplc,
		     size_t n,
		     const size_t findlen,
		     const size_t rplclen) JSTR_NOEXCEPT
{
	typedef unsigned char u;
	unsigned char *dst = *(u **)s + start_idx;
	if (jstr_unlikely(rplclen == 0)) {
		*sz = pjstr_rmall_len_p(flag, (char *)dst, find, n, *sz - start_idx, findlen) - *s;
		return 1;
	}
	if (jstr_unlikely(findlen == 1)) {
		if (jstr_unlikely(rplclen == 1)) {
			jstr_replacechr_len((char *)dst, *find, *rplc, *sz - start_idx);
			return 1;
		}
	} else if (jstr_unlikely(findlen == 0))
		return 1;
	const unsigned char *p = dst;
	const unsigned char *old = p;
	while (((flag & P_JSTR_FLAG_USE_N) ? n-- : 1)
	       && (p = (u *)JSTR_MEMMEM((char *)p, (*(u **)s + *sz) - p, find, findlen))) {
		if (rplclen <= findlen)
			P_JSTR_RPLCALL_IN_PLACE(dst, old, p, rplc, rplclen, findlen);
		else
			p = (u *)jstr_replaceat_len(s, sz, cap, p - *(u **)s, rplc, rplclen, findlen);
		if (jstr_unlikely(p == NULL))
			return 0;
	}
	if (jstr_unlikely(dst == (u *)s))
		return 1;
	if (rplclen < findlen) {
		memmove(dst, old, *(u **)s + *sz - old + 1);
		*sz = (char *)(dst + (*(u **)s + *sz - old)) - *s;
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
jstr_replacen_len(char *R *R const s,
		  size_t *R const sz,
		  size_t *R const cap,
		  const char *R const find,
		  const char *R const rplc,
		  size_t n,
		  const size_t findlen,
		  const size_t rplclen) JSTR_NOEXCEPT
{
	return pjstr_replaceall_len(P_JSTR_FLAG_USE_N, s, sz, cap, 0, find, rplc, n, findlen, rplclen);
}

/*
  Replace N SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_replacen_len_from_unsafe(char *R *R const s,
			      size_t *R const sz,
			      size_t *R const cap,
			      const size_t start_idx,
			      const char *R const find,
			      const char *R const rplc,
			      size_t n,
			      const size_t findlen,
			      const size_t rplclen) JSTR_NOEXCEPT
{
	return pjstr_replaceall_len(P_JSTR_FLAG_USE_N, s, sz, cap, start_idx, find, rplc, n, findlen, rplclen);
}

/*
  Replace all SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_replaceall_len(char *R *R const s,
		    size_t *R const sz,
		    size_t *R const cap,
		    const char *R const find,
		    const char *R const rplc,
		    const size_t findlen,
		    const size_t rplclen) JSTR_NOEXCEPT
{
	return pjstr_replaceall_len(P_JSTR_FLAG_USE_NOT_N, s, sz, cap, 0, find, rplc, 0, findlen, rplclen);
}

/*
  Replace all SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_replaceall_len_from_unsafe(char *R *R const s,
				size_t *R const sz,
				size_t *R const cap,
				const size_t start_idx,
				const char *R const find,
				const char *R const rplc,
				const size_t findlen,
				const size_t rplclen) JSTR_NOEXCEPT
{
	return pjstr_replaceall_len(P_JSTR_FLAG_USE_NOT_N, s, sz, cap, start_idx, find, rplc, 0, findlen, rplclen);
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
   Return ptr to first non-ctype character.
*/
JSTR_INLINE
JSTR_PURE
static char *
jstr_skip_ctype(const char *R s,
		const jstr_ctype_ty ctype)
{
	if (jstr_unlikely(*s != '\0'))
		return (char *)s;
	while (jstr_isctype(*s, ctype))
		;
	return (char *)s - 1;
}

/*
   Return ptr to first non-ctype character from END to begin.
*/
JSTR_INLINE
JSTR_PURE
static char *
jstr_skip_ctype_rev(const char *begin,
		    const char *end,
		    const jstr_ctype_ty ctype)
{
	while (end != begin && jstr_isctype(*end, ctype))
		--end;
	return (char *)end;
}

/*
   Return ptr to first non-space character.
*/
JSTR_INLINE
JSTR_PURE
static char *
jstr_skip_spaces(const char *R s)
{
	return jstr_skip_ctype(s, JSTR_ISSPACE);
}

/*
   Return ptr to first non-space character from END to BEGIN.
*/
JSTR_INLINE
JSTR_PURE
static char *
jstr_skip_spaces_rev(const char *begin,
		     const char *end)
{
	return jstr_skip_ctype_rev(begin, end, JSTR_ISSPACE);
}

/*
   Return ptr to first non-blank character.
*/
JSTR_INLINE
JSTR_PURE
static char *
jstr_skip_blanks(const char *R s)
{
	while (jstr_isblank(*s++))
		;
	return (char *)s - 1;
}

/*
   Return ptr to first non-blank character from END to BEGIN.
*/
JSTR_INLINE
JSTR_PURE
static char *
jstr_skip_blanks_rev(const char *begin,
		     const char *end)
{
	while (end != begin && jstr_isblank(*end))
		--end;
	return (char *)end;
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
	memmove(start + 1, s, end - (u *)s + 1);
	return (char *)(start + (end - (u *)s));
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
pjstr_insert_len_unsafe(char *R const s,
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
		P_JSTR_REALLOC(*s, *cap, at + srclen + 1, goto err);
		*sz = at + srclen;
		(*s)[*sz] = '\0';
	}
	pjstr_insert_len_unsafe(*s, at, src, srclen);
	return 1;
err:
	P_JSTR_NULLIFY_MEMBERS(*sz, *cap);
	return 0;
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
	const char *const p = JSTR_STRSTR_LEN(*s, *sz, find, findlen);
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

P_JSTR_END_DECLS

#undef R

#endif /* JSTR_H_REPLACE_DEF */
