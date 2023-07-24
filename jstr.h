#ifndef JSTR_H_DEF
#define JSTR_H_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "jstr_macros.h"

#define JSTR_EXIT_IF_MALLOC_ERROR 1

#define JSTR_EXTERN_C  1
#define JSTR_NAMESPACE 0

JSTR_NOINLINE
static void JSTR_ERR(void) JSTR_NOEXCEPT
{
	fprintf(stderr, "%s:%d:%s:Can't malloc:", __FILE__, __LINE__, __func__);
	perror("");
#if JSTR_EXIT_IF_MALLOC_ERROR
	exit(1);
#endif /* JSTR_EXIT_IF_MALLOC_ERROR */
}

#if JSTR_NAMESPACE && defined(__cplusplus)
#	define JSTR_IN_NAMESPACE 1
namespace jstr {
#endif /* JSTR_NAMESPACE */
#if JSTR_EXTERN_C && defined(__cplusplus)
extern "C" {
#endif /* JSTR_EXTERN_C */

#include "jstr_ctype.h"
#include "jstr_string.h"

#define JSTR_RST JSTR_RESTRICT

#define JSTR_MALLOC_ERR(p, malloc_fail) \
	do {                            \
		if (unlikely(!(p))) {   \
			JSTR_ERR();     \
			malloc_fail;    \
		}                       \
	} while (0)

#define JSTR_GROW(oldcap, newcap) \
	while (((oldcap) *= 2) < (newcap))

#define JSTR_REALLOC(p, oldcap, newcap, malloc_fail)        \
	do {                                                \
		JSTR_GROW(oldcap, newcap);                  \
		(p) = JSTR_CAST(char *) realloc(p, newcap); \
		JSTR_MALLOC_ERR(p, malloc_fail);            \
	} while (0)

/*
  exit(1) if ptr is NULL.
*/
JSTR_INLINE
static void jstr_err(char *JSTR_RST p) JSTR_NOEXCEPT
{
	if (unlikely(!p))
		JSTR_ERR();
}

/*
  free(p) and set p to NULL.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_del(char *JSTR_RST p) JSTR_NOEXCEPT
{
	free(p);
	p = NULL;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc(char **JSTR_RST s,
		       size_t *JSTR_RST cap,
		       const size_t top) JSTR_NOEXCEPT
{
	*s = JSTR_CAST(char *) malloc(top * 2);
	JSTR_MALLOC_ERR(*s, return);
	*cap = top * 2;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc_appendmem(char **JSTR_RST dst,
				 size_t *JSTR_RST dsz,
				 size_t *JSTR_RST dcap,
				 const char *JSTR_RST src,
				 const size_t slen) JSTR_NOEXCEPT
{
	jstr_alloc(dst, dcap, slen * 2);
	if (unlikely(!*dst))
		return;
	*dsz = slen;
	memcpy(*dst, src, slen + 1);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc_append(char **JSTR_RST dst,
			      size_t *JSTR_RST dsz,
			      size_t *JSTR_RST dcap,
			      const char *JSTR_RST src) JSTR_NOEXCEPT
{
	jstr_alloc_appendmem(dst, dsz, dcap, src, strlen(src));
}

typedef struct jstr_t {
	size_t size;
	size_t cap;
	char *data;

#ifdef __cplusplus

	JSTR_INLINE
	JSTR_NONNULL_ALL
	jstr_t(const size_t len) JSTR_NOEXCEPT
	{
		jstr_alloc(&this->data, &this->cap, len);
	}

	JSTR_INLINE
	JSTR_NONNULL_ALL
	jstr_t(const char *JSTR_RST src) JSTR_NOEXCEPT
	{
		jstr_alloc_append(&this->data, &this->size, &this->cap, src);
	}

	JSTR_INLINE
	JSTR_NONNULL_ALL
	jstr_t(const char *JSTR_RST src,
	       const size_t slen) JSTR_NOEXCEPT
	{
		jstr_alloc_appendmem(&this->data, &this->size, &this->cap, src, slen);
	}

	JSTR_INLINE
	JSTR_NONNULL_ALL
	~jstr_t(void) JSTR_NOEXCEPT
	{
		free(this->data);
	}

	/*
	  free(STR) and set STR to NULL.
	*/
	JSTR_INLINE
	JSTR_NONNULL_ALL
	void del(void) JSTR_NOEXCEPT
	{
		free(this->data);
		this->data = NULL;
	}

	/*
	  exit(1) if STR is NULL.
	*/
	JSTR_INLINE
	void err(void) JSTR_NOEXCEPT
	{
		if (unlikely(!this->data))
			exit(1);
	}
#endif /* __cpluslus */
} jstr_t;

/*
   Append SRC to DST.
   Use non-f version for bounds checking.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_appendmemf(char **JSTR_RST dst,
			    size_t *JSTR_RST dsz,
			    const char *JSTR_RST src,
			    const size_t slen) JSTR_NOEXCEPT
{
	memcpy(*dst, src, slen + 1);
	*dsz += slen;
}

/*
   Append SRC to DST.
   Use non-f version for bounds checking.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_appendf(char **JSTR_RST dst,
			 size_t *JSTR_RST dsz,
			 const char *JSTR_RST src) JSTR_NOEXCEPT
{
	jstr_appendmemf(dst, dsz, src, strlen(src));
}

/*
   Append SRC to DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_appendmem(char **JSTR_RST dst,
			   size_t *JSTR_RST dsz,
			   size_t *JSTR_RST dcap,
			   const char *JSTR_RST src,
			   const size_t slen) JSTR_NOEXCEPT
{
	if (*dcap < *dsz + slen)
		JSTR_REALLOC(*dst, *dcap, *dsz + slen, return);
	jstr_appendmemf(dst, dsz, src, slen);
}

/*
   Append SRC to DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_append(char **JSTR_RST dst,
			size_t *JSTR_RST dsz,
			size_t *JSTR_RST dcap,
			const char *JSTR_RST src) JSTR_NOEXCEPT
{
	jstr_appendmem(dst, dsz, dcap, src, strlen(src));
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

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
#ifdef JSTR_HAS_MEMMEM

static int jstr_countsmem(const char *JSTR_RST hs,
			  const char *JSTR_RST ne,
			  const size_t hlen,
			  const size_t nlen) JSTR_NOEXCEPT
{
	int count = 0;
	for (const char *const end = hs + hlen;
	     (hs = JSTR_CAST(char *) memmem(hs, end - hs, ne, nlen));
	     ++count, hs += nlen)
		;
	return count;
}

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_counts(const char *JSTR_RST hs,
		       const char *JSTR_RST ne) JSTR_NOEXCEPT
{
	return jstr_countsmem(hs, ne, strlen(hs), strlen(ne));
}

#else

static int jstr_counts(const char *JSTR_RST hs,
		       const char *JSTR_RST ne) JSTR_NOEXCEPT
{
	int count = 0;
	while ((hs = strstr(hs, ne)))
		++count;
	return count;
}

#endif /* __JSTR_HAS_MEMMEM */

/*
  Reverse S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_revmem(char *JSTR_RST s,
			const size_t n) JSTR_NOEXCEPT
{
	if (unlikely(!n))
		return;
	char *end = s + n - 1;
	char tmp;
	do {
		tmp = *s;
		*s++ = *end;
		*end-- = tmp;
	} while (s < end);
}

/*
  Reverse S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_rev(char *JSTR_RST s) JSTR_NOEXCEPT
{
	jstr_revmem(s, strlen(s));
}

/*
  Trim spaces in [ \t] from end of S.
  Return value:
  Pointer to '\0' in S;
  NULL if SLEN is 0.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_trimmemp(char *JSTR_RST const s,
			   const size_t n) JSTR_NOEXCEPT
{
	if (unlikely(!n))
		return s;
	char *end = s + n - 1;
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
  Trim spaces in [ \t] from end of S.
  Return value:
  Pointer to '\0' in S;
  NULL if SLEN is 0.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_trim_p(char *JSTR_RST s) JSTR_NOEXCEPT
{
	return jstr_trimmemp(s, strlen(s));
}

/*
  Remove all C in S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_remove_c(char *JSTR_RST s,
			  const int c) JSTR_NOEXCEPT
{
	for (; *s; ++s)
		if (*s != c) {
			for (char *d = s; (*d++ = *s++);)
				;
			break;
		}
}

/*
  Remove all C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static char *jstr_removeall_c(char *JSTR_RST s,
			      const int c) JSTR_NOEXCEPT
{
	char *dst = s;
	for (; *s; ++s)
		if (*s != c)
			*dst++ = *s;
	*dst = '\0';
	return dst;
}

/*
  Remove characters in REJECT in S.
  Return value:
  Pointer to '\0' in S;
  NULL if REJECT is empty.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_stripspn_p(char *JSTR_RST s,
			     const char *JSTR_RST reject) JSTR_NOEXCEPT
{
	enum {
		ACCEPT = 0,
		REJECT,
		NUL,
	};
	if (unlikely(!reject[0]))
		return s;
	if (unlikely(!reject[1]))
		return jstr_removeall_c(s, *reject);
	unsigned char tbl[256];
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
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replace_c(char *JSTR_RST s,
			   const int srch,
			   const int rplc) JSTR_NOEXCEPT
{
	for (;; ++s) {
		if (*s == srch) {
			*s = rplc;
			break;
		}
		if (unlikely(!*s))
			break;
	}
}

JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static char *jstr_removemem_p(char *JSTR_RST s,
			      const char *JSTR_RST hs,
			      const size_t slen,
			      const size_t hlen) JSTR_NOEXCEPT
{
	char *const p = JSTR_CAST(char *) jstr_memmem(s, slen, hs, hlen);
	if (p) {
		memmove(p, p + hlen, (s + slen) - p);
		return s + slen - hlen;
	}
	return s + slen;
}

JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static char *jstr_remove_p(char *JSTR_RST s,
			   const char *JSTR_RST hs) JSTR_NOEXCEPT
{
	return jstr_removemem_p(s, hs, strlen(s), strlen(hs));
}

JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static char *jstr_removeallmem_p(char *JSTR_RST s,
				 const char *JSTR_RST const hs,
				 const size_t slen,
				 size_t hlen) JSTR_NOEXCEPT
{
	char *dst = s;
	const char *const end = s + slen - hlen;
	const size_t off = hlen - 8;
	while (s <= end) {
		if (*s == *hs && *(s + slen - 1) == *(hs + hlen - 1))
			if (hlen <= 15 || !memcmp(s + off, hs + off, 8))
				if (!memcmp(s, hs, hlen)) {
					s += hlen + 1;
					continue;
				}
		*dst++ = *s++;
	}
	memcpy(dst, s, s - end + hlen + 1);
	return dst;
}

/* 	for (char *rm = s; (rm = JSTR_CAST(char *) jstr_memmem(rm, (s + slen) - rm, rm, rmlen));) { */
/* 		slen -= rmlen; */
/* 		memmove(rm, */
/* 			rm + rmlen, */
/* 			(s + slen) - rm + 1); */
/* 		rm += rmlen; */
/* 	} */
/* 	return s + slen; */

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replaceall_c(char *JSTR_RST s,
			      const int srch,
			      const int rplc) JSTR_NOEXCEPT
{
	for (;; ++s) {
		if (*s == srch)
			*s = rplc;
		if (unlikely(!*s))
			break;
	}
}

/*
  Replace first SEARCH in S with REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replacemem(char **JSTR_RST s,
			    size_t *JSTR_RST ssz,
			    size_t *JSTR_RST scap,
			    const char *JSTR_RST srch,
			    const char *JSTR_RST rplc,
			    const size_t slen,
			    const size_t rlen) JSTR_NOEXCEPT
{
#define JSTR_REPLACE(update_mtc, malloc_fail)                         \
	do {                                                          \
		if (rlen <= slen || *scap > *ssz + rlen - slen + 1) { \
			memmove(mtc + rlen,                           \
				mtc + slen,                           \
				(*s + *ssz + 1) - mtc + slen);        \
			memcpy(mtc, rplc, rlen);                      \
			if (update_mtc)                               \
				mtc += rlen;                          \
		} else {                                              \
			JSTR_GROW(*scap, *ssz + rlen + 1);            \
			tmp = JSTR_CAST(char *) malloc(*scap);        \
			JSTR_MALLOC_ERR(tmp, malloc_fail);            \
			memcpy(tmp, *s, mtc - *s);                    \
			memcpy(tmp + (mtc - *s), rplc, rlen);         \
			memcpy(tmp + (mtc - *s) + rlen,               \
			       mtc + rlen,                            \
			       (*s + *ssz + 1) - (mtc + rlen));       \
			if (update_mtc)                               \
				mtc = tmp + (mtc - *s) + rlen;        \
			free(*s);                                     \
			*s = tmp;                                     \
		}                                                     \
		*ssz += (long long)(rlen - slen);                     \
	} while (0)
	char *mtc;
	if ((mtc = JSTR_CAST(char *) jstr_memmem(*s, *ssz, srch, slen))) {
		char *tmp;
		JSTR_REPLACE(0, return);
	}
}

/*
  Replace first SEARCH in S with REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replace(char **JSTR_RST s,
			 size_t *JSTR_RST ssz,
			 size_t *JSTR_RST scap,
			 const char *JSTR_RST srch,
			 const char *JSTR_RST rplc) JSTR_NOEXCEPT
{
	jstr_replacemem(s, ssz, scap, srch, rplc, strlen(srch), strlen(rplc));
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replaceallmem(char **JSTR_RST s,
			       size_t *JSTR_RST ssz,
			       size_t *JSTR_RST scap,
			       const char *JSTR_RST srch,
			       const char *JSTR_RST rplc,
			       const size_t slen,
			       const size_t rlen) JSTR_NOEXCEPT
{
	char *mtc = *s;
	char *tmp;
	while ((mtc = JSTR_CAST(char *) jstr_memmem(mtc, (*s + *ssz) - mtc, srch, slen)))
		JSTR_REPLACE(1, return);
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replaceall(char **JSTR_RST s,
			    size_t *JSTR_RST ssz,
			    size_t *JSTR_RST scap,
			    const char *JSTR_RST srch,
			    const char *JSTR_RST rplc) JSTR_NOEXCEPT
{
	jstr_replaceallmem(s, ssz, scap, srch, rplc, strlen(srch), strlen(rplc));
}

/*
  Insert SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slipmem(char **JSTR_RST dst,
			 size_t *JSTR_RST dsz,
			 size_t *JSTR_RST dcap,
			 const size_t at,
			 const char *JSTR_RST src,
			 const size_t slen) JSTR_NOEXCEPT
{
	if (*dcap > *dsz + slen + 1) {
		memmove(*dst + at + slen,
			*dst + at,
			*dsz - at + 1);
		memcpy(*dst + at, src, slen);
	} else {
		JSTR_GROW(*dcap, *dsz + slen + 1);
		char *const tmp = JSTR_CAST(char *) malloc(*dcap);
		JSTR_MALLOC_ERR(tmp, return);
		memcpy(tmp, *dst, at);
		memcpy(tmp + at, src, slen);
		memcpy(tmp + at + slen,
		       *dst + at,
		       *dsz - at + 1);
		free(*dst);
		*dst = tmp;
		*dsz += slen;
	}
}

/*
  Insert SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slip(char **JSTR_RST dst,
		      size_t *JSTR_RST dsz,
		      size_t *JSTR_RST dcap,
		      const size_t at,
		      const char *JSTR_RST src) JSTR_NOEXCEPT
{
	jstr_slipmem(dst, dsz, dcap, at, src, strlen(src));
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
static int jstr_endswithmem(const char *JSTR_RST s1,
			    const char *JSTR_RST s2,
			    const size_t l1,
			    const size_t l2) JSTR_NOEXCEPT
{
	return (l1 < l2) ? 1 : memcmp(s1 + l1 - l2, s2, l2);
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
static int jstr_endswith(const char *JSTR_RST s1,
			 const char *JSTR_RST s2,
			 const size_t l1) JSTR_NOEXCEPT
{
	return jstr_endswithmem(s1, s2, l1, strlen(s2));
}

#if defined(__cplusplus) && JSTR_EXTERN_C
}
#endif /* JSTR_EXTERN_C */
#if defined(__cplusplus) && JSTR_NAMESPACE
}
#endif /* JSTR_NAMESPACE */

#undef JSTR_RST
#undef JSTR_REPLACE
#undef JSTR_MALLOC_ERR
#undef JSTR_GROW
#undef JSTR_REALLOC

#endif /* JSTR_H_DEF */
