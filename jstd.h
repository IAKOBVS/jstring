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
#endif

#ifndef __FILE__
#	define __FILE__
#endif

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
char *jstd_stpcpy(char *JSTD_RESTRICT dst, const char *JSTD_RESTRICT src) JSTD_NOEXCEPT
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

#define JSTD_GROW(p, oldcap, newcap)                  \
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

enum {
	JSTD_RET_FAIL = -1,
	JSTD_RET_SUCCESS = 0,
};

/*
	Return value:

	JSTD_RET_FAIL if malloc/realloc failure occurs;

	JSTD_RET_SUCCESS otherwise.
*/
JSTD_INLINE
int jstd_alloc(char **JSTD_RESTRICT s,
	       const size_t newsz,
	       size_t *JSTD_RESTRICT sz,
	       size_t *JSTD_RESTRICT cap)
{
	*s = JSTD_CAST(char *) malloc(newsz * 2);
	JSTD_MALLOC_ERR(*s, return JSTD_RET_FAIL);
	*cap = newsz * 2;
	*sz = 0;
	return JSTD_RET_SUCCESS;
}

/*
	Return value:

	JSTD_RET_FAIL if malloc/realloc failure occurs

	JSTD_RET_SUCCESS otherwise
*/
JSTD_INLINE
int jstd_alloc_appendmem(char **JSTD_RESTRICT s,
			 const char *JSTD_RESTRICT src,
			 const size_t srcsz,
			 size_t *JSTD_RESTRICT sz,
			 size_t *JSTD_RESTRICT cap)
{
	jstd_alloc(s, srcsz * 2, sz, cap);
	if (unlikely(!*s))
		return JSTD_RET_FAIL;
	memcpy(*s, src, srcsz + 1);
	return JSTD_RET_SUCCESS;
}

#define jstd_alloc_append(s, src, sz, cap) jstd_alloc_appendmem(s, src, strlen(src), sz, cap);

JSTD_INLINE
void jstd_appendmemf(char **JSTD_RESTRICT s,
		     const char *JSTD_RESTRICT src,
		     const size_t srcsz,
		     size_t *JSTD_RESTRICT sz)
{
	memcpy(*s, src, srcsz + 1);
	*sz += srcsz;
}

#define jstd_appendf(s, src, sz) jstd_appendmemf(s, src, strlen(src), sz)

/*
	Return value:

	JSTD_RET_FAIL if malloc/realloc failure occurs

	JSTD_RET_SUCCESS otherwise
*/
JSTD_INLINE
int jstd_appendmem(char **JSTD_RESTRICT s,
		   const char *JSTD_RESTRICT src,
		   const size_t srcsz,
		   size_t *JSTD_RESTRICT sz,
		   size_t *JSTD_RESTRICT cap)
{
	if (*cap < *sz + srcsz)
		JSTD_REALLOC(*s, *cap, *sz + srcsz, return JSTD_RET_FAIL);
	jstd_appendmemf(s, src, srcsz, sz);
	return JSTD_RET_SUCCESS;
}

#define jstd_append(s, src, sz, cap) jstd_appendmem(s, src, strlen(src), sz, cap)

/*
	Return value:

	Pointer to '\0' in DST.
*/
JSTD_INLINE
JSTD_WARN_UNUSED
char *jstd_stpcat(char *JSTD_RESTRICT dst, const char *JSTD_RESTRICT src) JSTD_NOEXCEPT
{
	dst += strlen(dst);
	return jstd_stpcpy(dst, src);
}

JSTD_INLINE
JSTD_CONST
JSTD_WARN_UNUSED
int jstd_arealnum(const char *JSTD_RESTRICT s) JSTD_NOEXCEPT
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
int jstd_arealpha(const char *JSTD_RESTRICT s) JSTD_NOEXCEPT
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
int jstd_aredigits(const char *JSTD_RESTRICT s) JSTD_NOEXCEPT
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
int jstd_toupper(const int c) JSTD_NOEXCEPT
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
int jstd_tolower(const int c) JSTD_NOEXCEPT
{
	switch (c) {
		JSTD_CASE_UPPER
		return c - 'A' + 'a';
	}
	return c;
}

JSTD_INLINE
void jstd_touppers(char *JSTD_RESTRICT s) JSTD_NOEXCEPT
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
void jstd_tolowers(char *JSTD_RESTRICT s) JSTD_NOEXCEPT
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
void jstd_capitalize(char *JSTD_RESTRICT const s) JSTD_NOEXCEPT
{
	*s = jstd_toupper(*s);
}

JSTD_INLINE
void jstd_uncapitalize(char *JSTD_RESTRICT const s) JSTD_NOEXCEPT
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
int jstd_countc(const char *JSTD_RESTRICT s, const int c) JSTD_NOEXCEPT
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
char *jstd_memrchr(char *JSTD_RESTRICT s, const int c, size_t n) JSTD_NOEXCEPT
{
	if (unlikely(!n))
		return NULL;
	char *JSTD_RESTRICT end = s + n - 1;
	do
		if (*end == c)
			return end;
	while (--end <= s);
	return NULL;
}

#else

#	define jstd_memrchr(s, c, n) \
		memrchr(s, c, n)

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

