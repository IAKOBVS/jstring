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

#include "macros.h"

#ifndef __LINE__
#	define __LINE__
#endif

#ifndef __FILE__
#	define __FILE__
#endif

#define JSTR_MALLOC_ERR(p)                                                         \
	do {                                                                       \
		if (unlikely(!(p))) {                                              \
			fprintf(stderr, "%s:%d:Can't malloc", __FILE__, __LINE__); \
			exit(1);                                                   \
		}                                                                  \
	} while (0)

#define JSTR_GROW(p, oldsz, newsz)                  \
	do {                                        \
		do {                                \
			if ((oldsz)*2 > (newsz)) {  \
				(oldsz) *= 2;       \
				break;              \
			}                           \
			if ((oldsz)*4 > (newsz)) {  \
				(oldsz) *= 4;       \
				break;              \
			}                           \
			if ((oldsz)*16 > (newsz)) { \
				(oldsz) *= 16;      \
				break;              \
			}                           \
			(oldsz) *= 32;              \
		} while ((oldsz) < (newsz));        \
	} while (0)

#define JSTR_REALLOC(p, oldsz, newsz)                      \
	do {                                               \
		JSTR_GROW(p, oldsz, newsz);                \
		(p) = JSTR_CAST(char *) realloc(p, newsz); \
		JSTR_MALLOC_ERR(p);                        \
	} while (0)

