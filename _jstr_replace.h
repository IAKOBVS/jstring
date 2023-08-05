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

#include "_jstr_macros.h"
#include "_jstr_private_replace_memmem.h"

#define JSTR_EXTERN_C  1
#define JSTR_NAMESPACE 0
#define JSTR_INLINE_IF_CONSTEXPR

#if JSTR_NAMESPACE && defined(__cplusplus)
namespace jstr {
#endif /* JSTR_NAMESPACE */
#if JSTR_EXTERN_C && defined(__cplusplus)
extern "C" {
#endif /* JSTR_EXTERN_C */

#define JSTR_ASCII_SIZE 256

/* Use _constexpr functions when passing arguments known at compile time. */

/* TODO: Fix jstr_memmem3 and 4. */

/*
  Remove first C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *jstr_rmc_mem_p(char *JSTR_RST s,
			    const int c,
			    const size_t sz) JSTR_NOEXCEPT
{
	const char *const sstart = s;
	s = (char *)memchr(s, c, sz);
	if (unlikely(!s))
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
static char *jstr_rmc_p(char *JSTR_RST s,
			const int c) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_STRCHRNUL
	s = strchrnul(s, c);
	if (unlikely(!*s))
		return s;
	const char *src = s;
	while ((*s++ = *++src))
		;
	return s - 1;
#else
	return jstr_rmc_mem_p(s, c, strlen(s));
#endif /* HAS_STRCHRNUL */
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
static char *jstr_rmallc_mem_p(char *JSTR_RST s,
			       const int c,
			       const size_t sz) JSTR_NOEXCEPT
{
	s = (char *)memchr(s, c, sz);
	if (unlikely(!s))
		return s + sz;
	const char *src = s + 1;
	for (;; ++src)
		if (*src != c) {
			if (unlikely(!*src))
				break;
			*s++ = *src;
		}
	*s = '\0';
	return s;
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
static char *jstr_rmallc_p(char *JSTR_RST s,
			   const int c) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_STRCHRNUL
	s = strchrnul(s, c);
	if (unlikely(!*s))
		return s;
	const char *src = s + 1;
	for (;; ++src)
		if (*src != c) {
			if (unlikely(!*src))
				break;
			*s++ = *src;
		}
	*s = '\0';
	return s;
#endif /* HAS_STRCHRNUL */
	return jstr_rmallc_mem_p(s, c, strlen(s));
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
static char *jstr_rmnc_mem_p(char *JSTR_RST s,
			     const int c,
			     size_t n,
			     const size_t sz) JSTR_NOEXCEPT
{
	s = (char *)memchr(s, c, sz);
	if (unlikely(!s))
		return s;
	const char *const end = s + sz;
	const char *src = s + 1;
	for (;; ++src)
		if (*src != c) {
			if (unlikely(!*src))
				break;
			*s++ = *src;
		} else {
			if (unlikely(!--n)) {
				memmove(s, src, end - src + 1);
				break;
			}
		}
	*s = '\0';
	return s;
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
static char *jstr_rmnc_p(char *JSTR_RST s,
			 const int c,
			 size_t n) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_STRCHRNUL
	s = (char *)strchrnul(s, c);
	if (unlikely(!*s))
		return s;
	const char *src = s + 1;
	for (;; ++src)
		if (*src != c) {
			if (unlikely(!*src))
				break;
			*s++ = *src;
		} else {
			if (unlikely(!--n)) {
				do
					*s++ = *src++;
				while (*src);
				break;
			}
		}
	*s = '\0';
	return s;
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
static char *jstr_stripspn_mem_p(char *JSTR_RST s,
				 const char *JSTR_RST reject,
				 const size_t sz) JSTR_NOEXCEPT
{
	enum {
		ACCEPT = 0,
		REJECT,
		NUL,
	};
	if (unlikely(!reject[0]))
		return s + sz;
	if (unlikely(!reject[1]))
		return jstr_rmallc_mem_p(s, *reject, sz);
	unsigned char tbl[JSTR_ASCII_SIZE];
	memset(tbl, ACCEPT, 64);
	memset(&tbl[64], ACCEPT, 64);
	memset(&tbl[128], ACCEPT, 64);
	memset(&tbl[192], ACCEPT, 64);
	tbl[0] = NUL;
	do
		tbl[(unsigned char)*reject++] = REJECT;
	while (*reject);
	for (const unsigned char *src = (unsigned char *)s;; ++src) {
		switch (tbl[*src]) {
		case ACCEPT:
			*s++ = *src;
		case REJECT:
			continue;
		case NUL:;
		}
		break;
	}
	*s = '\0';
	return s;
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
static char *jstr_stripspn_p(char *JSTR_RST s,
			     const char *JSTR_RST reject) JSTR_NOEXCEPT
{
	enum {
		ACCEPT = 0,
		REJECT,
		NUL,
	};
	if (unlikely(!reject[0]))
		return s + strlen(s);
	if (unlikely(!reject[1]))
		return jstr_rmallc_p(s, *reject);
	unsigned char tbl[JSTR_ASCII_SIZE];
	memset(tbl, ACCEPT, 64);
	memset(&tbl[64], ACCEPT, 64);
	memset(&tbl[128], ACCEPT, 64);
	memset(&tbl[192], ACCEPT, 64);
	tbl[0] = NUL;
	do
		tbl[(unsigned char)*reject++] = REJECT;
	while (*reject);
	for (const unsigned char *src = (unsigned char *)s;; ++src) {
		switch (tbl[*src]) {
		case ACCEPT:
			*s++ = *src;
		case REJECT:
			continue;
		case NUL:;
		}
		break;
	}
	*s = '\0';
	return s;
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
static char *jstr_rm_mem_p(char *JSTR_RST const s,
			   const char *JSTR_RST const searc,
			   const size_t sz,
			   const size_t searclen) JSTR_NOEXCEPT
{
	char *const p = (char *)JSTR_MEMMEM(s, sz, searc, searclen);
	if (unlikely(!p))
		return s + sz;
	memmove(p, p + searclen, (s + sz) - p);
	return s + sz - searclen;
}

/*
  Remove N HS in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_RETURNS_NONNULL
static char *jstr_rmn_mem_p_constexpr(char *JSTR_RST s,
				      const char *JSTR_RST const searc,
				      size_t n,
				      size_t sz,
				      const size_t searclen) JSTR_NOEXCEPT
{
	if (unlikely(searclen > sz))
		return s + sz;
	switch (searclen) {
	case 0: return s + sz;
	case 1: return private_jstr_rmn_memmem1(s, *searc, n, sz);
	case 2: return private_jstr_rmn_memmem2(s, searc, n, sz);
	/* case 3: return private_jstr_rmn_memmem3(s, searc, n, sz); */
	/* case 4: return private_jstr_rmn_memmem4(s, searc, n, sz); */
	default: return private_jstr_rmn_memmem5(s, searc, n, sz, searclen);
	}
}

/*
  Remove N HS in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE_IF_CONSTEXPR
JSTR_RETURNS_NONNULL
static char *jstr_rmn_mem_p(char *JSTR_RST s,
			    const char *JSTR_RST const searc,
			    size_t n,
			    size_t sz,
			    const size_t searclen) JSTR_NOEXCEPT
{
	return jstr_rmn_mem_p_constexpr(s, searc, n, sz, searclen);
}

/*
  Remove all HS in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_RETURNS_NONNULL
static char *jstr_rmall_mem_p_constexpr(char *JSTR_RST s,
					const char *JSTR_RST const searc,
					size_t sz,
					const size_t searclen) JSTR_NOEXCEPT
{
	if (unlikely(searclen > sz))
		return s + sz;
	switch (searclen) {
	case 0: return s + sz;
	case 1: return private_jstr_rmall_memmem1(s, *searc, sz);
	case 2: return private_jstr_rmall_memmem2(s, searc, sz);
	/* case 3: return private_jstr_rmall_memmem3(s, searc, sz); */
	/* case 4: return private_jstr_rmall_memmem4(s, searc, sz); */
	default: return private_jstr_rmall_memmem5(s, searc, sz, searclen);
	}
}

/*
  Remove all HS in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE_IF_CONSTEXPR
JSTR_RETURNS_NONNULL
static char *jstr_rmall_mem_p(char *JSTR_RST s,
			      const char *JSTR_RST const searc,
			      size_t sz,
			      const size_t searclen) JSTR_NOEXCEPT
{
	return jstr_rmall_mem_p_constexpr(s, searc, sz, searclen);
}

/*
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replacec_mem(char *JSTR_RST s,
			      const int searc,
			      const int rplc,
			      const size_t sz) JSTR_NOEXCEPT
{
	s = (char *)memchr(s, searc, sz);
	if (unlikely(!s))
		return;
	*s = rplc;
}

/*
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replacec(char *JSTR_RST s,
			  const int searc,
			  const int rplc) JSTR_NOEXCEPT
{
	s = strchr(s, searc);
	if (unlikely(!s))
		return;
	*s = rplc;
}

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replaceallc_mem(char *JSTR_RST s,
				 const int searc,
				 const int rplc,
				 const size_t sz) JSTR_NOEXCEPT
{
	s = (char *)memchr(s, searc, sz);
	if (unlikely(!s))
		return;
	goto MTC;
	for (;; ++s)
		if (*s == searc)
		MTC:
			*s = rplc;
		else if (unlikely(!*s))
			break;
}

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replaceallc(char *JSTR_RST s,
			     const int searc,
			     const int rplc) JSTR_NOEXCEPT
{
	s = strchr(s, searc);
	if (unlikely(!s))
		return;
	goto MTC;
	for (;; ++s)
		if (*s == searc)
		MTC:
			*s = rplc;
		else if (unlikely(!*s))
			break;
}

/*
  Replace N SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replacenc_mem(char *JSTR_RST s,
			       const int searc,
			       const int rplc,
			       size_t n,
			       const size_t sz) JSTR_NOEXCEPT
{
	s = (char *)memchr(s, searc, sz);
	if (unlikely(!s))
		return;
	goto MTC;
	for (;; ++s)
		if (*s == searc) {
		MTC:
			*s = rplc;
			if (unlikely(!--n))
				break;
		} else if (unlikely(!*s)) {
			break;
		}
}

/*
  Replace N SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replacenc(char *JSTR_RST s,
			   const int searc,
			   const int rplc,
			   size_t n) JSTR_NOEXCEPT
{
	s = strchr(s, searc);
	if (unlikely(!s))
		return;
	goto MTC;
	for (;;) {
		if (*s == searc) {
		MTC:
			*s = rplc;
			if (unlikely(!--n))
				break;
		} else if (unlikely(!*++s)) {
			break;
		}
	}
}

/*
  Replace first SEARCH in S with REPLACE.
  Assumes that SEARCHLEN > REPLACELEN.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *jstr_replace_mem_p_f(char *JSTR_RST const s,
				  const char *JSTR_RST const searc,
				  const char *JSTR_RST const rplc,
				  const size_t sz,
				  const size_t searclen,
				  const size_t rplclen) JSTR_NOEXCEPT
{
	if (unlikely(searclen == 0))
		return s + sz;
	switch (rplclen) {
	case 0:
		return jstr_rm_mem_p(s, searc, sz, searclen);
	case 1:
		if (searclen == 1) {
			jstr_replacec_mem(s, *searc, *rplc, sz);
			return s + sz;
		}
		/* FALLTHROUGH */
	default: {
		char *mtc = (char *)JSTR_MEMMEM(s, sz, searc, searclen);
		if (unlikely(!mtc))
			return s + sz;
		memmove(mtc + rplclen,
			mtc + searclen,
			(s + sz + 1) - mtc + searclen);
		memcpy(mtc, rplc, rplclen);
		return s + sz - (long long)(rplclen - searclen);
	}
	}
}