int jstd_counts(const char *JSTD_RESTRICT hs,
		size_t hslen,
		const char *JSTD_RESTRICT ne,
		size_t nelen) JSTD_NOEXCEPT
{
	int count = 0;
	for (const char *JSTD_RESTRICT old = hs;
	     (hs = JSTD_CAST(char *) memmem(hs, hslen, ne, nelen));
	     hslen -= (hs - old), ++count)
		;
	return count;
}

#else

int jstd_counts(const char *JSTD_RESTRICT hs,
		const char *JSTD_RESTRICT ne) JSTD_NOEXCEPT
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
int jstd_casecmp(const char *JSTD_RESTRICT s1, const char *JSTD_RESTRICT s2) JSTD_NOEXCEPT
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
void jstd_revmem(char *JSTD_RESTRICT s, size_t slen) JSTD_NOEXCEPT
{
	if (unlikely(!slen))
		return;
	char *JSTD_RESTRICT end = s + slen - 1;
	char tmp;
	do {
		tmp = *s;
		*s++ = *end;
		*end-- = tmp;
	} while (s < end);
}

#define jstd_revstr(s) \
	jstd_revmem(s, strlen(s))

JSTD_INLINE
void jstd_swap(char **JSTD_RESTRICT s1, char **JSTD_RESTRICT s2) JSTD_NOEXCEPT
{
	char *JSTD_RESTRICT const tmp = *s1;
	*s1 = *s2;
	*s2 = tmp;
}

/*
   	Remove all C in S.

	Return value:

	Pointer to '\0' in S.
*/
JSTD_INLINE
char *jstd_stripp(char *JSTD_RESTRICT s, const int c)
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
char *jstd_stripspnp(char *JSTD_RESTRICT s, const char *JSTD_RESTRICT reject)
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
	for (const unsigned char *JSTD_RESTRICT src = (unsigned char *)s;;
	     ++src) {
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
char *jstd_trimpmem(char *JSTD_RESTRICT s, size_t slen) JSTD_NOEXCEPT
{
	if (unlikely(!slen))
		return NULL;
	char *JSTD_RESTRICT end = s + slen - 1;
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

#define jstd_trimp(s) \
	jstd_trimpmem(s, strlen(s))

/*
	Replace first SEARCH in REPLACE.
*/
JSTD_INLINE
void jstd_replacec(char *JSTD_RESTRICT s,
		   const char search,
		   const char replace) JSTD_NOEXCEPT
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
void jstd_replacecall(char *JSTD_RESTRICT s,
		      const char search,
		      const char replace) JSTD_NOEXCEPT
{
	for (;; ++s) {
		if (*s == search)
			*s = replace;
		if (unlikely(*s == '\0'))
			break;
	}
}

#define JSTD_REPLACE(malloc_fail)                                              \
	do {                                                                   \
		if (*scap < *ssz + rlen) {                                     \
			JSTD_REALLOC(*s, *scap, *ssz + rlen, malloc_fail);     \
			mtc = *s + mtcsz;                                      \
		}                                                              \
		memmove(mtc + rlen, mtc + slen, (*s + *ssz + 1) - mtc + slen); \
		memcpy(mtc, replace, rlen);                                    \
		*ssz += (rlen - slen);                                         \
	} while (0)

/*
	Replace first SEARCH in S with REPLACE.

	Return value:

	JSTD_RET_FAIL if malloc/realloc failure occurs;

	JSTD_RET_SUCCESS otherwise.
*/
JSTD_INLINE
int jstd_replace(char **JSTD_RESTRICT s,
		 const char *JSTD_RESTRICT search,
		 const char *JSTD_RESTRICT replace,
		 size_t *JSTD_RESTRICT ssz,
		 size_t *JSTD_RESTRICT scap) JSTD_NOEXCEPT
{
	char *mtc;
	const size_t slen = strlen(search);
	const size_t rlen = strlen(replace);
	if ((mtc = JSTD_CAST(char *)
#ifdef JSTD_HAS_MEMMEM
	     memmem(*s, *ssz, search, slen)
#else
	     strstr(*s, search)
#endif
	     )) {
		const size_t mtcsz = mtc - *s;
		JSTD_REPLACE(return JSTD_RET_FAIL);
	}
	return JSTD_RET_SUCCESS;
}

/*
   	Replace all SEARCH in S with REPLACE.

	Return value:

	JSTD_RET_FAIL if malloc/realloc failure occurs;

	JSTD_RET_SUCCESS otherwise.
*/
JSTD_INLINE
int jstd_replaceall(char **JSTD_RESTRICT s,
		    const char *JSTD_RESTRICT search,
		    const char *JSTD_RESTRICT replace,
		    size_t *JSTD_RESTRICT ssz,
		    size_t *JSTD_RESTRICT scap) JSTD_NOEXCEPT
{
	char *mtc;
	const size_t slen = strlen(search);
	const size_t rlen = strlen(replace);
	for (size_t mtcsz; (mtc = JSTD_CAST(char *)
#ifdef JSTD_HAS_MEMMEM
			    memmem(*s, *ssz, search, slen)
#else
			    strstr(*s, search)
#endif
	     );) {
		mtcsz = mtc - *s;
		JSTD_REPLACE(return JSTD_RET_FAIL);
	}
	return JSTD_RET_SUCCESS;
}

#undef JSTD_REPLACE

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // JSTD_H_DEF
