#ifndef JSTR_TYPES_H_DEF
#define JSTR_TYPES_H_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

#include <stddef.h>

#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "jstr_macros.h"

#define JSTR_EXTERN_C  1
#define JSTR_NAMESPACE 0

#if JSTR_NAMESPACE && !JSTR_IN_NAMESPACE && defined(__cplusplus)
namespace jstr {
#endif /* JSTR_NAMESPACE */
#if JSTR_EXTERN_C && defined(__cplusplus)
extern "C" {
#define JSTR_IN_NAMESPACE 1
#endif /* JSTR_EXTERN_C */

#define JSTR_RST  JSTR_RESTRICT

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_isalnum(const int c) JSTR_NOEXCEPT
{
	switch (c) {
		JSTR_CASE_ALPHANUM;
		return 1;
	default:
		return 0;
	}
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_arealnum(const char *JSTR_RST s) JSTR_NOEXCEPT
{
	for (;;)
		switch (*s++) {
		default:
			return 0;
		case '\0':
			return 1;
			JSTR_CASE_ALPHANUM;
		}
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_isalpha(const int c) JSTR_NOEXCEPT
{
	switch (c) {
		JSTR_CASE_ALPHA;
		return 1;
	default:
		return 0;
	}
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_arealpha(const char *JSTR_RST s) JSTR_NOEXCEPT
{
	for (;;)
		switch (*s++) {
		default:
			return 0;
		case '\0':
			return 1;
			JSTR_CASE_ALPHA;
		}
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_isdigit(const int c) JSTR_NOEXCEPT
{
	switch (c) {
		JSTR_CASE_DIGIT;
		return 1;
	}
	return 0;
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_aredigits(const char *JSTR_RST s) JSTR_NOEXCEPT
{
	for (;;)
		switch (*s++) {
		default:
			return 0;
		case '\0':
			return 1;
			JSTR_CASE_DIGIT;
		}
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_toupper(const int c) JSTR_NOEXCEPT
{
	switch (c) {
		JSTR_CASE_LOWER
		return c - 'a' + 'A';
	}
	return c;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_touppers(char *JSTR_RST s) JSTR_NOEXCEPT
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
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_tolower(const int c) JSTR_NOEXCEPT
{
	switch (c) {
		JSTR_CASE_UPPER
		return c - 'A' + 'a';
	}
	return c;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_tolowers(char *JSTR_RST s) JSTR_NOEXCEPT
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

#if JSTR_EXTERN_C && defined(__cplusplus)
}
#endif /* JSTR_EXTERN_C */
#if JSTR_NAMESPACE && !JSTR_IN_NAMESPACE && defined(__cplusplus)
}
#endif /* JSTR_NAMESPACE */

#undef JSTR_RST

#endif /* JSTR_TYPES_H_DEF */