/*
  Replace first SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
static void jstr_replace_mem_constexpr(char **JSTR_RST const s,
				       size_t *JSTR_RST const sz,
				       size_t *JSTR_RST const cap,
				       const char *JSTR_RST const searc,
				       const char *JSTR_RST const rplc,
				       const size_t searclen,
				       const size_t rplclen) JSTR_NOEXCEPT
{
	if (unlikely(searclen == 0))
		return;
	switch (rplclen) {
	case 0:
		*sz = jstr_rm_mem_p(*s, searc, *sz, searclen) - *s;
		return;
	case 1:
		if (searclen == 1) {
			jstr_replacec_mem(*s, *searc, *rplc, *sz);
			return;
		}
		/* FALLTHROUGH */
	default: {
		char *mtc = (char *)JSTR_MEMMEM(*s, *sz, searc, searclen);
		if (unlikely(!mtc))
			return;
		if (rplclen <= searclen || *cap > *sz + rplclen - searclen + 1) {
			memmove(mtc + rplclen,
				mtc + searclen,
				(*s + *sz + 1) - mtc + searclen);
			memcpy(mtc, rplc, rplclen);
		} else {
			JSTR_GROW(*cap, *sz + rplclen + 1);
			char *const tmp = (char *)malloc(*cap);
			JSTR_MALLOC_ERR(tmp, return);
			memcpy(tmp, *s, mtc - *s);
			memcpy(tmp + (mtc - *s), rplc, rplclen);
			memcpy(tmp + (mtc - *s) + rplclen,
			       mtc + rplclen,
			       (*s + *sz + 1) - (mtc + rplclen));
			free(*s);
			*s = tmp;
		}
		*sz += (long long)(rplclen - searclen);
	}
	}
}

