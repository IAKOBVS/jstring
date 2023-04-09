#ifndef JSTD_DEF_H__
#define JSTD_DEF_H__

#	 include "macros.h"

#ifdef __cplusplus
#	include <cstddef>
#	include <cstring>
extern "C" {
#else
#	 include <stddef.h>
#	 include <string.h>
#endif // __cplusplus

JSTR_INLINE__
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
JSTR_WARN_UNUSED__
JSTR_CONST__
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

JSTR_INLINE__
JSTR_CONST__
JSTR_WARN_UNUSED__
char *jstd_rchr(const char *s, const int c, size_t slen)
{
	const char *JSTR_RESTRICT__ const begin = s;
	char *JSTR_RESTRICT__ end = s + slen - 1;
	for ( ; end != begin; --end)
		if (*end == c)
			return end;
	return NULL;
}

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
		const char *JSTR_RESTRICT__ const needle) JSTR_NOEXCEPT__
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
int jstd_casecmp(const char *JSTR_RESTRICT__ s1,
		const char *JSTR_RESTRICT__ s2) JSTR_NOEXCEPT__
{
	for (char c;; ++s1, ++s2) {
		switch (*s1) {
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
		JSTR_CASE_LOWER
			if (*s2 != c)
				return 1;
			break;
		JSTR_CASE_UPPER
			if ((*s2 - 'A' + 'a') != c)
				return 1;
		}
	}
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // JSTD_DEF_H__
