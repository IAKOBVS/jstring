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

#ifdef __cplusplus
extern "C" {
#endif // __cpluslus

#include "jstr_std.h"
#include "jstr_types.h"

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

#define JSTR_GROW_SMALL(oldcap, newcap) \
	while (((oldcap) *= 2) < (newcap))

#define JSTR_REALLOC_SMALL(p, oldcap, newcap, malloc_fail)  \
	do {                                                \
		JSTR_GROW_SMALL(oldcap, newcap);            \
		(p) = JSTR_CAST(char *) realloc(p, newcap); \
		JSTR_MALLOC_ERR(p, malloc_fail);            \
	} while (0)

#define JSTR_GROW(oldcap, newcap)                      \
	do {                                           \
		do {                                   \
			if ((oldcap)*2 > (newcap)) {   \
				(oldcap) *= 2;         \
				break;                 \
			}                              \
			if ((oldcap)*4 > (newcap)) {   \
				(oldcap) *= 4;         \
				break;                 \
			}                              \
			if ((oldcap)*16 > (newcap)) {  \
				(oldcap) *= 16;        \
				break;                 \
			}                              \
		} while (((oldcap) *= 32) < (newcap)); \
	} while (0)

#define JSTR_REALLOC(p, oldcap, newcap, malloc_fail)        \
	do {                                                \
		JSTR_GROW(p, oldcap, newcap);               \
		(p) = JSTR_CAST(char *) realloc(p, newcap); \
		JSTR_MALLOC_ERR(p, malloc_fail);            \
	} while (0)

JSTR_INLINE
static void jstr_alloc(char **JSTR_RST s,
		       const size_t newsz,
		       size_t *JSTR_RST cap) JSTR_NOEX
{
	*s = JSTR_CAST(char *) malloc(newsz * 2);
	JSTR_MALLOC_ERR(*s, return);
	*cap = newsz * 2;
}

JSTR_INLINE
static void jstr_alloc_appendmem(char **JSTR_RST dst,
				 size_t *JSTR_RST dsz,
				 size_t *JSTR_RST dcap,
				 const char *JSTR_RST src,
				 const size_t ssz) JSTR_NOEX
{
	jstr_alloc(dst, ssz * 2, dcap);
	if (unlikely(!*dst))
		return;
	*dsz = ssz;
	memcpy(*dst, src, ssz + 1);
}

JSTR_INLINE
static void jstr_alloc_append(char **JSTR_RST dst,
			      size_t *JSTR_RST dsz,
			      size_t *JSTR_RST dcap,
			      const char *JSTR_RST src) JSTR_NOEX
{
	jstr_alloc_appendmem(dst, dsz, dcap, src, strlen(src));
}

JSTR_INLINE
static void jstr_appendmemf(char **JSTR_RST dst,
			    size_t *JSTR_RST dsz,
			    const char *JSTR_RST src,
			    const size_t ssz) JSTR_NOEX
{
	memcpy(*dst, src, ssz + 1);
	*dsz += ssz;
}

JSTR_INLINE
static void jstr_appendf(char **JSTR_RST dst,
			 size_t *JSTR_RST dsz,
			 const char *JSTR_RST src) JSTR_NOEX
{
	jstr_appendmemf(dst, dsz, src, strlen(src));
}

JSTR_INLINE
static void jstr_appendmem(char **JSTR_RST dst,
			   size_t *JSTR_RST dsz,
			   size_t *JSTR_RST dcap,
			   const char *JSTR_RST src,
			   const size_t ssz) JSTR_NOEX
{
	if (*dcap < *dsz + ssz)
		JSTR_REALLOC_SMALL(*dst, *dcap, *dsz + ssz, return);
	jstr_appendmemf(dst, dsz, src, ssz);
}

JSTR_INLINE
static void jstr_append(char **JSTR_RST dst,
			size_t *JSTR_RST dsz,
			size_t *JSTR_RST dcap,
			const char *JSTR_RST src) JSTR_NOEX
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
JSTR_WARN_UNUSED
static int jstr_countc(const char *JSTR_RST s, const int c) JSTR_NOEX
{
	int count = 0;
	while (*s)
		if (*s == c)
			++count;
	return count;
}

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
#ifdef JSTR_HAS_MEMMEM

static int jstr_counts(const char *JSTR_RST hs,
		       size_t hslen,
		       const char *JSTR_RST ne,
		       const size_t nelen) JSTR_NOEX
{
	int count = 0;
	for (const char *old = hs;
	     (hs = JSTR_CAST(char *) memmem(hs, hslen, ne, nelen));
	     hslen -= (hs - old), ++count)
		;
	return count;
}

#else

static int jstr_counts(const char *JSTR_RST hs, const char *JSTR_RST ne) JSTR_NOEX
{
	int count = 0;
	while ((hs = JSTR_CAST(char *) strstr(hs, ne)))
		++count;
	return count;
}

#endif // __JSTR_HAS_MEMMEM

/*
  Reverse S.
*/
JSTR_INLINE
static void jstr_revmem(char *JSTR_RST s, const size_t n) JSTR_NOEX
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
static void jstr_rev(char *JSTR_RST s) JSTR_NOEX
{
	jstr_revmem(s, strlen(s));
}

JSTR_INLINE
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

/*
  Remove all C in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
static char *jstr_stripp(char *JSTR_RST s, const int c) JSTR_NOEX
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
static char *jstr_stripspnp(char *JSTR_RST s, const char *JSTR_RST reject) JSTR_NOEX
{
	enum {
		ACCEPT = 0,
		REJECT,
		NUL,
	};
	if (unlikely(!reject[0]))
		return NULL;
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
  Trim spaces [ \t] from end of S.
  Return value:
  Pointer to '\0' in S;
  NULL if SLEN is 0.
*/
JSTR_INLINE
static char *jstr_trimmemp(char *JSTR_RST s, const size_t n) JSTR_NOEX
{
	if (unlikely(!n))
		return NULL;
	char *end = s + n - 1;
	do {
		switch (*end) {
		case '\t':
		case ' ':
			--end;
			continue;
		default:
			*(end + 1) = '\0';
		}
		break;
	} while (end >= s);
	return end;
}

/*
  Trim spaces [ \t] from end of S.
  Return value:
  Pointer to '\0' in S;
  NULL if SLEN is 0.
*/
JSTR_INLINE
static char *jstr_trimp(char *JSTR_RST s) JSTR_NOEX
{
	return jstr_trimmemp(s, strlen(s));
}

/*
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
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
static void jstr_replace(char **JSTR_RST s,
			 size_t *JSTR_RST ssz,
			 size_t *JSTR_RST scap,
			 const char *JSTR_RST search,
			 const char *JSTR_RST replace) JSTR_NOEX
{
	jstr_replacemem(s, ssz, scap, search, strlen(search), replace, strlen(replace));
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_INLINE
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
static void jstr_replaceall(char **JSTR_RST s,
			    size_t *JSTR_RST ssz,
			    size_t *JSTR_RST scap,
			    const char *JSTR_RST search,
			    const char *JSTR_RST replace) JSTR_NOEX
{
	jstr_replaceallmem(s, ssz, scap, search, strlen(search), replace, strlen(replace));
}

/*
  Insert SRC into DST[AT].
*/
JSTR_INLINE
static void jstr_insertmem(char **JSTR_RST dst,
			   size_t *JSTR_RST dsz,
			   size_t *JSTR_RST dcap,
			   const size_t at,
			   char *JSTR_RST src,
			   size_t ssz) JSTR_NOEX
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
static void jstr_insert(char **JSTR_RST dst,
			size_t *JSTR_RST dsz,
			size_t *JSTR_RST dcap,
			const size_t at,
			char *JSTR_RST src) JSTR_NOEX
{
	jstr_insertmem(dst, dsz, dcap, at, src, strlen(src));
}

JSTR_INLINE
JSTR_CONST
static int jstr_endswithmem(char *JSTR_RST s1,
			    const size_t l1,
			    char *JSTR_RST s2,
			    size_t l2) JSTR_NOEX
{
	return (l1 < l2) ? 1 : memcmp(s1 + l1 - l2, s2, l2);
}

JSTR_INLINE
JSTR_CONST
static int jstr_endswith(char *JSTR_RST s1,
			 const size_t l1,
			 char *JSTR_RST s2) JSTR_NOEX
{
	const size_t l2 = strlen(s2);
	return (l1 < l2) ? 1 : memcmp(s1 + l1 - l2, s2, l2);
}

#ifdef __cplusplus
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