/*
  Replace first SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_INLINE_IF_CONSTEXPR
JSTR_MAYBE_UNUSED
static void jstr_replace_mem(char **JSTR_RST const s,
			     size_t *JSTR_RST const sz,
			     size_t *JSTR_RST const cap,
			     const char *JSTR_RST const searc,
			     const char *JSTR_RST const rplc,
			     const size_t searclen,
			     const size_t rplclen) JSTR_NOEXCEPT
{
	return jstr_replace_mem_constexpr(s, sz, cap, searc, rplc, searclen, rplclen);
}

/*
  Replace N SEARCH in S with REPLACE.
  Assumes that S have enough space for REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_RETURNS_NONNULL
static char *jstr_replacen_mem_p_f_constexpr(char *JSTR_RST s,
					     const char *JSTR_RST const searc,
					     const char *JSTR_RST const rplc,
					     size_t n,
					     size_t sz,
					     const size_t searclen,
					     const size_t rplclen) JSTR_NOEXCEPT
{
	if (unlikely(rplclen == 0))
		return jstr_rmn_mem_p(s, searc, n, sz, searclen);
	if (unlikely(searclen > sz))
		return s + sz;
	if (rplclen <= searclen) {
		switch (searclen) {
		case 0: return s + sz;
		case 1:
			private_jstr_replacenc_memmem1(s, *searc, *rplc, n, sz);
			return s + sz;
		case 2:
			return private_jstr_replacen_memmem2(s, searc, rplc, n, sz, rplclen);
			/* case 3: return private_jstr_replacen_memmem3(s, searc, rplc, n, sz, rplclen); */
			/* case 4: return private_jstr_replacen_memmem4(s, searc, rplc, n, sz, rplclen); */
		default: return private_jstr_replacen_memmem5(s, searc, rplc, n, sz, searclen, rplclen);
		}
	}
	return private_jstr_replacen_f(s, searc, rplc, n, sz, searclen, rplclen);
}

