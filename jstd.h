#ifndef JSTD_H_DEF
#define JSTD_H_DEF

#ifdef __cplusplus
#	include <cstddef>
#	include <cstring>
#	include <cstdlib>
#	include <cstdio>
extern "C" {
#else
#	include <stddef.h>
#	include <string.h>
#	include <stdlib.h>
#	include <stdio.h>
#endif // __cplusplus

#ifdef __cplusplus
}
#endif // __cpluslus

#include "jstd_macros.h"

#ifndef __LINE__
#	define __LINE__
#endif /* __LINE__ */

#ifndef __FILE__
#	define __FILE__
#endif /* __FILE__ */

#define JSTD_RST  JSTD_RESTRICT
#define JSTD_NOEX JSTD_NOEXCEPT

#ifdef __cplusplus
extern "C" {
#endif // __cpluslus

#ifdef JSTD_HAS_STPCPY

#	define jstd_stpcpy(dst, src) stpcpy(dst, src)

#else

/*
  Return value:
  pointer to '\0' in DST.
*/
JSTD_INLINE
JSTD_WARN_UNUSED
char *jstd_stpcpy(char *JSTD_RST dst, const char *JSTD_RST src) JSTD_NOEX
{
	const size_t slen = strlen(src);
	memcpy(dst, src, slen + 1);
	return dst + slen;
}

#endif // JSTD_HAS_STPCPY

#define JSTD_MALLOC_ERR(p, malloc_fail)                                            \
	do {                                                                       \
		if (unlikely(!(p))) {                                              \
			fprintf(stderr, "%s:%d:Can't malloc", __FILE__, __LINE__); \
			malloc_fail;                                               \
		}                                                                  \
	} while (0)

#define JSTD_GROW_SMALL(oldcap, newcap) \
	while (((oldcap) *= 2) < (newcap))

#define JSTD_REALLOC_SMALL(p, oldcap, newcap, malloc_fail)  \
	do {                                                \
		JSTD_GROW_SMALL(oldcap, newcap);            \
		(p) = JSTD_CAST(char *) realloc(p, newcap); \
		JSTD_MALLOC_ERR(p, malloc_fail);            \
	} while (0)

#define JSTD_GROW(oldcap, newcap)                     \
	do {                                          \
		do {                                  \
			if ((oldcap)*2 > (newcap)) {  \
				(oldcap) *= 2;        \
				break;                \
			}                             \
			if ((oldcap)*4 > (newcap)) {  \
				(oldcap) *= 4;        \
				break;                \
			}                             \
			if ((oldcap)*16 > (newcap)) { \
				(oldcap) *= 16;       \
				break;                \
			}                             \
			(oldcap) *= 32;               \
		} while ((oldcap) < (newcap));        \
	} while (0)

#define JSTD_REALLOC(p, oldcap, newcap, malloc_fail)        \
	do {                                                \
		JSTD_GROW(p, oldcap, newcap);               \
		(p) = JSTD_CAST(char *) realloc(p, newcap); \
		JSTD_MALLOC_ERR(p, malloc_fail);            \
	} while (0)

#define jstd_declare_string(s, sz, cap) \
	char *s;                        \
	size_t sz;                      \
	size_t cap

/*
  Return value:
  JSTD_RET_FAIL if malloc/realloc failure occurs;
  JSTD_RET_SUCCESS otherwise.
*/
JSTD_INLINE
void jstd_alloc(char **JSTD_RST s,
		const size_t newsz,
		size_t *JSTD_RST cap) JSTD_NOEX
{
	*s = JSTD_CAST(char *) malloc(newsz * 2);
	JSTD_MALLOC_ERR(*s, return);
	*cap = newsz * 2;
}

/*
  Return value:
  JSTD_RET_FAIL if malloc/realloc failure occurs
  JSTD_RET_SUCCESS otherwise
*/
JSTD_INLINE
void jstd_alloc_appendmem(char **JSTD_RST dst,
			  size_t *JSTD_RST dsz,
			  size_t *JSTD_RST dcap,
			  const char *JSTD_RST src,
			  const size_t ssz) JSTD_NOEX
{
	jstd_alloc(dst, ssz * 2, dcap);
	if (unlikely(!*dst))
		return;
	*dsz = ssz;
	memcpy(*dst, src, ssz + 1);
}

#define jstd_alloc_append(dst, sz, cap, src) \
	jstd_alloc_appendmem(dst, sz, cap, src, strlen(src))

JSTD_INLINE
void jstd_appendmemf(char **JSTD_RST dst,
		     size_t *JSTD_RST dsz,
		     const char *JSTD_RST src,
		     const size_t ssz) JSTD_NOEX
{
	memcpy(*dst, src, ssz + 1);
	*dsz += ssz;
}

#define jstd_appendf(dst, src, sz) jstd_appendmemf(dst, src, strlen(src), sz)

/*
  Return value:
  JSTD_RET_FAIL if malloc/realloc failure occurs
  JSTD_RET_SUCCESS otherwise
*/
JSTD_INLINE
void jstd_appendmem(char **JSTD_RST dst,
		    const char *JSTD_RST src,
		    const size_t srcsz,
		    size_t *JSTD_RST sz,
		    size_t *JSTD_RST cap) JSTD_NOEX
{
	if (*cap < *sz + srcsz)
		JSTD_REALLOC_SMALL(*dst, *cap, *sz + srcsz, return);
	jstd_appendmemf(dst, sz, src, srcsz);
	return;
}

#define jstd_append(dst, src, sz, cap) \
	jstd_appendmem(dst, src, strlen(src), sz, cap)

/*
  Return value:
  Pointer to '\0' in DST.
*/
JSTD_INLINE
JSTD_WARN_UNUSED
char *jstd_stpcat(char *JSTD_RST dst, const char *JSTD_RST src) JSTD_NOEX
{
	dst += strlen(dst);
	return jstd_stpcpy(dst, src);
}

JSTD_INLINE
JSTD_CONST
JSTD_WARN_UNUSED
int jstd_arealnum(const char *JSTD_RST s) JSTD_NOEX
{
	for (;; ++s)
		switch (*s) {
		default:
			return 0;
		case '\0':
			return 1;
			JSTD_CASE_ALPHANUM;
		}
}

JSTD_INLINE
JSTD_CONST
JSTD_WARN_UNUSED
int jstd_arealpha(const char *JSTD_RST s) JSTD_NOEX
{
	for (;; ++s)
		switch (*s) {
		default:
			return 0;
		case '\0':
			return 1;
			JSTD_CASE_ALPHA;
		}
}

JSTD_INLINE
JSTD_CONST
JSTD_WARN_UNUSED
int jstd_aredigits(const char *JSTD_RST s) JSTD_NOEX
{
	for (;; ++s)
		switch (*s) {
		default:
			return 0;
		case '\0':
			return 1;
			JSTD_CASE_DIGIT;
		}
}

JSTD_INLINE
JSTD_CONST
JSTD_WARN_UNUSED
int jstd_toupper(const int c) JSTD_NOEX
{
	switch (c) {
		JSTD_CASE_LOWER
		return c - 'a' + 'A';
	}
	return c;
}

JSTD_INLINE
JSTD_CONST
JSTD_WARN_UNUSED
int jstd_tolower(const int c) JSTD_NOEX
{
	switch (c) {
		JSTD_CASE_UPPER
		return c - 'A' + 'a';
	}
	return c;
}

JSTD_INLINE
void jstd_touppers(char *JSTD_RST s) JSTD_NOEX
{
	for (;; ++s) {
		switch (*s) {
			JSTD_CASE_LOWER
			*s = *s - 'a' + 'A';
		default:
			continue;
		case '\0':;
		}
		break;
	}
}

JSTD_INLINE
void jstd_tolowers(char *JSTD_RST s) JSTD_NOEX
{
	for (;; ++s) {
		switch (*s) {
			JSTD_CASE_UPPER
			*s = *s - 'A' + 'a';
		default:
			continue;
		case '\0':;
		}
		break;
	}
}

JSTD_INLINE
void jstd_capitalize(char *JSTD_RST const s) JSTD_NOEX
{
	*s = jstd_toupper(*s);
}

JSTD_INLINE
void jstd_uncapitalize(char *JSTD_RST const s) JSTD_NOEX
{
	*s = jstd_tolower(*s);
}

/*
  Count occurences of C in S.
  Return value:
  Occurences of C in S.
*/
JSTD_INLINE
JSTD_CONST
JSTD_WARN_UNUSED
int jstd_countc(const char *JSTD_RST s, const int c) JSTD_NOEX
{
	int count = 0;
	while (*s)
		if (*s == c)
			++count;
	return count;
}

#ifndef JSTD_HAS_MEMRCHR

/*
  Return pointer to last C in S.
  Return value:
  pointer to last C;
  NULL if not found.
*/
JSTD_INLINE
JSTD_CONST
JSTD_WARN_UNUSED
char *jstd_memrchr(char *JSTD_RST s, const int c, size_t n) JSTD_NOEX
{
	if (unlikely(!n))
		return NULL;
	char *JSTD_RST end = s + n - 1;
	do
		if (*end == c)
			return end;
	while (--end <= s);
	return NULL;
}

#else

#	define jstd_memrchr(s, c, n) memrchr(s, c, n)

#endif // ! JSTD_HAS_MEMRCHR

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTD_INLINE
JSTD_CONST
JSTD_WARN_UNUSED
#ifdef JSTD_HAS_MEMMEM

int jstd_counts(const char *JSTD_RST hs,
		size_t hslen,
		const char *JSTD_RST ne,
		size_t nelen) JSTD_NOEX
{
	int count = 0;
	for (const char *JSTD_RST old = hs;
	     (hs = JSTD_CAST(char *) memmem(hs, hslen, ne, nelen));
	     hslen -= (hs - old), ++count)
		;
	return count;
}

#else

int jstd_counts(const char *JSTD_RST hs, const char *JSTD_RST ne) JSTD_NOEX
{
	int count = 0;
	while ((hs = JSTD_CAST(char *) strstr(hs, ne)))
		++count;
	return count;
}

#endif // __JSTD_HAS_MEMMEM

/*
  Compare S1 with S2 case-insensitively.
  Return value:
  0 if strings match;
  non-zero otherwise.
*/
JSTD_INLINE
JSTD_CONST
JSTD_WARN_UNUSED
int jstd_casecmp(const char *JSTD_RST s1, const char *JSTD_RST s2) JSTD_NOEX
{
	for (char c;; ++s1, ++s2) {
		switch (*s1) {
		default:
			JSTD_CASE_LOWER
			c = *s1;
			break;
			JSTD_CASE_UPPER
			c = *s1 - 'A' + 'a';
			break;
		case '\0':
			return *s2;
		}
		switch (*s2) {
		default:
			JSTD_CASE_LOWER
			if (*s2 != c)
				return 1;
			break;
			JSTD_CASE_UPPER
			if ((*s2 - 'A' + 'a') != c)
				return 1;
			break;
		case '\0':
			return 1;
		}
	}
}

/*
  Reverse S.
*/
JSTD_INLINE
void jstd_revmem(char *JSTD_RST s, size_t n) JSTD_NOEX
{
	if (unlikely(!n))
		return;
	char *JSTD_RST end = s + n - 1;
	char tmp;
	do {
		tmp = *s;
		*s++ = *end;
		*end-- = tmp;
	} while (s < end);
}

#define jstd_revstr(s) jstd_revmem(s, strlen(s))

JSTD_INLINE
void jstd_swap(char **JSTD_RST s1, char **JSTD_RST s2) JSTD_NOEX
{
	char *JSTD_RST const tmp = *s1;
	*s1 = *s2;
	*s2 = tmp;
}

/*
  Remove all C in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTD_INLINE
char *jstd_stripp(char *JSTD_RST s, const int c) JSTD_NOEX
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
JSTD_INLINE
char *jstd_stripspnp(char *JSTD_RST s, const char *JSTD_RST reject) JSTD_NOEX
{
	if (unlikely(reject[0] == '\0'))
		return NULL;
	if (unlikely(reject[1] == '\0'))
		return jstd_stripp(s, *reject);
	enum { ACCEPT = 0,
	       REJECT = 1,
	       NUL = 2 };
	unsigned char table[256];
	memset(table, ACCEPT, 64);
	memset(&table[64], ACCEPT, 64);
	memset(&table[128], ACCEPT, 64);
	memset(&table[192], ACCEPT, 64);
	table[0] = NUL;
	do
		table[(unsigned char)*reject++] = REJECT;
	while (*reject);
	for (const unsigned char *JSTD_RST src = (unsigned char *)s;; ++src) {
		switch (table[*src]) {
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
JSTD_INLINE
char *jstd_trimpmem(char *JSTD_RST s, size_t n) JSTD_NOEX
{
	if (unlikely(!n))
		return NULL;
	char *JSTD_RST end = s + n - 1;
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

#define jstd_trimp(s) jstd_trimpmem(s, strlen(s))

/*
  Replace first SEARCH in REPLACE.
*/
JSTD_INLINE
void jstd_replacec(char *JSTD_RST s,
		   const char search,
		   const char replace) JSTD_NOEX
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
JSTD_INLINE
void jstd_replacecall(char *JSTD_RST s,
		      const char search,
		      const char replace) JSTD_NOEX
{
	for (;; ++s) {
		if (*s == search)
			*s = replace;
		if (unlikely(*s == '\0'))
			break;
	}
}

#include <math.h>

/*
  Replace first SEARCH in S with REPLACE.
  Return value:
  JSTD_RET_FAIL if malloc/realloc failure occurs;
  JSTD_RET_SUCCESS otherwise.
*/
JSTD_INLINE
void jstd_replacemem(char **JSTD_RST s,
		     size_t *JSTD_RST ssz,
		     size_t *JSTD_RST scap,
		     const char *JSTD_RST search,
		     const size_t slen,
		     const char *JSTD_RST replace,
		     const size_t rlen) JSTD_NOEX
{
	char *mtc;
	char *tmp;
#ifdef JSTD_HAS_MEMMEM
	if ((mtc = JSTD_CAST(char *) memmem(*s, *ssz, search, slen)))
#else
	if ((mtc = JSTD_CAST(char *) strstr(*s, search)))
#endif
#define JSTD_REPLACE(update_mtc, malloc_fail)                         \
	do {                                                          \
		if (rlen <= slen || *scap > *ssz + rlen - slen + 1) { \
			memmove(mtc + rlen,                           \
				mtc + slen,                           \
				(*s + *ssz + 1) - mtc + slen);        \
			memcpy(mtc, replace, rlen);                   \
			if (update_mtc)                               \
				mtc += rlen;                          \
		} else {                                              \
			JSTD_GROW(*scap, *ssz + rlen + 1);            \
			tmp = JSTD_CAST(char *) malloc(*scap);        \
			JSTD_MALLOC_ERR(tmp, malloc_fail);            \
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
		JSTD_REPLACE(0, return);
}

#define jstd_replace(s, ssz, cap, search, replace) jstd_replacemem(s, ssz, cap, search, strlen(search), replace, strlen(replace))

/*
  Replace all SEARCH in S with REPLACE.
  Return value:
  JSTD_RET_FAIL if malloc/realloc failure occurs;
  JSTD_RET_SUCCESS otherwise.
*/
JSTD_INLINE
void jstd_replaceallmem(char **JSTD_RST s,
			size_t *JSTD_RST ssz,
			size_t *JSTD_RST scap,
			const char *JSTD_RST search,
			const size_t slen,
			const char *JSTD_RST replace,
			const size_t rlen) JSTD_NOEX
{
	char *mtc = *s;
	char *tmp;
#ifdef JSTD_HAS_MEMMEM
	while ((mtc = JSTD_CAST(char *) memmem(mtc, (*s + *ssz) - mtc, search, slen)))
#else
	while ((mtc = JSTD_CAST(char *) strstr(mtc, search)))
#endif
		JSTD_REPLACE(1, return);
}

#define jstd_replaceall(s, ssz, scap, search, replace) jstd_replaceallmem(s, ssz, scap, search, strlen(search), replace, strlen(replace))

#undef JSTD_REPLACE

/*
  Insert SRC into DST[AT].
  Return value:
  JSTD_RET_FAIL if malloc/realloc failure occurs;
  JSTD_RET_SUCCESS otherwise.
*/
JSTD_INLINE
void jstd_insertmem(char **JSTD_RST dst,
		    size_t *JSTD_RST dsz,
		    size_t *JSTD_RST dcap,
		    size_t at,
		    char *JSTD_RST src,
		    size_t ssz) JSTD_NOEX
{
	if (*dcap > *dsz + ssz + 1) {
		memmove(*dst + at + ssz,
			*dst + at,
			*dsz - at + 1);
		memcpy(*dst + at, src, ssz);
	} else {
		JSTD_GROW(*dcap, *dsz + ssz + 1);
		char *const tmp = JSTD_CAST(char *) malloc(*dcap);
		JSTD_MALLOC_ERR(tmp, return);
		memcpy(tmp, *dst, at);
		memcpy(tmp + at, src, ssz);
		memcpy(tmp + at + ssz, *dst + at, *dsz - at + 1);
		free(*dst);
		*dst = tmp;
		*dsz += ssz;
	}
}

#define jstd_insert(dst, dsz, dcap, at, src) \
	jstd_insertmem(dst, dsz, dcap, at, src, strlen(src))

#ifdef __cplusplus
}
#endif // __cplusplus

#undef JSTD_NOEX
#undef JSTD_RST

#endif // JSTD_H_DEF
