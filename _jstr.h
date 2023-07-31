#ifndef JSTR_H_DEF
#define JSTR_H_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "_jstr_builder.h"
#include "_jstr_ctype.h"
#include "_jstr_macros.h"
#include "_jstr_replace.h"
#include "_jstr_string.h"

#define JSTR_EXTERN_C  1
#define JSTR_NAMESPACE 0

#if JSTR_NAMESPACE && defined(__cplusplus)
namespace jstr {
#endif /* JSTR_NAMESPACE */
#if JSTR_EXTERN_C && defined(__cplusplus)
extern "C" {
#endif /* JSTR_EXTERN_C */

/*
   Append SRC to DST.
   Use non-f version for bounds checking.
   Return value:
   New len of S.
*/
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_INLINE
static char *jstr_append_mem_p_f(char *JSTR_RST const s,
				 const char *JSTR_RST const src,
				 const size_t sz,
				 const size_t srclen) JSTR_NOEXCEPT
{
	memcpy(s, src, srclen + 1);
	return s + sz + srclen;
}

/*
   Append SRC to DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_append_mem(char **JSTR_RST const s,
			    size_t *JSTR_RST const sz,
			    size_t *JSTR_RST const cap,
			    const char *JSTR_RST const src,
			    const size_t srclen) JSTR_NOEXCEPT
{
	if (*cap < *sz + srclen)
		JSTR_REALLOC(*s, *cap, *sz + srclen, return);
	*sz = jstr_append_mem_p_f(*s, src, *sz, srclen) - *s;
}

/*
  Count occurences of C in S.
  Return value:
  Occurences of C in S.
*/
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_countc(const char *JSTR_RST s,
		       const int c) JSTR_NOEXCEPT
{
	int count = 0;
	for (;; ++s)
		if (*s == c)
			++count;
		else if (unlikely(!*s))
			break;
	return count;
}

#ifdef JSTR_HAS_MEMMEM

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_count_mem(const char *JSTR_RST s,
			  const char *JSTR_RST const searc,
			  size_t sz,
			  const size_t searclen) JSTR_NOEXCEPT
{
	int count = 0;
	while ((s = (char *)memmem(s, sz, searc, searclen))) {
		++count;
		s += searclen;
		sz += searclen;
	}
	return count;
}

#else

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_count(const char *JSTR_RST s,
		      const char *JSTR_RST const searc) JSTR_NOEXCEPT
{
	int count = 0;
	while ((s = strstr(s, searc)))
		++count;
	return count;
}

#endif /* __JSTR_HAS_MEMMEM */

/*
  Reverse S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_rev_mem(char *JSTR_RST s,
			 const size_t sz) JSTR_NOEXCEPT
{
	if (unlikely(!*s))
		return;
	char *end = s + sz - 1;
	char tmp;
	do {
		tmp = *s;
		*s = *end;
		*end = tmp;
	} while (++s < --end);
}

/*
  Trim spaces in [ \t] from end of S.
  Return value:
  Pointer to '\0' in S;
  S if SLEN is 0.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_trim_mem_p(char *JSTR_RST const s,
			     const size_t sz) JSTR_NOEXCEPT
{
	if (unlikely(!*s))
		return s;
	char *end = s + sz - 1;
	do {
		switch (*end--) {
		case '\t':
		case ' ':
			continue;
		default:
			end += 2;
			*end = '\0';
		}
		break;
	} while (end >= s);
	return end;
}

/*
  Insert SRC into DST[AT].
  Assumes that S have enough space for SRC.
  Return value:
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insert_mem_f(char *JSTR_RST const s,
			      const size_t at,
			      const char *JSTR_RST const src,
			      const size_t srclen) JSTR_NOEXCEPT
{
	memcpy(s + at, src, srclen);
}

/*
  Insert SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insert_mem(char **JSTR_RST const s,
			    size_t *JSTR_RST const sz,
			    size_t *JSTR_RST const cap,
			    const size_t at,
			    const char *JSTR_RST const src,
			    const size_t srclen) JSTR_NOEXCEPT
{
	if (at + srclen > *sz) {
		JSTR_REALLOC(*s, *cap, at + srclen + 1, return);
		*sz = at + srclen;
		*(*s + *sz) = '\0';
	}
	jstr_insert_mem_f(*s, at, src, srclen);
}

/*
  Insert SRC after C in DST.
  Assumes that S have enough space for S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaftc_mem_f(char *JSTR_RST const s,
				  const int c,
				  const char *JSTR_RST const src,
				  const size_t sz,
				  const size_t srclen) JSTR_NOEXCEPT
{
	if (unlikely(srclen == 0))
		return;
	const char *const p = (char *)memchr(s, c, sz);
	if (p)
		jstr_insert_mem_f(s, p - s + 1, src, srclen);
}

/*
  Insert SRC after C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaftc_mem(char **JSTR_RST const s,
				size_t *JSTR_RST const sz,
				size_t *JSTR_RST const cap,
				const int c,
				const char *JSTR_RST const src,
				const size_t srclen) JSTR_NOEXCEPT
{
	if (unlikely(srclen == 0))
		return;
	const char *const p = (char *)memchr(*s, c, *sz);
	if (p)
		jstr_insert_mem(s, sz, cap, p - *s + 1, src, srclen);
}

/*
  Insert SRC after all C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaftallc_mem(char **JSTR_RST const s,
				   size_t *JSTR_RST const sz,
				   size_t *JSTR_RST const cap,
				   const int c,
				   const char *JSTR_RST const src,
				   const size_t srclen) JSTR_NOEXCEPT
{
	if (unlikely(srclen == 0))
		return;
	size_t off = 0;
	char *p;
	while ((p = (char *)memchr(*s + off, c, *sz - off))) {
		jstr_insert_mem(s, sz, cap, p - *s + 1, src, srclen);
		off += *s - p + 1;
	}
}

/*
  Insert SRC after end of NE in DST.
  Assumes that S have enough space for SRC.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaft_mem_f(char *JSTR_RST const s,
				 const char *JSTR_RST const searc,
				 const char *JSTR_RST const src,
				 const size_t sz,
				 const size_t searclen,
				 const size_t srclen) JSTR_NOEXCEPT
{
	switch (searclen) {
	case 0:
		break;
	case 1: {
		jstr_insertaftc_mem_f(s, *searc, src, sz, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return;
		const char *const p = (char *)JSTR_MEMMEM(s, sz, searc, searclen);
		if (p)
			jstr_insert_mem_f(s, p - s + searclen, src, srclen);
		break;
	}
	}
}

/*
  Insert SRC after end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaft_mem(char **JSTR_RST const s,
			       size_t *JSTR_RST const sz,
			       size_t *JSTR_RST const cap,
			       const char *JSTR_RST const searc,
			       const char *JSTR_RST const src,
			       const size_t searclen,
			       const size_t srclen) JSTR_NOEXCEPT
{
	switch (searclen) {
	case 0:
		break;
	case 1: {
		jstr_insertaftc_mem(s, sz, cap, *searc, src, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return;
		const char *const p = (char *)JSTR_MEMMEM(*s, *sz, searc, searclen);
		if (p)
			jstr_insert_mem(s, sz, cap, p - *s + searclen, src, srclen);
		break;
	}
	}
}

/*
  Insert SRC after all end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaftall_mem(char **JSTR_RST const s,
				  size_t *JSTR_RST const sz,
				  size_t *JSTR_RST const cap,
				  const char *JSTR_RST const searc,
				  const char *JSTR_RST const src,
				  const size_t searclen,
				  const size_t srclen) JSTR_NOEXCEPT
{
	switch (searclen) {
	case 0:
		break;
	case 1: {
		jstr_insertaftallc_mem(s, sz, cap, *searc, src, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return;
		size_t off = 0;
		char *p;
		while ((p = (char *)JSTR_MEMMEM(*s + off, *sz - off, searc, searclen))) {
			jstr_insert_mem(s, sz, cap, p - *s + searclen, src, srclen);
			off += *s - p + searclen;
		}
		break;
	}
	}
}

/*
  Slip SRC into DST[AT].
  Return value:
  Pointer to '\0' in S.
  Assumes that S have enough space for SRC.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_slip_mem_p_f(char *JSTR_RST const s,
			       const size_t at,
			       const char *JSTR_RST const src,
			       const size_t sz,
			       const size_t srclen) JSTR_NOEXCEPT
{
	memmove(s + at + srclen,
		s + at,
		sz - at + 1);
	memcpy(s + at, src, srclen);
	return s + sz + srclen;
}

/*
  Slip SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slip_mem(char **JSTR_RST const s,
			  size_t *JSTR_RST const sz,
			  size_t *JSTR_RST const cap,
			  const size_t at,
			  const char *JSTR_RST const src,
			  const size_t srclen) JSTR_NOEXCEPT
{
	if (*cap > *sz + srclen + 1) {
		memmove(*s + at + srclen,
			*s + at,
			*sz - at + 1);
		memcpy(*s + at, src, srclen);
	} else {
		JSTR_GROW(*cap, *sz + srclen + 1);
		char *const tmp = (char *)malloc(*cap);
		JSTR_MALLOC_ERR(tmp, return);
		memcpy(tmp, *s, at);
		memcpy(tmp + at, src, srclen);
		memcpy(tmp + at + srclen,
		       *s + at,
		       *sz - at + 1);
		free(*s);
		*s = tmp;
	}
	*sz += srclen;
}

/*
  Slip SRC after C in DST.
  Return value:
  Pointer to '\0' in S.
  Assumes that S have enough space for SRC.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_slipaftc_mem_p_f(char *JSTR_RST const s,
				   const int c,
				   const char *JSTR_RST const src,
				   const size_t sz,
				   const size_t srclen) JSTR_NOEXCEPT
{
	if (unlikely(srclen == 0))
		return s + sz;
	const char *const p = (char *)memchr(s, c, sz);
	if (p)
		return jstr_slip_mem_p_f(s, p - s, src, sz - (p - s), srclen);
	return s + sz;
}

/*
  Slip SRC after C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slipaftc_mem(char **JSTR_RST const s,
			      size_t *JSTR_RST const sz,
			      size_t *JSTR_RST const cap,
			      const int c,
			      const char *JSTR_RST const src,
			      const size_t srclen) JSTR_NOEXCEPT
{
	if (unlikely(srclen == 0))
		return;
	const char *const p = (char *)memchr(*s, c, *sz);
	if (p)
		jstr_slip_mem(s, sz, cap, p - *s + 1, src, srclen);
}

/*
  Slip SRC after all C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static char *jstr_slipaftallc_mem_p_f(char *JSTR_RST const s,
				      const int c,
				      const char *JSTR_RST const src,
				      size_t sz,
				      const size_t srclen) JSTR_NOEXCEPT
{
	if (unlikely(srclen == 0))
		return s + sz;
	size_t off = 0;
	char *p;
	while ((p = (char *)memchr(s + off, c, sz - off))) {
		sz = jstr_slip_mem_p_f(s, p - s, src, sz, srclen) - s;
		off += p - s + 1;
	}
	return s + sz;
}

/*
  Slip SRC after all C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slipaftallc_mem(char **JSTR_RST const s,
				 size_t *JSTR_RST const sz,
				 size_t *JSTR_RST const cap,
				 const int c,
				 const char *JSTR_RST const src,
				 const size_t srclen) JSTR_NOEXCEPT
{
	if (unlikely(srclen == 0))
		return;
	size_t off = 0;
	char *p;
	while ((p = (char *)memchr(*s + off, c, *sz - off))) {
		jstr_slip_mem(s, sz, cap, p - *s + 1, src, srclen);
		off += p - *s + 1;
	}
}

/*
  Slip SRC after end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_slipaft_mem_f(char *JSTR_RST const s,
				const char *JSTR_RST const searc,
				const char *JSTR_RST const src,
				const size_t sz,
				const size_t searclen,
				const size_t srclen) JSTR_NOEXCEPT
{
	switch (searclen) {
	case 0:
		return s + sz;
		break;
	case 1: {
		return jstr_slipaftc_mem_p_f(s, *searc, src, sz, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return s + sz;
		const char *const p = (char *)JSTR_MEMMEM(s, sz, searc, searclen);
		if (p)
			return jstr_slip_mem_p_f(s, p - s, src, sz, srclen);
		return s + sz;
	}
	}
}

/*
  Slip SRC after end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slipaft_mem(char **JSTR_RST const s,
			     size_t *JSTR_RST const sz,
			     size_t *JSTR_RST const cap,
			     const char *JSTR_RST const searc,
			     const char *JSTR_RST const src,
			     const size_t searclen,
			     const size_t srclen) JSTR_NOEXCEPT
{
	switch (searclen) {
	case 0:
		break;
	case 1: {
		jstr_slipaftc_mem(s, sz, cap, *searc, src, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return;
		const char *const p = (char *)JSTR_MEMMEM(*s, *sz, searc, searclen);
		if (p)
			jstr_slip_mem(s, sz, cap, p - *s + searclen, src, srclen);
		break;
	}
	}
}

/*
  Slip SRC after all end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static char *jstr_slipaftall_mem_p_f(char *JSTR_RST const s,
				    const char *JSTR_RST const searc,
				    const char *JSTR_RST const src,
				    size_t sz,
				    const size_t searclen,
				    const size_t srclen) JSTR_NOEXCEPT
{
	switch (searclen) {
	case 0:
		return s + sz;
		break;
	case 1: {
		return jstr_slipaftallc_mem_p_f(s, *searc, src, sz, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return s + sz;
		size_t off = 0;
		char *p;
		while ((p = (char *)JSTR_MEMMEM(s + off, sz - off, searc, searclen))) {
			sz = jstr_slip_mem_p_f(s, p - s, src, sz, srclen) - s;
			off += p - s + searclen;
		}
		return s + sz;
	}
	}
}

/*
  Slip SRC after all end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slipaftall_mem(char **JSTR_RST const s,
				size_t *JSTR_RST const sz,
				size_t *JSTR_RST const cap,
				const char *JSTR_RST const searc,
				const char *JSTR_RST const src,
				const size_t searclen,
				const size_t srclen) JSTR_NOEXCEPT
{
	switch (searclen) {
	case 0:
		break;
	case 1: {
		jstr_slipaftallc_mem(s, sz, cap, *searc, src, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return;
		size_t off = 0;
		char *p;
		while ((p = (char *)JSTR_MEMMEM(*s + off, *sz - off, searc, searclen))) {
			jstr_slip_mem(s, sz, cap, p - *s + searclen, src, srclen);
			off += p - *s + searclen;
		}
	}
	}
}

/*
  Checks if S2 is in end of S1.
  Return value:
  0 if true;
  1 if false.
  Assumes that HS is longer than NE.
  Let memcmp do the bounds check.
*/
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_endswith_mem_f(const char *JSTR_RST const hs,
			       const char *JSTR_RST const ne,
			       const size_t hsz,
			       const size_t nelen) JSTR_NOEXCEPT
{
	return memcmp(hs + hsz - nelen, ne, nelen);
}

/*
  Checks if S2 is in end of S1.
  Return value:
  0 if true;
  1 if false.
*/
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_endswith_mem(const char *JSTR_RST const hs,
			     const char *JSTR_RST const ne,
			     const size_t hsz,
			     const size_t nelen) JSTR_NOEXCEPT
{
	return (hsz < nelen) ? 1 : memcmp(hs + hsz - nelen, ne, nelen);
}

#if JSTR_EXTERN_C && defined(__cplusplus)
}
#endif /* JSTR_EXTERN_C */
#if JSTR_NAMESPACE && defined(__cplusplus)
}
#endif /* JSTR_NAMESPACE */

#undef JSTR_RST
#undef JSTR_REPLACE
#undef JSTR_MALLOC_ERR
#undef JSTR_GROW
#undef JSTR_REALLOC

#endif /* JSTR_H_DEF */
