#ifndef JSTR_CTYPE_H_DEF
#define JSTR_CTYPE_H_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "_macros.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_isupper(const int _c) JSTR_NOEXCEPT
{
	switch (_c) {
		JSTR_CASE_LOWER
		return 1;
	default:
		return 0;
	}
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_islower(const int _c) JSTR_NOEXCEPT
{
	switch (_c) {
		JSTR_CASE_LOWER
		return 1;
	default:
		return 0;
	}
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_isalnum(const int _c) JSTR_NOEXCEPT
{
	switch (_c) {
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
JSTR_NOTHROW
static int
jstr_arealnum(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	for (;;)
		switch (*_s++) {
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
JSTR_NOTHROW
static int
jstr_isalpha(const int _c) JSTR_NOEXCEPT
{
	switch (_c) {
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
JSTR_NOTHROW
static int
jstr_arealpha(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	for (;;)
		switch (*_s++) {
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
JSTR_NOTHROW
static int
jstr_arelower(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	for (;;)
		switch (*_s++) {
		default:
			return 0;
		case '\0':
			return 1;
			JSTR_CASE_LOWER;
		}
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_areupper(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	for (;;)
		switch (*_s++) {
		default:
			return 0;
		case '\0':
			return 1;
			JSTR_CASE_UPPER;
		}
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_isdigit(const int _c) JSTR_NOEXCEPT
{
	switch (_c) {
		JSTR_CASE_DIGIT;
		return 1;
	}
	return 0;
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_NOTHROW
JSTR_WARN_UNUSED
static int
jstr_isspace(const int _c) JSTR_NOEXCEPT
{
	if (_c == ' ' || (unsigned char)(_c - '\t') <= '\r' - '\t')
		return 1;
	return 0;
}

JSTR_INLINE
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_NOTHROW
JSTR_WARN_UNUSED
static int
jstr_arespace(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	return *(_s + strspn(_s, "\t\n\v\f\r ")) == '\0';
}

JSTR_INLINE
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_NOTHROW
JSTR_WARN_UNUSED
static int
jstr_areblank(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	return *(_s + strspn(_s, " \t")) == '\0';
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_aredigit(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	return *(_s + strspn(_s, "0123456789")) == '\0';
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_toupper(const int _c) JSTR_NOEXCEPT
{
	switch (_c) {
		JSTR_CASE_LOWER
		return _c - 'a' + 'A';
	}
	return _c;
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_touppers(char *JSTR_RST _s) JSTR_NOEXCEPT
{
	for (;; ++_s) {
		switch (*_s) {
			JSTR_CASE_LOWER
			*_s -= 'a' + 'A';
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
JSTR_NOTHROW
static int
jstr_tolower(const int _c) JSTR_NOEXCEPT
{
	switch (_c) {
		JSTR_CASE_UPPER
		return _c - 'A' + 'a';
	}
	return _c;
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_tolowers(char *JSTR_RST _s) JSTR_NOEXCEPT
{
	for (;; ++_s) {
		switch (*_s) {
			JSTR_CASE_UPPER
			*_s -= 'A' + 'a';
		default:
			continue;
		case '\0':;
		}
		break;
	}
}

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#endif /* JSTR_CTYPE_H_DEF */
