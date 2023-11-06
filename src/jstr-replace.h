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

JSTR_FUNC_VOID
JSTR_INLINE
static void
pjstr_rmallinplace(unsigned char **dst,
		   const unsigned char **oldp,
		   const unsigned char **p,
		   const size_t find_len)
JSTR_NOEXCEPT
{
	if (jstr_likely(*dst != *oldp))
		memmove(*dst, *oldp, *p - *oldp);
	*dst += *p - *oldp;
	*oldp += *p - *oldp;
	*p += find_len;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
pjstr_rplcallinplace(unsigned char **dst,
		     const unsigned char **oldp,
		     const unsigned char **p,
		     const unsigned char *rplc,
		     const size_t rplc_len,
		     const size_t find_len)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	if (jstr_likely(find_len != rplc_len)
	    && jstr_likely(*dst != *oldp))
		memmove(*dst, *oldp, *p - *oldp);
	*dst += *p - *oldp;
	*oldp += (*p - *oldp) + find_len;
	*p += find_len;
	*dst = (u *)jstr_mempcpy(*dst, rplc, rplc_len);
}

/*
  Insert SRC into DST[AT].
  Return value:
  ptr to '\0' in S.
  Assume that S have enough space for SRC.
*/
JSTR_INLINE
JSTR_FUNC_VOID
static void
jstr_insert_unsafe(char *R s,
		   const size_t at,
		   const char *R src,
		   const size_t sz,
		   const size_t src_len)
JSTR_NOEXCEPT
{
	memmove(s + at + src_len,
		s + at,
		sz - at + 1);
	memcpy(s + at, src, src_len);
}

JSTR_INLINE
JSTR_FUNC_RET_NONNULL
static char *
pjstr_rplcat_len_unsafe(char *R s,
			size_t *R sz,
			const size_t at,
			const char *R rplc,
			const size_t rplc_len,
			const size_t find_len)
JSTR_NOEXCEPT
{
	memmove(s + at + rplc_len,
		s + at + find_len,
		*sz - (at + find_len) + 1);
	memcpy(s + at, rplc, rplc_len);
	*sz += rplc_len - find_len;
	return s + at + rplc_len;
}

/*
  Insert SRC into DST[AT].
  Return 0 on malloc error;
  otherwise 1.
*/
JSTR_FUNC
static int
jstr_insert_len(char *R *R s,
		size_t *R sz,
		size_t *R cap,
		const size_t at,
		const char *R src,
		const size_t src_len)
JSTR_NOEXCEPT
{
	PJSTR_RESERVE(s, sz, cap, *sz + src_len, return 0)
	jstr_insert_unsafe(*s, at, src, *sz, src_len);
	*sz += src_len;
	return 1;
}

JSTR_INLINE
JSTR_FUNC
static char *
pjstr_rplcat_len_higher(char *R *R s,
			size_t *R sz,
			size_t *R cap,
			const size_t at,
			const char *R rplc,
			const size_t rplc_len,
			const size_t find_len)
JSTR_NOEXCEPT
{
	PJSTR_RESERVE(s, sz, cap, *sz + rplc_len - find_len, return NULL)
	return pjstr_rplcat_len_unsafe(*s, sz, at, rplc, rplc_len, find_len);
}

/*
   Replace RPLC in S with FIND.
   Return value:
   ptr to RPLC in S + RPLCLEN;
   NULL on error.
*/
JSTR_FUNC
static char *
jstr_rplcat_len(char *R *R s,
		size_t *R sz,
		size_t *R cap,
		const size_t at,
		const char *R rplc,
		const size_t rplc_len,
		const size_t find_len)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(rplc_len == find_len)) {
		memcpy(*s + at, rplc, rplc_len);
		*sz += rplc_len - find_len;
		return *s + at + rplc_len;
	}
	return pjstr_rplcat_len_higher(s, sz, cap, at, rplc, rplc_len, find_len);
}