/*
  Replace N SEARCH in S with REPLACE.
  Assumes that S have enough space for REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_INLINE_IF_CONSTEXPR
JSTR_MAYBE_UNUSED
JSTR_RETURNS_NONNULL
static char *jstr_replacen_mem_p_f(char *JSTR_RST s,
				   const char *JSTR_RST const searc,
				   const char *JSTR_RST const rplc,
				   size_t n,
				   size_t sz,
				   const size_t searclen,
				   const size_t rplclen) JSTR_NOEXCEPT
{
	return jstr_replacen_mem_p_f_constexpr(s, searc, rplc, n, sz, searclen, rplclen);
}

/*
  Replace all SEARCH in S with REPLACE.
  Assumes that S have enough space for REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_RETURNS_NONNULL
static char *jstr_replaceall_mem_p_f_constexpr(char *JSTR_RST s,
					       const char *JSTR_RST const searc,
					       const char *JSTR_RST const rplc,
					       size_t sz,
					       const size_t searclen,
					       const size_t rplclen) JSTR_NOEXCEPT
{
	if (unlikely(rplclen == 0))
		return jstr_rmall_mem_p(s, searc, sz, searclen);
	if (unlikely(searclen > sz))
		return s + sz;
	if (rplclen <= searclen) {
		switch (searclen) {
		case 0: return s + sz;
		case 1: return private_jstr_replaceall_memmem1(s, *searc, *rplc, sz);
		default: return private_jstr_replaceall_memmem5(s, searc, rplc, sz, searclen, rplclen);
		case 2:
			return private_jstr_replaceall_memmem2(s, searc, rplc, sz, rplclen);
			/* case 3: return private_jstr_replaceall_memmem3(s, searc, rplc, sz, rplclen); */
			/* case 4: return private_jstr_replaceall_memmem4(s, searc, rplc, sz, rplclen); */
		}
	}
	return private_jstr_replaceall_f(s, searc, rplc, sz, searclen, rplclen);
}

