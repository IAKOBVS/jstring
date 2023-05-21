#ifndef JSTD_DEF_H__
#define JSTD_DEF_H__

#ifdef __cplusplus
#	include <cstddef>
#	include <cstring>
extern "C" {
#else
#	 include <stddef.h>
#	 include <string.h>
#endif // __cplusplus

#ifdef __cplusplus
}
#endif // __cpluslus

#include "macros.h"

#ifdef __cplusplus
extern "C" {
#endif // __cpluslus
		
#ifdef JSTD_HAS_STPCPY
#	define jstd_stpcpy(dst, src) stpcpy(dst, src)
#else
JSTR_INLINE__
JSTR_WARN_UNUSED__
char *jstd_stpcpy(char *JSTR_RESTRICT__ dst, const char *JSTR_RESTRICT__ src) JSTR_NOEXCEPT__
{
	const size_t slen = strlen(src);
	memcpy(dst, src, slen + 1);
	return dst + slen;
}
#endif // JSTD_HAS_STPCPY

JSTR_INLINE__
JSTR_WARN_UNUSED__
char *jstd_stpcat(char *JSTR_RESTRICT__ dst, const char *JSTR_RESTRICT__ src) JSTR_NOEXCEPT__
{
	dst += strlen(dst);
	return jstd_stpcpy(dst, src);
}
	
JSTR_INLINE__
JSTR_CONST__
JSTR_WARN_UNUSED__
int jstd_arealnum(const char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__
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

JSTR_INLINE__
JSTR_CONST__
JSTR_WARN_UNUSED__
int jstd_arealpha(const char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__
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

JSTR_INLINE__
JSTR_CONST__
JSTR_WARN_UNUSED__
int jstd_aredigits(const char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__
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

JSTR_INLINE__
JSTR_CONST__
JSTR_WARN_UNUSED__
int jstd_toupper(const int c) JSTR_NOEXCEPT__
{
	switch (c) {
	JSTR_CASE_LOWER
		return c - 'a' + 'A';
	}
	return c;
}

JSTR_INLINE__
JSTR_CONST__
JSTR_WARN_UNUSED__
int jstd_tolower(const int c) JSTR_NOEXCEPT__
{
	switch (c) {
	JSTR_CASE_UPPER
		return c - 'A' + 'a';
	}
	return c;
}

JSTR_INLINE__
void jstd_toupper_s(char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__
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

JSTR_INLINE__
void jstd_tolower_s(char *JSTR_RESTRICT__ s) JSTR_NOEXCEPT__
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

JSTR_INLINE__
void jstd_capitalize(char *JSTR_RESTRICT__ const s) JSTR_NOEXCEPT__
{
	*s = jstd_toupper(*s);
}

JSTR_INLINE__
void jstd_uncapitalize(char *JSTR_RESTRICT__ const s) JSTR_NOEXCEPT__
{
	*s = jstd_tolower(*s);
}

JSTR_INLINE__
JSTR_CONST__
JSTR_WARN_UNUSED__
int jstd_count_c(const char *JSTR_RESTRICT__ s, const int c) JSTR_NOEXCEPT__
{
	int count = 0;
	while (*s)
		if (*s == c)
			++count;
	return count;
}

#ifndef JSTR_HAS_MEMRCHR__

/* memrchr */
JSTR_INLINE__
JSTR_CONST__
JSTR_WARN_UNUSED__
char *jstd_memrchr(char *s, const int c, size_t n) JSTR_NOEXCEPT__
{
	if (unlikely(!n))
		return NULL;
	char *JSTR_RESTRICT__ end = s + n - 1;
	do
		if (*end == c)
			return end;
	while (--end <= s);
	return NULL;
}

#else

#define jstd_memrchr(s, c, n) \
	memrchr(s, c, n)

#endif // ! JSTR_HAS_MEMRCHR__

JSTR_INLINE__
JSTR_CONST__
JSTR_WARN_UNUSED__
#ifdef JSTR_HAS_MEMMEM__

int jstd_count_s(const char *JSTR_RESTRICT__ haystack,
		size_t haystacklen,
		const char *JSTR_RESTRICT__ needle,
		size_t needlelen) JSTR_NOEXCEPT__
{
	int count = 0;
	for (const char *JSTR_RESTRICT__ old = haystack;
		(haystack = JSTR_CAST__(char *)memmem(haystack, haystacklen, needle, needlelen));
		haystacklen -= (haystack - old), ++count);
	return count;
}

#else

int jstd_count_s(const char *JSTR_RESTRICT__ haystack,
		const char *JSTR_RESTRICT__ needle) JSTR_NOEXCEPT__
{
	int count = 0;
	while ((haystack = JSTR_CAST__(char *)strstr(haystack, needle)))
		++count;
	return count;
}

#endif // __JSTR_HAS_MEMMEM__

JSTR_INLINE__
JSTR_CONST__
JSTR_WARN_UNUSED__
int jstd_casecmp(const char *JSTR_RESTRICT__ s1, const char *JSTR_RESTRICT__ s2) JSTR_NOEXCEPT__
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

JSTR_INLINE__
void jstd_memrev(char *JSTR_RESTRICT__ s, size_t slen) JSTR_NOEXCEPT__
{
	if (unlikely(!slen))
		return;
	char *JSTR_RESTRICT__ end = s + slen - 1;
	char tmp;
	do {
		tmp = *s;
		*s++ = *end;
		*end-- = tmp;
	} while (s < end);
}

#define jstd_strrev(s)            \
	jstd_memrev(s, strlen(s))

JSTR_INLINE__
void jstd_strswap(char **JSTR_RESTRICT__ s1, char **JSTR_RESTRICT__ s2) JSTR_NOEXCEPT__
{
	char *JSTR_RESTRICT__ const tmp = *s1;
	*s1 = *s2;
	*s2 = tmp;
}

JSTR_INLINE__
char *jstd_stpstrip(char *JSTR_RESTRICT__ s, const int c)
{
	for (const char *src = s;; ++src)
		if (*src != c)
			*s++ = *src;
		else if (unlikely(*src == '\0'))
			break;
	*s = '\0';
	return s;
}

JSTR_INLINE__
char *jstd_stpstripspn(char *JSTR_RESTRICT__ s, const char *JSTR_RESTRICT__ reject)
{
	if (unlikely(reject[0] == '\0'))
		return NULL;
	if (unlikely(reject[1] == '\0'))
		return jstd_stpstrip(s, *reject);
	enum { ACCEPT = 0, REJECT = 1, NUL = 2 };
	unsigned char table[256];
	memset(table, ACCEPT, 64);
	memset(&table[64], ACCEPT, 64);
	memset(&table[128], ACCEPT, 64);
	memset(&table[192], ACCEPT, 64);
	table[0] = NUL;
	do
		table[(unsigned char)*reject++] = REJECT;
	while (*reject);
	for (const unsigned char *JSTR_RESTRICT__ src = (unsigned char *)s;;
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

JSTR_INLINE__
char *jstd_memptrim(char *JSTR_RESTRICT__ s, size_t slen) JSTR_NOEXCEPT__
{
	if (unlikely(!slen))
		return NULL;
	char *JSTR_RESTRICT__ end = s + slen - 1;
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

#define jstd_stptrim(s)            \
	jstd_memptrim(s, strlen(s))

JSTR_INLINE__
char *jstd_strreplace_c(char *JSTR_RESTRICT__ s,
			const char search,
			const char replace) JSTR_NOEXCEPT__
{
	for ( ;; ++s)
		if (*s == search)
			*s = replace;
		else if (unlikely(*s == '\0'))
			break;
	return s;
}

JSTR_INLINE__
#ifdef JSTR_HAS_MEMMEM__

char *jstd_memreplace(char *JSTR_RESTRICT__ s,
			const char *JSTR_RESTRICT__ search,
			const char *JSTR_RESTRICT__ replace,
			size_t n) JSTR_NOEXCEPT__
{
	char *JSTR_RESTRICT__ mtc;
	size_t slen = strlen(search);
	while ((mtc = JSTR_CAST__(char *)memmem(s, n, search, slen))) {
		memcpy(mtc, replace, slen);
		n -= (mtc - s);
	}
	return mtc;
}

#define jstd_strreplace(s, search, replace)            \
	jstd_memreplace(s, search, replace, strlen(s))

#else

char *jstd_replace(char *JSTR_RESTRICT__ s,
			const char *JSTR_RESTRICT__ search,
			const char *JSTR_RESTRICT__ replace) JSTR_NOEXCEPT__
{
	size_t n = strlen(s);
	const char *JSTR_RESTRICT__ const end = s + n;
	char *JSTR_RESTRICT__ mtc;
	size_t slen = strlen(search);
	while ((mtc = JSTR_CAST__(char *)strstr(s, search))) {
		if (unlikely(end - mtc < slen))
			break;
		memcpy(mtc, replace, slen);
	}
	return mtc;
}

#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // JSTD_DEF_H_