/*
  Insert SRC after C in DST.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_INLINE
JSTR_FUNC
static int
jstr_insertafterchr_len(char *R *R s,
			size_t *R sz,
			size_t *R cap,
			const int c,
			const char *R src,
			const size_t src_len)
JSTR_NOEXCEPT
{
	const char *const p = (char *)memchr(*s, c, *sz);
	if (p != NULL)
		return jstr_insert_len(s, sz, cap, p - *s + 1, src, src_len);
	return 1;
}

/*
  Insert SRC after all C in DST.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_insertafterallchr_len(char *R *R s,
			   size_t *R sz,
			   size_t *R cap,
			   const int c,
			   const char *R src,
			   const size_t src_len)
JSTR_NOEXCEPT
{
	size_t off = 0;
	const char *p;
	while ((p = (char *)memchr(*s + off, c, *sz - off))) {
		off = p - *s;
		if (jstr_unlikely(!jstr_insert_len(s, sz, cap, off, src, src_len)))
			return 0;
		off += src_len + 1;
	}
	return 1;
}

/*
  Insert SRC after end of NE in DST.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_insertafter_len(char *R *R s,
		     size_t *R sz,
		     size_t *R cap,
		     const char *R find,
		     const char *R src,
		     const size_t find_len,
		     const size_t src_len)
JSTR_NOEXCEPT
{
	if (find_len == 1)
		return jstr_insertafterchr_len(s, sz, cap, *find, src, src_len);
	if (jstr_unlikely(find_len == 0))
		return 1;
	const char *const p = jstr_strstr_len(*s, *sz, find, find_len);
	if (p != NULL)
		return jstr_insert_len(s, sz, cap, p - *s + find_len, src, src_len);
	return 1;
}

/*
  Insert SRC after all end of NE in DST.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_insertafterall_len(char *R *R s,
			size_t *R sz,
			size_t *R cap,
			const char *R find,
			const char *R src,
			const size_t find_len,
			const size_t src_len)
JSTR_NOEXCEPT
{
	if (find_len == 1)
		return jstr_insertafterallchr_len(s, sz, cap, *find, src, src_len);
	if (jstr_unlikely(find_len == 0))
		return 1;
	size_t off = 0;
	const char *p;
	while ((p = jstr_strstr_len(*s + off, *sz - off, find, find_len))) {
		off = p - *s;
		if (jstr_unlikely(!jstr_insert_len(s, sz, cap, p - *s + find_len, src, src_len)))
			return 0;
		off += find_len + src_len;
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
		 const size_t sz)
JSTR_NOEXCEPT
{
	const char *const start = s;
	s = (char *)memchr(s, c, sz);
	return s ? jstr_stpmove_len(s, s + 1, sz - (s - start)) : s + sz;
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
	     const int c)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	s = strchrnul(s, c);
	return *s ? jstr_stpmove_len(s, s + 1, strlen(s)) : s;
#else
	return jstr_rmchr_len_p(s, c, strlen(s));
#endif /* HAVE_STRCHRNUL */
}

/*
  Replace first group of REJECT in S with RPLC.
*/
JSTR_FUNC_VOID
static void
jstr_rplcspn(char *R s,
	     const char *R reject,
	     const int rplc)
JSTR_NOEXCEPT
{
	if (jstr_likely(*s)
	    && (*(s += strcspn(s, reject))))
		memset(s, rplc, strspn(s, reject));
}

