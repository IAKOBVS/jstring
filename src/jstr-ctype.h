#ifndef JSTR_CTYPE_H
#define JSTR_CTYPE_H 1

#include "jstr-macros.h"

P_JSTR_BEGIN_DECLS
#include <string.h>
P_JSTR_END_DECLS

#include "jstr-ctype-table.h"
#include "jstr-macros.h"
#include "jstr-std-string.h"

#define R JSTR_RESTRICT

P_JSTR_BEGIN_DECLS

/*
   ASCII.
   Will NOT handle EOF correctly.
   toupper(EOF) != EOF;
*/
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_toupper(const int c) JSTR_NOEXCEPT
{
	return pjstr_table_toupper[(unsigned char)c];
}

/*
   ASCII.
   Will NOT handle EOF correctly.
   tolower(EOF) != EOF;
*/
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_tolower(const int c) JSTR_NOEXCEPT
{
	return pjstr_table_tolower[(unsigned char)c];
}

#define P_JSTR_IS_CTYPE(ctype, ctype_enum)                               \
	/* ASCII */                                                      \
	JSTR_INLINE                                                      \
	JSTR_FUNC_CONST                                                  \
	static int                                                       \
	jstr_##ctype(const int c) JSTR_NOEXCEPT                          \
	{                                                                \
		return pjstr_table_ctype[(unsigned char)c] & ctype_enum; \
	}

P_JSTR_IS_CTYPE(isalpha, JSTR_ISALPHA)
P_JSTR_IS_CTYPE(islower, JSTR_ISLOWER)
P_JSTR_IS_CTYPE(isupper, JSTR_ISUPPER)
P_JSTR_IS_CTYPE(isspace, JSTR_ISSPACE)
P_JSTR_IS_CTYPE(isblank, JSTR_ISBLANK)
P_JSTR_IS_CTYPE(isdigit, JSTR_ISDIGIT)
P_JSTR_IS_CTYPE(isxdigit, JSTR_ISXDIGIT)
P_JSTR_IS_CTYPE(ispunct, JSTR_ISPUNCT)
P_JSTR_IS_CTYPE(isword, JSTR_ISWORD)

#undef P_JSTR_IS_CTYPE

/*
   ASCII.
   For example: jstr_isctype(C, JSTR_ISLOWER | JSTR_ISDIGIT) checks if C is either lowercase or a number.
*/
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isctype(const int c, jstr_ctype_ty type) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)c] & type;
}

#define P_JSTR_IS_CTYPE_STR(ctype)                        \
	/* ASCII */                                       \
	JSTR_INLINE                                       \
	JSTR_FUNC_PURE                                    \
	static int                                        \
	jstr_##ctype##_str(const char *R s) JSTR_NOEXCEPT \
	{                                                 \
		if (jstr_unlikely(*s == '\0'))            \
			return 0;                         \
		while (jstr_##ctype(s[0])) {              \
			if (!jstr_##ctype(s[1])) {        \
				s += 1;                   \
				break;                    \
			}                                 \
			if (!jstr_##ctype(s[2])) {        \
				s += 2;                   \
				break;                    \
			}                                 \
			if (!jstr_##ctype(s[3])) {        \
				s += 3;                   \
				break;                    \
			}                                 \
			s += 4;                           \
		}                                         \
		return *s == '\0';                        \
	}

P_JSTR_IS_CTYPE_STR(isalpha)
P_JSTR_IS_CTYPE_STR(islower)
P_JSTR_IS_CTYPE_STR(isupper)
P_JSTR_IS_CTYPE_STR(isspace)
P_JSTR_IS_CTYPE_STR(isblank)
P_JSTR_IS_CTYPE_STR(isdigit)
P_JSTR_IS_CTYPE_STR(isxdigit)
P_JSTR_IS_CTYPE_STR(ispunct)
P_JSTR_IS_CTYPE_STR(isword)

#undef P_JSTR_IS_CTYPE_STR

/*
   Return ptr to first non-ctype character.
*/
JSTR_INLINE
JSTR_PURE
static char *
jstr_skip_ctype(const char *R s,
		const jstr_ctype_ty ctype)
{
	if (jstr_unlikely(*s != '\0'))
		return (char *)s;
#define P_JSTR_SKIP_CTYPE_IMPL(ctype_enum)             \
	do {                                           \
		while (jstr_isctype(*s++, ctype_enum)) \
			;                              \
		return (char *)s - 1;                  \
	} while (0)
	P_JSTR_SKIP_CTYPE_IMPL(ctype);
}