/*
  Replace all SEARCH in S with REPLACE.
  Assumes that S have enough space for REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_INLINE_IF_CONSTEXPR
JSTR_MAYBE_UNUSED
JSTR_RETURNS_NONNULL
static char *jstr_replaceall_mem_p_f(char *JSTR_RST s,
				     const char *JSTR_RST const searc,
				     const char *JSTR_RST const rplc,
				     size_t sz,
				     const size_t searclen,
				     const size_t rplclen) JSTR_NOEXCEPT
{
	return jstr_replaceall_mem_p_f_constexpr(s, searc, rplc, sz, searclen, rplclen);
}

/*
  Replace N SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
static void jstr_replacen_mem_constexpr(char **JSTR_RST const s,
					size_t *JSTR_RST const sz,
					size_t *JSTR_RST const cap,
					const char *JSTR_RST const searc,
					const char *JSTR_RST const rplc,
					size_t n,
					const size_t searclen,
					const size_t rplclen) JSTR_NOEXCEPT
{
	if (unlikely(rplclen == 0)) {
		*sz = jstr_rmn_mem_p(*s, searc, n, *sz, searclen) - *s;
		return;
	}
	if (unlikely(searclen > *sz))
		return;
	if (rplclen <= searclen) {
		switch (searclen) {
		case 0: return;
		case 1:
			private_jstr_replacenc_memmem1(*s, *searc, *rplc, n, *sz);
			return;
		case 2:
			*sz = private_jstr_replacen_memmem2(*s, searc, rplc, n, *sz, rplclen) - *s;
			return;
		/* case 3: */
		/* 	*sz = private_jstr_replacen_memmem3(*s, searc, rplc, n, *sz, rplclen) - *s; */
		/* 	return; */
		/* case 4: */
		/* 	*sz = private_jstr_replacen_memmem4(*s, searc, rplc, n, *sz, rplclen) - *s; */
		/* 	return; */
		default:
			*sz = private_jstr_replacen_memmem5(*s, searc, rplc, n, *sz, searclen, rplclen) - *s;
			return;
		}
	}
	private_jstr_replacen_grow(s, sz, cap, searc, rplc, n, searclen, rplclen);
}