/*
  Replace first group of REJECT in S with RPLC.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rplcspn_p(char *R s,
	       const char *R reject,
	       const int rplc)
JSTR_NOEXCEPT
{
	if (jstr_likely(*s)
	    && (*(s += strcspn(s, reject)))) {
		s = jstr_stpset_len(s, rplc, strspn(s, reject));
		return s + strlen(s);
	}
	return s;
}

/*
  Replace first group of REJECT in S with RPLC.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rplcspn_len_p(char *R s,
		   const char *R reject,
		   const size_t sz,
		   const int rplc)
JSTR_NOEXCEPT
{
	if (jstr_likely(*s)) {
		const char *end = s + sz;
		if (*(s += strcspn(s, reject))) {
			const char *const p = s + strspn(s, reject);
			end += (p - s);
			memset(s, rplc, p - s);
			return (char *)end;
		}
	}
	return s;
}

/*
  Replace all REJECT in S with RPLC.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rplcallspn_p(char *R s,
		  const char *R reject,
		  const int rplc)
JSTR_NOEXCEPT
{
	for (; *s && (*(s += strcspn(s, reject))); s = jstr_stpset_len(s, rplc, strspn(s, reject)))
		;
	return s;
}

/*
  Remove first group of REJECT in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmspn_p(char *R s,
	     const char *R reject)
JSTR_NOEXCEPT
{
	if (jstr_likely(*s)
	    && (*(s += strcspn(s, reject)))) {
		char *const p = s + strspn(s, reject);
		return jstr_stpmove_len(s, p, strlen(p));
	}
	return s;
}

/*
  Remove first group of REJECT in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmspn_len_p(char *R s,
		 const char *R reject,
		 const size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_likely(*s)) {
		const char *const end = s + sz;
		if (*(s += strcspn(s, reject))) {
			char *const p = s + strspn(s, reject);
			return jstr_stpmove_len(p, s, end - p);
		}
	}
	return s;
}

/*
  Remove all REJECT in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmallspn_p(char *R s,
		const char *R reject)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	unsigned char *dst = (u *)s;
	const unsigned char *oldp = dst;
	const unsigned char *p = dst;
	for (; *p && (*(p += strcspn((char *)p, reject))); pjstr_rmallinplace(&dst, &oldp, &p, strspn((char *)p, reject)))
		;
	if (jstr_likely(p != oldp))
		return jstr_stpmove_len(dst, oldp, p - oldp);
	return (char *)p;
}

JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
pjstr_rmallchr_len_p(const int use_n,
		     char *R s,
		     const int c,
		     size_t n,
		     const size_t sz)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	unsigned char *dst = (u *)s;
	const unsigned char *oldp = dst;
	const unsigned char *p = dst;
	const unsigned char *const end = dst + sz;
	while ((use_n & 1 ? n-- : 1)
	       && (p = (u *)memchr(p, c, end - p)))
		pjstr_rmallinplace(&dst, &oldp, &p, 1);
	return (dst != (u *)s) ? jstr_stpmove_len(dst, oldp, end - oldp) : s + sz;
}

/*
  Remove all C in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmallchr_len_p(char *R s,
		    const int c,
		    const size_t sz)
JSTR_NOEXCEPT
{
	return pjstr_rmallchr_len_p(0, s, c, 0, sz);
}

/*
  Remove all C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmallchr_p(char *R s,
		const int c)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	typedef unsigned char u;
	unsigned char *dst = (u *)s;
	const unsigned char *oldp = dst;
	const unsigned char *p = dst;
	while (*(p = (u *)strchrnul((char *)p, c)))
		pjstr_rmallinplace(&dst, &oldp, &p, 1);
	return (dst != (u *)s) ? jstr_stpmove_len(dst, oldp, p - oldp) : (char *)p;
#else
	return jstr_rmallchr_len_p(s, c, strlen(s));
#endif
}

/*
  Remove N C in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmnchr_len_p(char *R s,
		  const int c,
		  const size_t n,
		  const size_t sz)
JSTR_NOEXCEPT
{
	return pjstr_rmallchr_len_p(1, s, c, n, sz);
}

/*
  Remove N C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_INLINE
JSTR_FUNC_RET_NONNULL
static char *
jstr_rmnchr_p(char *R s,
	      const int c,
	      size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCHRNUL
	typedef unsigned char u;
	unsigned char *dst = (u *)s;
	const unsigned char *oldp = dst;
	const unsigned char *p = dst;
	while (n-- && *(p = (u *)strchrnul((char *)p, c)))
		pjstr_rmallinplace(&dst, &oldp, &p, 1);
	if (jstr_unlikely(dst == (u *)s))
		return s + n;
	return jstr_stpmove_len(dst, oldp, p - oldp);
#else
	return jstr_rmnchr_len_p(s, c, n, strlen(s));
#endif /* HAVE_STRCHRNUL */
}