#ifdef __cplusplus
extern "C" {
#endif // __cpluslus

#ifdef JSTD_HAS_STPCPY

#	define jstd_stpcpy(dst, src) stpcpy(dst, src)

#else

JSTR_INLINE
JSTR_WARN_UNUSED
char *jstd_stpcpy(char *JSTR_RESTRICT dst, const char *JSTR_RESTRICT src) JSTR_NOEXCEPT
{
	const size_t slen = strlen(src);
	memcpy(dst, src, slen + 1);
	return dst + slen;
}

#endif // JSTD_HAS_STPCPY

JSTR_INLINE
JSTR_WARN_UNUSED
char *jstd_stpcat(char *JSTR_RESTRICT dst, const char *JSTR_RESTRICT src) JSTR_NOEXCEPT
{
	dst += strlen(dst);
	return jstd_stpcpy(dst, src);
}

JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
int jstd_arealnum(const char *JSTR_RESTRICT s) JSTR_NOEXCEPT
{
	for (;; ++s)
		switch (*s) {
		default:
			return 0;
		case '\0':
			return 1;
			JSTR_CASE_ALPHANUM;
		}
}

JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
int jstd_arealpha(const char *JSTR_RESTRICT s) JSTR_NOEXCEPT
{
	for (;; ++s)
		switch (*s) {
		default:
			return 0;
		case '\0':
			return 1;
			JSTR_CASE_ALPHA;
		}
}

JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
int jstd_aredigits(const char *JSTR_RESTRICT s) JSTR_NOEXCEPT
{
	for (;; ++s)
		switch (*s) {
		default:
			return 0;
		case '\0':
			return 1;
			JSTR_CASE_DIGIT;
		}
}

JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
int jstd_toupper(const int c) JSTR_NOEXCEPT
{
	switch (c) {
		JSTR_CASE_LOWER
		return c - 'a' + 'A';
	}
	return c;
}

JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
int jstd_tolower(const int c) JSTR_NOEXCEPT
{
	switch (c) {
		JSTR_CASE_UPPER
		return c - 'A' + 'a';
	}
	return c;
}

JSTR_INLINE
void jstd_toupper_s(char *JSTR_RESTRICT s) JSTR_NOEXCEPT
{
	for (;; ++s) {
		switch (*s) {
			JSTR_CASE_LOWER
			*s = *s - 'a' + 'A';
		default:
			continue;
		case '\0':;
		}
		break;
	}
}

JSTR_INLINE
void jstd_tolower_s(char *JSTR_RESTRICT s) JSTR_NOEXCEPT
{
	for (;; ++s) {
		switch (*s) {
			JSTR_CASE_UPPER
			*s = *s - 'A' + 'a';
		default:
			continue;
		case '\0':;
		}
		break;
	}
}

JSTR_INLINE
void jstd_capitalize(char *JSTR_RESTRICT const s) JSTR_NOEXCEPT
{
	*s = jstd_toupper(*s);
}

JSTR_INLINE
void jstd_uncapitalize(char *JSTR_RESTRICT const s) JSTR_NOEXCEPT
{
	*s = jstd_tolower(*s);
}

JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
int jstd_count_c(const char *JSTR_RESTRICT s, const int c) JSTR_NOEXCEPT
{
	int count = 0;
	while (*s)
		if (*s == c)
			++count;
	return count;
}

#ifndef JSTR_HAS_MEMRCHR

/* memrchr */
JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
char *jstd_memrchr(char *JSTR_RESTRICT s, const int c, size_t n) JSTR_NOEXCEPT
{
	if (unlikely(!n))
		return NULL;
	char *JSTR_RESTRICT end = s + n - 1;
	do
		if (*end == c)
			return end;
	while (--end <= s);
	return NULL;
}

#else

#	define jstd_memrchr(s, c, n) \
		memrchr(s, c, n)

#endif // ! JSTR_HAS_MEMRCHR

JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
#ifdef JSTR_HAS_MEMMEM

int jstd_count_s(const char *JSTR_RESTRICT haystack,
		 size_t haystacklen,
		 const char *JSTR_RESTRICT needle,
		 size_t needlelen) JSTR_NOEXCEPT
{
	int count = 0;
	for (const char *JSTR_RESTRICT old = haystack;
	     (haystack = JSTR_CAST(char *) memmem(haystack, haystacklen, needle, needlelen));
	     haystacklen -= (haystack - old), ++count)
		;
	return count;
}

#else

int jstd_count_s(const char *JSTR_RESTRICT haystack,
		 const char *JSTR_RESTRICT needle) JSTR_NOEXCEPT
{
	int count = 0;
	while ((haystack = JSTR_CAST(char *) strstr(haystack, needle)))
		++count;
	return count;
}

#endif // __JSTR_HAS_MEMMEM

JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
int jstd_casecmp(const char *JSTR_RESTRICT s1, const char *JSTR_RESTRICT s2) JSTR_NOEXCEPT
{
	for (char c;; ++s1, ++s2) {
		switch (*s1) {
		default:
			JSTR_CASE_LOWER
			c = *s1;
			break;
			JSTR_CASE_UPPER
			c = *s1 - 'A' + 'a';
			break;
		case '\0':
			return *s2;
		}
		switch (*s2) {
		default:
			JSTR_CASE_LOWER
			if (*s2 != c)
				return 1;
			break;
			JSTR_CASE_UPPER
			if ((*s2 - 'A' + 'a') != c)
				return 1;
			break;
		case '\0':
			return 1;
		}
	}
}

JSTR_INLINE
void jstd_memrev(char *JSTR_RESTRICT s, size_t slen) JSTR_NOEXCEPT
{
	if (unlikely(!slen))
		return;
	char *JSTR_RESTRICT end = s + slen - 1;
	char tmp;
	do {
		tmp = *s;
		*s++ = *end;
		*end-- = tmp;
	} while (s < end);
}

#define jstd_strrev(s) \
	jstd_memrev(s, strlen(s))

JSTR_INLINE
void jstd_strswap(char **JSTR_RESTRICT s1, char **JSTR_RESTRICT s2) JSTR_NOEXCEPT
{
	char *JSTR_RESTRICT const tmp = *s1;
	*s1 = *s2;
	*s2 = tmp;
}

JSTR_INLINE
char *jstd_stpstrip(char *JSTR_RESTRICT s, const int c)
{
	for (const char *src = s;; ++src)
		if (*src != c)
			*s++ = *src;
		else if (unlikely(*src == '\0'))
			break;
	*s = '\0';
	return s;
}

JSTR_INLINE
char *jstd_stpstripspn(char *JSTR_RESTRICT s, const char *JSTR_RESTRICT reject)
{
	if (unlikely(reject[0] == '\0'))
		return NULL;
	if (unlikely(reject[1] == '\0'))
		return jstd_stpstrip(s, *reject);
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
	for (const unsigned char *JSTR_RESTRICT src = (unsigned char *)s;;
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

JSTR_INLINE
char *jstd_memptrim(char *JSTR_RESTRICT s, size_t slen) JSTR_NOEXCEPT
{
	if (unlikely(!slen))
		return NULL;
	char *JSTR_RESTRICT end = s + slen - 1;
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

#define jstd_stptrim(s) \
	jstd_memptrim(s, strlen(s))

JSTR_INLINE
char *jstd_strreplace_c(char *JSTR_RESTRICT s,
			const char search,
			const char replace) JSTR_NOEXCEPT
{
	for (;; ++s)
		if (*s == search)
			*s = replace;
		else if (unlikely(*s == '\0'))
			break;
	return s;
}

JSTR_INLINE
int jstd_memreplace(char **JSTR_RESTRICT s,
		    const char *JSTR_RESTRICT search,
		    const char *JSTR_RESTRICT replace,
		    size_t *ssz, size_t *scap) JSTR_NOEXCEPT
{
	char *mtc;
	const size_t slen = strlen(search);
	const size_t rlen = strlen(replace);
	if ((mtc = JSTR_CAST(char *)
#ifdef JSTR_HAS_MEMMEM
	     memmem(*s, *ssz, search, slen)
#else
	     strstr(*s, search)
#endif
	     )) {
		const size_t mtcsz = mtc - *s;
		if (*scap < *ssz + rlen) {
			JSTR_REALLOC(*s, *scap, *ssz + rlen);
			mtc = *s + mtcsz;
		}
		memmove(mtc + rlen, mtc + slen, (*s + *ssz + 1) - mtc + slen);
		memcpy(mtc, replace, rlen);
		*ssz += (rlen + slen);
	}
	return mtc ? 1 : 0;
}

JSTR_INLINE
unsigned int jstd_memreplaceall(char **JSTR_RESTRICT s,
				const char *JSTR_RESTRICT search,
				const char *JSTR_RESTRICT replace,
				size_t *ssz, size_t *scap) JSTR_NOEXCEPT
{
	char *mtc;
	const size_t slen = strlen(search);
	const size_t rlen = strlen(replace);
	unsigned int mtcd = 0;
	for (size_t mtcsz; (mtc = JSTR_CAST(char *)
#ifdef JSTR_HAS_MEMMEM
			    memmem(*s, *ssz, search, slen)
#else
			    strstr(*s, search)
#endif
	     );
	     ++mtcd) {
		mtcsz = mtc - *s;
		if (*scap < *ssz + rlen) {
			JSTR_REALLOC(*s, *scap, *ssz + rlen);
			mtc = *s + mtcsz;
		}
		memmove(mtc + rlen, mtc + slen, (*s + *ssz + 1) - mtc + slen);
		memcpy(mtc, replace, rlen);
		*ssz += (rlen + slen);
	}
	return mtcd;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // JSTD_H_DEF
