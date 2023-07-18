#ifndef JSTR_TYPES_H_DEF
#define JSTR_TYPES_H_DEF

#include <stddef.h>
#include "jstr_macros.h"

#define JSTR_RST JSTR_RESTRICT
#define JSTR_NOEX JSTR_NOEXCEPT

JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
static int jstr_isalnum(const int c) JSTR_NOEX
{
	switch (c) {
		JSTR_CASE_ALPHANUM;
		return 1;
	}
	return 0;
}

JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
static int jstr_arealnum(const char *JSTR_RST s) JSTR_NOEX
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
static int jstr_isalpha(const int c) JSTR_NOEX
{
	switch (c) {
		JSTR_CASE_ALPHA;
		return 1;
	}
	return 0;
}

JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
static int jstr_arealpha(const char *JSTR_RST s) JSTR_NOEX
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
static int jstr_isdigit(const int c) JSTR_NOEX
{
	switch (c) {
		JSTR_CASE_DIGIT;
		return 1;
	}
	return 0;
}

JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
static int jstr_aredigits(const char *JSTR_RST s) JSTR_NOEX
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
static int jstr_toupper(const int c) JSTR_NOEX
{
	switch (c) {
		JSTR_CASE_LOWER
		return c - 'a' + 'A';
	}
	return c;
}

JSTR_INLINE
static void jstr_touppers(char *JSTR_RST s) JSTR_NOEX
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
JSTR_WARN_UNUSED
static int jstr_tolower(const int c) JSTR_NOEX
{
	switch (c) {
		JSTR_CASE_UPPER
		return c - 'A' + 'a';
	}
	return c;
}

JSTR_INLINE
static void jstr_tolowers(char *JSTR_RST s) JSTR_NOEX
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
static void jstr_capitalize(char *JSTR_RST const s) JSTR_NOEX
{
	*s = jstr_toupper(*s);
}

JSTR_INLINE
static void jstr_uncapitalize(char *JSTR_RST const s) JSTR_NOEX
{
	*s = jstr_tolower(*s);
}

#undef JSTR_RST
#undef JSTR_NOEX

#endif /* JSTR_TYPES_H_DEF */