/*
  Remove characters in REJECT in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_stripspn_p(char *R s,
		const char *R rjct)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	unsigned char *dst = (u *)s;
	const unsigned char *oldp = dst;
	const unsigned char *p = dst;
	while (*(p += strcspn((char *)p, rjct)))
		pjstr_rmallinplace(&dst, &oldp, &p, 1);
	if (jstr_likely(p != oldp))
		return jstr_stpmove_len(dst, oldp, p - oldp);
	return (char *)p;
}

/*
  Remove first HS in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstr_rmat_len_p(char *R s,
		const size_t at,
		const size_t sz,
		const size_t find_len)
JSTR_NOEXCEPT
{
	memmove(s + at, s + at + find_len, (s + sz) - (s + at));
	return s + sz - find_len;
}

/*
  Remove first HS in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_FUNC_RET_NONNULL
static char *
jstr_rm_len_p(char *R s,
	      const char *R find,
	      const size_t sz,
	      const size_t find_len)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(find_len == 0))
		return s + sz;
	char *const p = jstr_strstr_len(s, sz, find, find_len);
	if (jstr_unlikely(p == NULL))
		return s + sz;
	memmove(p, p + find_len, (s + sz) - p);
	return s + sz - find_len;
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
		 const size_t sz)
JSTR_NOEXCEPT
{
	s = (char *)memchr(s, find, sz);
	if (s != NULL)
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
	     const int rplc)
JSTR_NOEXCEPT
{
	s = strchr(s, find);
	if (s != NULL)
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
		    const size_t sz)
JSTR_NOEXCEPT
{
	const char *R end = s + sz;
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
		const int rplc)
JSTR_NOEXCEPT
{
	while ((s = strchr(s, find)))
		*s++ = rplc;
}

/*
  Replace N SEARCH in REPLACE.
*/
JSTR_FUNC_VOID
static void
jstr_rplcnchr_len(char *R s,
		  const int find,
		  const int rplc,
		  size_t n,
		  const size_t sz)
JSTR_NOEXCEPT
{
	const char *R end = s + sz;
	while (n-- && (s = (char *)memchr(s, find, end - s)))
		*s++ = rplc;
}

/*
  Replace N SEARCH in REPLACE.
*/
JSTR_FUNC_VOID
static void
jstr_rplcnchr(char *R s,
	      const int find,
	      const int rplc,
	      size_t n)
JSTR_NOEXCEPT
{
	while (n-- && (s = strchr(s, find)))
		*s++ = rplc;
}

JSTR_FUNC
static int
pjstr_rplc_len(char *R *R s,
	       size_t *R sz,
	       size_t *R cap,
	       const size_t start_idx,
	       const char *R find,
	       const char *R rplc,
	       const size_t find_len,
	       const size_t rplc_len)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(rplc_len == 0)) {
		*sz = jstr_rm_len_p(*s + start_idx, find, *sz - start_idx, find_len) - *s;
		return 1;
	}
	if (find_len == 1) {
		if (rplc_len == 1) {
			jstr_rplcchr_len(*s + start_idx, *find, *rplc, *sz - start_idx);
			return 1;
		}
	} else if (jstr_unlikely(find_len == 0))
		return 1;
	char *p = jstr_strstr_len(*s + start_idx, *sz - start_idx, find, find_len);
	if (p != NULL)
		return jstr_rplcat_len(s, sz, cap, p - *s, rplc, rplc_len, find_len) != NULL;
	return 1;
}