#define P_JSTR_SKIP_CTYPE(ctype, ctype_enum)        \
	JSTR_INLINE                                 \
	JSTR_PURE                                   \
	static char *                               \
	jstr_skip_##ctype(const char *R s)          \
	{                                           \
		P_JSTR_SKIP_CTYPE_IMPL(ctype_enum); \
	}

P_JSTR_SKIP_CTYPE(alpha, JSTR_ISALPHA)
P_JSTR_SKIP_CTYPE(lower, JSTR_ISLOWER)
P_JSTR_SKIP_CTYPE(upper, JSTR_ISUPPER)
P_JSTR_SKIP_CTYPE(space, JSTR_ISSPACE)
P_JSTR_SKIP_CTYPE(blank, JSTR_ISBLANK)
P_JSTR_SKIP_CTYPE(digit, JSTR_ISDIGIT)
P_JSTR_SKIP_CTYPE(xdigit, JSTR_ISXDIGIT)
P_JSTR_SKIP_CTYPE(punct, JSTR_ISPUNCT)
P_JSTR_SKIP_CTYPE(word, JSTR_ISWORD)

#undef P_JSTR_SKIP_CTYPE_IMPL
#undef P_JSTR_SKIP_CTYPE

#define P_JSTR_SKIP_CTYPE_REV(ctype, ctype_enum)                    \
	JSTR_INLINE                                                 \
	JSTR_PURE                                                   \
	static char *                                               \
	jstr_skip_##ctype##_rev(const char *const begin,            \
				const char *end,                    \
				const jstr_ctype_ty ctype)          \
	{                                                           \
		while (begin != end && jstr_isctype(*end--, ctype)) \
			;                                           \
		return (char *)end;                                 \
	}

P_JSTR_SKIP_CTYPE_REV(alpha, JSTR_ISALPHA)
P_JSTR_SKIP_CTYPE_REV(lower, JSTR_ISLOWER)
P_JSTR_SKIP_CTYPE_REV(upper, JSTR_ISUPPER)
P_JSTR_SKIP_CTYPE_REV(space, JSTR_ISSPACE)
P_JSTR_SKIP_CTYPE_REV(blank, JSTR_ISBLANK)
P_JSTR_SKIP_CTYPE_REV(digit, JSTR_ISDIGIT)
P_JSTR_SKIP_CTYPE_REV(xdigit, JSTR_ISXDIGIT)
P_JSTR_SKIP_CTYPE_REV(punct, JSTR_ISPUNCT)
P_JSTR_SKIP_CTYPE_REV(word, JSTR_ISWORD)

#undef P_JSTR_SKIP_CTYPE_REV

#ifdef __clang__
#	pragma clang diagnostic ignored "-Wunknown-pragmas"
#	pragma clang diagnostic push
#elif defined __GNUC__
#	pragma GCC diagnostic ignored "-Wanalyzer-allocation-size"
#	pragma GCC diagnostic ignored "-Wanalyzer-use-of-uninitialized-value"
#	pragma GCC diagnostic push
#endif

JSTR_FUNC_VOID
static void
jstr_tolower_str_len(char *R s,
		     const size_t sz)
{
	for (int i = sz % 4; i-- > 0; ++s)
		*s = jstr_tolower(*s);
	if (jstr_unlikely(sz < 4))
		return;
	for (; (s[0] = jstr_tolower(s[0])); s += 4) {
		s[1] = jstr_tolower(s[1]);
		s[2] = jstr_tolower(s[2]);
		s[3] = jstr_tolower(s[3]);
	}
}

JSTR_FUNC_VOID
static void
jstr_toupper_str_len(char *R s,
		     const size_t sz)
{
	for (int i = sz % 4; i-- > 0; ++s)
		*s = jstr_toupper(*s);
	if (jstr_unlikely(sz < 4))
		return;
	for (; (s[0] = jstr_toupper(s[0])); s += 4) {
		s[1] = jstr_toupper(s[1]);
		s[2] = jstr_toupper(s[2]);
		s[3] = jstr_toupper(s[3]);
	}
}

#ifdef __clang__
#	pragma clang diagnositc pop
#elif defined __GNUC__
#	pragma GCC diagnostic pop
#endif

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_toupper_str(char *R s) JSTR_NOEXCEPT
{
	while ((*s = jstr_toupper(*s)))
		++s;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_tolower_str(char *R s) JSTR_NOEXCEPT
{
	while ((*s = jstr_tolower(*s)))
		++s;
}

P_JSTR_END_DECLS

#undef R

#endif /* JSTR_CTYPE_H */
