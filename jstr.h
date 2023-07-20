#ifndef JSTR_H_DEF
#define JSTR_H_DEF

#ifdef __cplusplus
extern "C" {
#endif // __cpluslus

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
}
#endif // __cpluslus

#include "jstr_macros.h"

#define JSTR_EXTERN_C  1
#define JSTR_NAMESPACE 0

#if JSTR_NAMESPACE && defined(__cplusplus)
namespace jstr {
#endif /* JSTR_NAMESPACE */

#if JSTR_EXTERN_C && defined(__cplusplus)
extern "C" {
#endif // __cpluslus

#include "jstr_ctype.h"
#include "jstr_string.h"

#define JSTR_RST  JSTR_RESTRICT
#define JSTR_NOEX JSTR_NOEXCEPT

typedef struct jstr_t {
	size_t size;
	size_t cap;
	char *data;
} jstr_t;

#define JSTR_MALLOC_ERR(p, malloc_fail)                                            \
	do {                                                                       \
		if (unlikely(!(p))) {                                              \
			fprintf(stderr, "%s:%d:Can't malloc", __FILE__, __LINE__); \
			malloc_fail;                                               \
		}                                                                  \
	} while (0)

#define JSTR_GROW(oldcap, newcap) \
	while (((oldcap) *= 2) < (newcap))

#define JSTR_REALLOC(p, oldcap, newcap, malloc_fail)        \
	do {                                                \
		JSTR_GROW(oldcap, newcap);                  \
		(p) = JSTR_CAST(char *) realloc(p, newcap); \
		JSTR_MALLOC_ERR(p, malloc_fail);            \
	} while (0)

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc(char **JSTR_RST s,
		       size_t *JSTR_RST cap,
		       const size_t top) JSTR_NOEX
{
	*s = JSTR_CAST(char *) malloc(top * 2);
	JSTR_MALLOC_ERR(*s, return);
	*cap = top * 2;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc_j(jstr_t *JSTR_RST j,
			 const size_t top) JSTR_NOEX
{
	jstr_alloc(&j->data, &j->cap, top);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc_appendmem(char **JSTR_RST dst,
				 size_t *JSTR_RST dsz,
				 size_t *JSTR_RST dcap,
				 const char *JSTR_RST src,
				 const size_t ssz) JSTR_NOEX
{
	jstr_alloc(dst, dcap, ssz * 2);
	if (unlikely(!*dst))
		return;
	*dsz = ssz;
	memcpy(*dst, src, ssz + 1);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc_appendmem_j(jstr_t *JSTR_RST j,
				   const char *JSTR_RST src,
				   const size_t ssz) JSTR_NOEX
{
	jstr_alloc_appendmem(&j->data, &j->size, &j->cap, src, ssz);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc_append(char **JSTR_RST dst,
			      size_t *JSTR_RST dsz,
			      size_t *JSTR_RST dcap,
			      const char *JSTR_RST src) JSTR_NOEX
{
	jstr_alloc_appendmem(dst, dsz, dcap, src, strlen(src));
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_alloc_append_j(jstr_t *JSTR_RST j,
				const char *JSTR_RST src) JSTR_NOEX
{
	jstr_alloc_appendmem_j(j, src, strlen(src));
}

/*
   Append SRC to DST.
   Use non-f version for bounds checking.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_appendmemf(char **JSTR_RST dst,
			    size_t *JSTR_RST dsz,
			    const char *JSTR_RST src,
			    const size_t ssz) JSTR_NOEX
{
	memcpy(*dst, src, ssz + 1);
	*dsz += ssz;
}

/*
   Append SRC to DST.
   Use non-f version for bounds checking.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_appendmemf_j(jstr_t *JSTR_RST j,
			      const char *JSTR_RST src,
			      const size_t ssz) JSTR_NOEX
{
	jstr_appendmemf(&j->data, &j->size, src, ssz);
}

/*
   Append SRC to DST.
   Use non-f version for bounds checking.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_appendf(char **JSTR_RST dst,
			 size_t *JSTR_RST dsz,
			 const char *JSTR_RST src) JSTR_NOEX
{
	jstr_appendmemf(dst, dsz, src, strlen(src));
}

/*
   Append SRC to DST.
   Use non-f version for bounds checking.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_appendf_j(jstr_t *JSTR_RST j,
			   const char *JSTR_RST src) JSTR_NOEX
{
	jstr_appendmemf(&j->data, &j->size, src, strlen(src));
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
			   const size_t ssz) JSTR_NOEX
{
	if (*dcap < *dsz + ssz)
		JSTR_REALLOC(*dst, *dcap, *dsz + ssz, return);
	jstr_appendmemf(dst, dsz, src, ssz);
}

/*
   Append SRC to DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_appendmem_j(jstr_t *JSTR_RST j,
			     const char *JSTR_RST src,
			     const size_t ssz) JSTR_NOEX
{
	jstr_appendmem(&j->data, &j->size, &j->cap, src, ssz);
}

/*
   Append SRC to DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_append(char **JSTR_RST dst,
			size_t *JSTR_RST dsz,
			size_t *JSTR_RST dcap,
			const char *JSTR_RST src) JSTR_NOEX
{
	jstr_appendmem(dst, dsz, dcap, src, strlen(src));
}

/*
   Append SRC to DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_append_j(jstr_t *JSTR_RST j,
			  const char *JSTR_RST src) JSTR_NOEX
{
	jstr_appendmem_j(j, src, strlen(src));
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
		       const int c) JSTR_NOEX
{
	int count = 0;
	for (;; ++s)
		if (*s == c)
			++count;
		else if (unlikely(*s == '\0'))
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

static int jstr_counts(const char *JSTR_RST hs,
		       size_t hlen,
		       const char *JSTR_RST ne,
		       const size_t nlen) JSTR_NOEX
{
	int count = 0;
	for (const char *const old = hs;
	     (hs = JSTR_CAST(char *) memmem(hs, hlen - (hs - old), ne, nlen));
	     ++count)
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
static int jstr_counts_j(const jstr_t *JSTR_RST j,
			 const char *JSTR_RST ne,
			 const size_t nlen) JSTR_NOEX
{
	return jstr_counts(j->data, j->size, ne, nlen);
}

#else

static int jstr_counts(const char *JSTR_RST hs,
		       const char *JSTR_RST ne) JSTR_NOEX
{
	int count = 0;
	while ((hs = strstr(hs, ne)))
		++count;
	return count;
}

#endif // __JSTR_HAS_MEMMEM

/*
  Reverse S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_revmem(char *JSTR_RST s,
			const size_t n) JSTR_NOEX
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
static void jstr_rev(char *JSTR_RST s) JSTR_NOEX
{
	jstr_revmem(s, strlen(s));
}
/*
  Reverse S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_rev_j(jstr_t *JSTR_RST j) JSTR_NOEX
{
	jstr_revmem(j->data, j->size);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_swap(char **JSTR_RST s1,
		      size_t *s1sz,
		      size_t *s1cap,
		      char **JSTR_RST s2,
		      size_t *s2sz,
		      size_t *s2cap) JSTR_NOEX
{
	char *const tmp = *s1;
	*s1 = *s2;
	*s2 = tmp;
	size_t tmp_sz = *s1sz;
	*s1sz = *s2sz;
	*s2sz = tmp_sz;
	tmp_sz = *s1cap;
	*s1cap = *s2cap;
	*s2cap = tmp_sz;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_swap_j(jstr_t *JSTR_RST j1,
			jstr_t *JSTR_RST j2) JSTR_NOEX
{
	const jstr_t tmp = *j1;
	*j1 = *j2;
	*j2 = tmp;
}

/*
  Remove all C in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_stripp(char *JSTR_RST s,
			 const int c) JSTR_NOEX
{
	for (const char *src = s;; ++src)
		if (*src != c)
			*s++ = *src;
		else if (unlikely(*src == '\0'))
			break;
	*s = '\0';
	return s;
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
static char *jstr_stripspnp(char *JSTR_RST s,
			    const char *JSTR_RST reject) JSTR_NOEX
{
	enum {
		ACCEPT = 0,
		REJECT,
		NUL,
	};
	if (unlikely(!reject[0]))
		return s;
	if (unlikely(!reject[1]))
		return jstr_stripp(s, *reject);
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
  Trim spaces in [ \t] from end of S.
  Return value:
  Pointer to '\0' in S;
  NULL if SLEN is 0.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_trimmemp(char *JSTR_RST const s,
			   const size_t n) JSTR_NOEX
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
static char *jstr_trimp(char *JSTR_RST s) JSTR_NOEX
{
	return jstr_trimmemp(s, strlen(s));
}

/*
  Trim spaces in [ \t] from end of S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_trim_j(jstr_t *JSTR_RST j) JSTR_NOEX
{
	j->size = jstr_trimmemp(j->data, j->size) - j->data;
}

/*
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replacec(char *JSTR_RST s,
			  const int search,
			  const int replace) JSTR_NOEX
{
	for (;; ++s) {
		if (*s == search) {
			*s = replace;
			break;
		}
		if (unlikely(*s == '\0'))
			break;
	}
}

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replacecall(char *JSTR_RST s,
			     const int search,
			     const int replace) JSTR_NOEX
{
	for (;; ++s) {
		if (*s == search)
			*s = replace;
		if (unlikely(*s == '\0'))
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
			    const char *JSTR_RST search,
			    const size_t slen,
			    const char *JSTR_RST replace,
			    const size_t rlen) JSTR_NOEX
{
	char *mtc;
	char *tmp;
	if ((mtc = JSTR_CAST(char *)
#ifdef JSTR_HAS_MEMMEM
	     memmem(*s, *ssz, search, slen)
#else
	     strstr(*s, search)
#endif
	     ))
#define JSTR_REPLACE(update_mtc, malloc_fail)                         \
	do {                                                          \
		if (rlen <= slen || *scap > *ssz + rlen - slen + 1) { \
			memmove(mtc + rlen,                           \
				mtc + slen,                           \
				(*s + *ssz + 1) - mtc + slen);        \
			memcpy(mtc, replace, rlen);                   \
			if (update_mtc)                               \
				mtc += rlen;                          \
		} else {                                              \
			JSTR_GROW(*scap, *ssz + rlen + 1);            \
			tmp = JSTR_CAST(char *) malloc(*scap);        \
			JSTR_MALLOC_ERR(tmp, malloc_fail);            \
			memcpy(tmp, *s, mtc - *s);                    \
			memcpy(tmp + (mtc - *s), replace, rlen);      \
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
		JSTR_REPLACE(0, return);
}

/*
  Replace first SEARCH in S with REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replace(char **JSTR_RST s,
			 size_t *JSTR_RST ssz,
			 size_t *JSTR_RST scap,
			 const char *JSTR_RST search,
			 const char *JSTR_RST replace) JSTR_NOEX
{
	jstr_replacemem(s, ssz, scap, search, strlen(search), replace, strlen(replace));
}

/*
  Replace first SEARCH in S with REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replacemem_j(jstr_t *JSTR_RST j,
			      const char *JSTR_RST search,
			      const size_t slen,
			      const char *JSTR_RST replace,
			      const size_t rlen) JSTR_NOEX
{
	jstr_replacemem(&j->data, &j->size, &j->cap, search, slen, replace, rlen);
}

/*
  Replace first SEARCH in S with REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replace_j(jstr_t *JSTR_RST j,
			   const char *JSTR_RST search,
			   const char *JSTR_RST replace) JSTR_NOEX
{
	jstr_replacemem_j(j, search, strlen(search), replace, strlen(replace));
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replaceallmem(char **JSTR_RST s,
			       size_t *JSTR_RST ssz,
			       size_t *JSTR_RST scap,
			       const char *JSTR_RST search,
			       const size_t slen,
			       const char *JSTR_RST replace,
			       const size_t rlen) JSTR_NOEX
{
	char *mtc = *s;
	char *tmp;
	while ((mtc = JSTR_CAST(char *)
#ifdef JSTR_HAS_MEMMEM
		memmem(mtc, (*s + *ssz) - mtc, search, slen)
#else
		strstr(mtc, search)
#endif
		))
		JSTR_REPLACE(1, return);
#undef JSTR_REPLACE
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replaceallmem_j(jstr_t *JSTR_RST j,
				 const char *JSTR_RST search,
				 const size_t slen,
				 const char *JSTR_RST replace,
				 const size_t rlen) JSTR_NOEX
{
	jstr_replaceallmem(&j->data, &j->size, &j->cap, search, slen, replace, rlen);
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replaceall(char **JSTR_RST s,
			    size_t *JSTR_RST ssz,
			    size_t *JSTR_RST scap,
			    const char *JSTR_RST search,
			    const char *JSTR_RST replace) JSTR_NOEX
{
	jstr_replaceallmem(s, ssz, scap, search, strlen(search), replace, strlen(replace));
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replaceall_j(jstr_t *JSTR_RST j,
			      const char *JSTR_RST search,
			      const char *JSTR_RST replace) JSTR_NOEX
{
	jstr_replaceallmem_j(j, search, strlen(search), replace, strlen(replace));
}

/*
  Insert SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertmem(char **JSTR_RST dst,
			   size_t *JSTR_RST dsz,
			   size_t *JSTR_RST dcap,
			   const size_t at,
			   const char *JSTR_RST src,
			   const size_t ssz) JSTR_NOEX
{
	if (*dcap > *dsz + ssz + 1) {
		memmove(*dst + at + ssz,
			*dst + at,
			*dsz - at + 1);
		memcpy(*dst + at, src, ssz);
	} else {
		JSTR_GROW(*dcap, *dsz + ssz + 1);
		char *const tmp = JSTR_CAST(char *) malloc(*dcap);
		JSTR_MALLOC_ERR(tmp, return);
		memcpy(tmp, *dst, at);
		memcpy(tmp + at, src, ssz);
		memcpy(tmp + at + ssz,
		       *dst + at,
		       *dsz - at + 1);
		free(*dst);
		*dst = tmp;
		*dsz += ssz;
	}
}

/*
  Insert SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertmem_j(jstr_t *JSTR_RST j,
			     const size_t at,
			     const char *JSTR_RST src,
			     const size_t ssz) JSTR_NOEX
{
	jstr_insertmem(&j->data, &j->size, &j->cap, at, src, ssz);
}

/*
  Insert SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insert(char **JSTR_RST dst,
			size_t *JSTR_RST dsz,
			size_t *JSTR_RST dcap,
			const size_t at,
			const char *JSTR_RST src) JSTR_NOEX
{
	jstr_insertmem(dst, dsz, dcap, at, src, strlen(src));
}

/*
  Insert SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insert_j(jstr_t *JSTR_RST j,
			  const size_t at,
			  const char *JSTR_RST src) JSTR_NOEX
{
	jstr_insertmem_j(j, at, src, strlen(src));
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
			    const size_t l1,
			    const char *JSTR_RST s2,
			    const size_t l2) JSTR_NOEX
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
static int jstr_endswithmem_j(const jstr_t *JSTR_RST j,
			      const char *JSTR_RST s2,
			      const size_t l2) JSTR_NOEX
{
	return (j->size < l2) ? 1 : memcmp(j->data + j->size - l2, s2, l2);
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
			 const size_t l1,
			 const char *JSTR_RST s2) JSTR_NOEX
{
	return jstr_endswithmem(s1, l1, s2, strlen(s2));
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
static int jstr_endswith_j(const jstr_t *JSTR_RST j,
			   const char *JSTR_RST s2) JSTR_NOEX
{
	return jstr_endswithmem_j(j, s2, strlen(s2));
}

#if defined(__cplusplus) && JSTR_NAMESPACE
}
#endif /* JSTR_NAMESPACE */

#if defined(__cplusplus) && JSTR_EXTERN_C
}
#endif // __cplusplus

#undef JSTR_NOEX
#undef JSTR_RST
#undef JSTR_MALLOC_ERR
#undef JSTR_GROW_SMALL
#undef JSTR_REALLOC_SMALL
#undef JSTR_GROW
#undef JSTR_REALLOC

#endif // JSTR_H_DEF