/*
  Replace first SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_rplc_len(char *R *R s,
	      size_t *R sz,
	      size_t *R cap,
	      const char *R find,
	      const char *R rplc,
	      const size_t find_len,
	      const size_t rplc_len)
JSTR_NOEXCEPT
{
	return pjstr_rplc_len(s, sz, cap, 0, find, rplc, find_len, rplc_len);
}

/*
  Replace first SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
static int
jstr_rplc_len_from(char *R *R s,
		   size_t *R sz,
		   size_t *R cap,
		   const size_t start_idx,
		   const char *R find,
		   const char *R rplc,
		   const size_t find_len,
		   const size_t rplc_len)
JSTR_NOEXCEPT
{
	return pjstr_rplc_len(s, sz, cap, start_idx, find, rplc, find_len, rplc_len);
}

/*
  Replace last SEARCH in S with REPLACE.
  Return ptr to '\0' in S.
*/
JSTR_FUNC
JSTR_INLINE
static char *
jstr_rmlast_len_p(char *R s,
		  size_t sz,
		  const char *R find,
		  const size_t find_len)
JSTR_NOEXCEPT
{
	const char *const p = (char *)jstr_strrstr_len(s, sz, find, find_len);
	return p ? jstr_rmat_len_p(s, p - s, sz, find_len) : s + sz;
}

/*
  Replace last SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_rplclast_len(char *R *R s,
		  size_t *R sz,
		  size_t *R cap,
		  const char *R find,
		  const char *R rplc,
		  const size_t find_len,
		  const size_t rplc_len)
JSTR_NOEXCEPT
{
	const char *const p = (char *)jstr_strrstr_len(*s, *sz, find, find_len);
	return p && jstr_rplcat_len(s, sz, cap, p - *s, rplc, rplc_len, find_len);
}

JSTR_FUNC_RET_NONNULL
static char *
jstr_rmn_len_p(char *R s,
	       size_t sz,
	       const char *R find,
	       const size_t find_len,
	       size_t n)
JSTR_NOEXCEPT
{
	if (find_len == 1)
		return pjstr_rmallchr_len_p(1, s, *find, n, sz);
	if (jstr_unlikely(find_len == 0))
		return s + sz;
	typedef unsigned char u;
	unsigned char *dst = (u *)s;
	const unsigned char *oldp = dst;
	const unsigned char *p = dst;
	const unsigned char *const end = dst + sz;
	while (n-- && (p = (u *)jstr_strstr_len(p, end - p, find, find_len)))
		pjstr_rmallinplace(&dst, &oldp, &p, find_len);
	if (jstr_unlikely(dst == (u *)s))
		return s + sz;
	return jstr_stpmove_len(dst, oldp, end - oldp);
}

/*
  Remove all SEARC in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstr_rmall_len_p(char *R s,
		 size_t sz,
		 const char *R find,
		 const size_t find_len)
JSTR_NOEXCEPT
{
	return jstr_rmn_len_p(s, sz, find, find_len, -1);
}

JSTR_INLINE
JSTR_FUNC
static int
jstr_rplcn_len_from(char *R *R s,
		    size_t *R sz,
		    size_t *R cap,
		    const size_t start_idx,
		    const char *R find,
		    const size_t find_len,
		    const char *R rplc,
		    const size_t rplc_len,
		    size_t n)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	unsigned char *dst = *(u **)s + start_idx;
	if (jstr_unlikely(rplc_len == 0)) {
		*sz = jstr_rmn_len_p((char *)dst, *sz - start_idx, find, find_len, n) - *s;
		return 1;
	}
	if (find_len == 1) {
		if (rplc_len == 1) {
			jstr_rplcchr_len((char *)dst, *find, *rplc, *sz - start_idx);
			return 1;
		}
	} else if (jstr_unlikely(find_len == 0))
		return 1;
	const unsigned char *p = dst;
	const unsigned char *oldp = p;
	while (n-- && (p = (u *)jstr_strstr_len(p, (*s + *sz) - (char *)p, find, find_len))) {
		if (rplc_len <= find_len)
			pjstr_rplcallinplace(&dst, &oldp, &p, (u *)rplc, rplc_len, find_len);
		else
			p = (u *)pjstr_rplcat_len_higher(s, sz, cap, p - *(u **)s, rplc, rplc_len, find_len);
		if (jstr_unlikely(p == NULL))
			return 0;
	}
	if (jstr_unlikely(dst == *(u **)s))
		return 1;
	if (rplc_len < find_len)
		*sz = jstr_stpmove_len(dst, oldp, *(u **)s + *sz - oldp) - *s;
	return 1;
}

/*
  Replace N SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_rplcn_len(char *R *R s,
	       size_t *R sz,
	       size_t *R cap,
	       const char *R find,
	       const char *R rplc,
	       size_t n,
	       const size_t find_len,
	       const size_t rplc_len)
JSTR_NOEXCEPT
{
	return jstr_rplcn_len_from(s, sz, cap, 0, find, find_len, rplc, rplc_len, n);
}

/*
  Replace all SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_rplcall_len_from(char *R *R s,
		      size_t *R sz,
		      size_t *R cap,
		      const size_t start_idx,
		      const char *R find,
		      const char *R rplc,
		      const size_t find_len,
		      const size_t rplc_len)
JSTR_NOEXCEPT
{
	return jstr_rplcn_len_from(s, sz, cap, start_idx, find, find_len, rplc, rplc_len, -1);
}

/*
  Replace N SEARCH in S with REPLACE.
  Return 0 on malloc error;
  otherwise, 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_rplcall_len(char *R *R s,
		 size_t *R sz,
		 size_t *R cap,
		 const char *R find,
		 const char *R rplc,
		 const size_t find_len,
		 const size_t rplc_len)
JSTR_NOEXCEPT
{
	return jstr_rplcn_len_from(s, sz, cap, 0, find, find_len, rplc, rplc_len, -1);
}

/*
  Reverse S.
*/
JSTR_FUNC_VOID
static void
jstr_rev_len(char *R s,
	     const size_t sz)