/*
  Replace N SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_INLINE_IF_CONSTEXPR
JSTR_MAYBE_UNUSED
static void jstr_replacen_mem(char **JSTR_RST const s,
			      size_t *JSTR_RST const sz,
			      size_t *JSTR_RST const cap,
			      const char *JSTR_RST const searc,
			      const char *JSTR_RST const rplc,
			      size_t n,
			      const size_t searclen,
			      const size_t rplclen) JSTR_NOEXCEPT
{
	jstr_replacen_mem_constexpr(s, sz, cap, searc, rplc, n, searclen, rplclen);
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
static void jstr_replaceall_mem_constexpr(char **JSTR_RST const s,
					  size_t *JSTR_RST const sz,
					  size_t *JSTR_RST const cap,
					  const char *JSTR_RST const searc,
					  const char *JSTR_RST const rplc,
					  const size_t searclen,
					  const size_t rplclen) JSTR_NOEXCEPT
{
	if (unlikely(rplclen == 0)) {
		*sz = jstr_rmall_mem_p(*s, searc, *sz, searclen) - *s;
		return;
	}
	if (unlikely(searclen > *sz))
		return;
	if (rplclen <= searclen) {
		switch (searclen) {
		case 0: return;
		case 1:
			*sz = private_jstr_replaceall_memmem1(*s, *searc, *rplc, *sz) - *s;
			return;
		case 2:
			*sz = private_jstr_replaceall_memmem2(*s, searc, rplc, *sz, rplclen) - *s;
			return;
			/* case 3: */
			/* 	*sz = private_jstr_replaceall_memmem3(*s, searc, rplc, *sz, rplclen) - *s; */
			/* 	return; */
			/* case 4: */
			/* 	*sz = private_jstr_replaceall_memmem4(*s, searc, rplc, *sz, rplclen) - *s; */
			/* 	return; */
		default:
			*sz = private_jstr_replaceall_memmem5(*s, searc, rplc, *sz, searclen, rplclen) - *s;
			return;
		}
	}
	private_jstr_replaceall_grow(s, sz, cap, searc, rplc, searclen, rplclen);
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_INLINE_IF_CONSTEXPR
JSTR_MAYBE_UNUSED
static void jstr_replaceall_mem(char **JSTR_RST const s,
				size_t *JSTR_RST const sz,
				size_t *JSTR_RST const cap,
				const char *JSTR_RST const searc,
				const char *JSTR_RST const rplc,
				const size_t searclen,
				const size_t rplclen) JSTR_NOEXCEPT
{
	jstr_replaceall_mem_constexpr(s, sz, cap, searc, rplc, searclen, rplclen);
}

#if JSTR_EXTERN_C && defined(__cplusplus)
}
#endif /* JSTR_EXTERN_C */
#if JSTR_NAMESPACE && !JSTR_IN_NAMESPACE && defined(__cplusplus)
}
#endif /* JSTR_NAMESPACE */

#undef JSTR_ASCII_SIZE

#if defined(__GNUC__) || defined(__clang__)
#	pragma GCC poison jstr_memmem2
#	pragma GCC poison jstr_memmem3
#	pragma GCC poison jstr_memmem4
#	pragma GCC poison jstr_memmem5
#	pragma GCC poison jstr_next_pow2_32
#	pragma GCC poison jstr_next_pow2_32_constexpr
#	pragma GCC poison jstr_next_pow2_64
#	pragma GCC poison jstr_next_pow2_64_constexpr
#	pragma GCC poison jstr_replaceall_f
#	pragma GCC poison jstr_replaceall_grow
#	pragma GCC poison jstr_replaceall_memmem1
#	pragma GCC poison jstr_replaceall_memmem2
#	pragma GCC poison jstr_replaceall_memmem3
#	pragma GCC poison jstr_replaceall_memmem4
#	pragma GCC poison jstr_replaceall_memmem5
#	pragma GCC poison jstr_replacenc_memmem1
#	pragma GCC poison jstr_replacen_f
#	pragma GCC poison jstr_replacen_grow
#	pragma GCC poison jstr_replacen_memmem2
#	pragma GCC poison jstr_replacen_memmem3
#	pragma GCC poison jstr_replacen_memmem4
#	pragma GCC poison jstr_replacen_memmem5
#	pragma GCC poison jstr_rmall_memmem1
#	pragma GCC poison jstr_rmall_memmem2
#	pragma GCC poison jstr_rmall_memmem3
#	pragma GCC poison jstr_rmall_memmem4
#	pragma GCC poison jstr_rmall_memmem5
#	pragma GCC poison jstr_rmn_memmem1
#	pragma GCC poison jstr_rmn_memmem2
#	pragma GCC poison jstr_rmn_memmem3
#	pragma GCC poison jstr_rmn_memmem4
#	pragma GCC poison jstr_rmn_memmem5
#endif /* defined(__GNUC__) || defined(__clang__) */

#endif /* JSTR_H_REPLACE_DEF */