JSTR_NOEXCEPT
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
jstr_rev(char *R s)
JSTR_NOEXCEPT
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
jstr_rev_p(char *R s)
JSTR_NOEXCEPT
{
	const size_t _len = strlen(s);
	jstr_rev_len(s, _len);
	return s + _len;
}

/*
  Trim leading and trailing jstr_isspace() chars in S.
  Return value:
  ptr to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_trimend_len_p(char *R s,
		   const size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return s;
	char *end = jstr_skip_space_rev(s, s + sz - 1);
	*++end = '\0';
	return end;
}

/*
  Trim leading and trailing jstr_isspace() chars in S.
  Return value:
  ptr to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstr_trimend_p(char *R s)
JSTR_NOEXCEPT
{
	return jstr_trimend_len_p(s, strlen(s));
}

/*
  Trim leading and trailing jstr_isspace() chars in S.
  Return value:
  ptr to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_trimstart_len_p(char *R s,
		     const size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return s;
	const char *const start = jstr_skip_space(s);
	if (jstr_likely(s != start))
		return jstr_stpmove_len(s, start, (s + sz) - start);
	return s + sz;
}

/*
  Trim leading jstr_isspace() chars in S.
  Return value:
  ptr to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstr_trimstart_p(char *R s)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return s;
	const char *const start = jstr_skip_space(s);
	if (jstr_likely(s != start))
		return jstr_stpmove_len(s, start, strlen(start));
	return s + strlen(start);
}

/*
  Trim leading jstr_isspace() chars in S.
*/
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_trimstart(char *R s)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return;
	const char *const start = jstr_skip_space(s);
	if (jstr_likely(s != start))
		jstr_strmove_len(s, start, strlen(start));
}

/*
  Trim leading and trailing jstr_isspace() chars in S.
  Return value:
  ptr to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_trim_len_p(char *R s,
		const size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return s;
	const char *const end = jstr_skip_space_rev(s, s + sz - 1) + 1;
	const char *const start = jstr_skip_space(s);
	if (jstr_likely(start != s))
		return jstr_stpmove_len(s, start, end - start);
	return s + sz;
}

/*
  Trim leading and trailing jstr_isspace() chars in S.
  Return value:
  ptr to '\0' in S;
*/
JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstr_trim_p(char *R s)
JSTR_NOEXCEPT
{
	return jstr_trim_len_p(s, strlen(s));
}

/*
  Put SRC into DST[AT].
  Assume that S have enough space for SRC.
  Return value:
*/
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_put_len_unsafe(char *R s,
		    const size_t at,
		    const char *R src,
		    const size_t src_len)
JSTR_NOEXCEPT
{
	memcpy(s + at, src, src_len);
}

/*
  Put SRC into DST[AT].
  Return 0 on malloc error;
  otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_put_len(char *R *R s,
	     size_t *R sz,
	     size_t *R cap,
	     const size_t at,
	     const char *R src,
	     const size_t src_len)
JSTR_NOEXCEPT
{
	if (at + src_len > *sz) {
		PJSTR_RESERVE_ALWAYS(s, sz, cap, at + src_len, return 0)
		*sz = at + src_len;
		*(*s + *sz) = '\0';
	}
	jstr_put_len_unsafe(*s, at, src, src_len);
	return 1;
}

/*
  Put SRC after C in DST.
  Return 0 on malloc error;
  otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_putafterchr_len(char *R *R s,
		     size_t *R sz,
		     size_t *R cap,
		     const int c,
		     const char *R src,
		     const size_t src_len)
JSTR_NOEXCEPT
{
	const char *const p = (char *)memchr(*s, c, *sz);
	if (p != NULL)
		return jstr_put_len(s, sz, cap, p - *s + 1, src, src_len);
	return 1;
}

/*
  Put SRC after end of NE in DST.
  Return 0 on malloc error;
  otherwise 1.
*/
JSTR_FUNC
static int
jstr_putafter_len(char *R *R s,
		  size_t *R sz,
		  size_t *R cap,
		  const char *R find,
		  const char *R src,
		  const size_t find_len,
		  const size_t src_len)
JSTR_NOEXCEPT
{
	if (find_len == 1)
		return jstr_putafterchr_len(s, sz, cap, *find, src, src_len);
	if (jstr_unlikely(find_len == 0))
		return 1;
	const char *const p = jstr_strstr_len(*s, *sz, find, find_len);
	if (p != NULL)
		return jstr_put_len(s, sz, cap, p - *s + find_len, src, src_len);
	return 1;
}

/*
   Convert snake_case to camelCase.
   Return ptr to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_toCamelCaseP(char *R s)
JSTR_NOEXCEPT
{

#if JSTR_HAVE_STRCHRNUL
	s = strchrnul(s, '_');
#else
	for (; *s && *s != '_'; ++s)
		;
#endif
	if (jstr_unlikely(*s == '\0'))
		return s;
	const char *src = s;
	goto start;
	for (; *src; ++src)
		if (jstr_likely(*src != '_'))
			*s++ = *src;
		else {
start:
			*s++ = jstr_toupper(*++src);
			if (jstr_unlikely(*src == '\0'))
				break;
		}
	*s = '\0';
	return s;
}

/*
   Convert camelCase to snake_case.
   Return ptr to '\0' in S.
*/
JSTR_FUNC_RET_NONNULL
static char *
jstr_to_snake_case_p(char *R s)
JSTR_NOEXCEPT
{
	for (*s = jstr_tolower(*s); *s && !jstr_isupper(*s); ++s)
		;
	if (jstr_unlikely(*s == '\0'))
		return s;
	const char *end = s + strlen(s);
	goto start;
	for (; *s; ++s)
		if (jstr_isupper(*s)) {
start:
			jstr_strmove_len(s + 1, s, end++ - s);
			*s++ = '_';
			*s = jstr_tolower(*s);
		}
	*s = '\0';
	return s;
}

PJSTR_END_DECLS

#undef R

#endif /* JSTR_H_REPLACE_DEF */
