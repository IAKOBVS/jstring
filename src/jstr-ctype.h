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

/*
   ASCII.
   For example: jstr_isctype(C, JSTR_ISLOWER | JSTR_ISDIGIT) checks if C is either lowercase or a digit.
*/
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isctype(const int c, jstr_ctype_ty type) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)c] & type;
}

#define P_JSTR_REPEAT_CTYPE(FUNC)   \
	FUNC(alpha, JSTR_ISALPHA)   \
	FUNC(lower, JSTR_ISLOWER)   \
	FUNC(upper, JSTR_ISUPPER)   \
	FUNC(space, JSTR_ISSPACE)   \
	FUNC(blank, JSTR_ISBLANK)   \
	FUNC(digit, JSTR_ISDIGIT)   \
	FUNC(xdigit, JSTR_ISXDIGIT) \
	FUNC(punct, JSTR_ISPUNCT)   \
	FUNC(word, JSTR_ISWORD)

#define P_JSTR_ISCTYPE(ctype, ctype_enum)                                \
	/* ASCII. */                                                     \
	JSTR_INLINE                                                      \
	JSTR_FUNC_CONST                                                  \
	static int                                                       \
	jstr_is##ctype(const int c) JSTR_NOEXCEPT                        \
	{                                                                \
		return pjstr_table_ctype[(unsigned char)c] & ctype_enum; \
	}

P_JSTR_REPEAT_CTYPE(P_JSTR_ISCTYPE);

#undef P_JSTR_ISCTYPE

#define P_JSTR_ISCTYPE_STR_IMPL(ctype_enum)                    \
	do {                                                   \
		if (jstr_unlikely(*s == '\0'))                 \
			return 0;                              \
		while (jstr_isctype(s[0], ctype_enum)) {       \
			if (!jstr_isctype(s[1], ctype_enum)) { \
				s += 1;                        \
				break;                         \
			}                                      \
			if (!jstr_isctype(s[2], ctype_enum)) { \
				s += 2;                        \
				break;                         \
			}                                      \
			if (!jstr_isctype(s[3], ctype_enum)) { \
				s += 3;                        \
				break;                         \
			}                                      \
			s += 4;                                \
		}                                              \
		return *s == '\0';                             \
	} while (0)

/* ASCII. */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isctype_str(const char *R s,
		 jstr_ctype_ty ctype) JSTR_NOEXCEPT
{
	P_JSTR_ISCTYPE_STR_IMPL(ctype);
}

#define P_JSTR_ISCTYPE_STR(ctype, ctype_enum)               \
	/* ASCII. */                                        \
	JSTR_INLINE                                         \
	JSTR_FUNC_PURE                                      \
	static int                                          \
	jstr_is##ctype##_str(const char *R s) JSTR_NOEXCEPT \
	{                                                   \
		P_JSTR_ISCTYPE_STR_IMPL(ctype_enum);        \
	}

P_JSTR_REPEAT_CTYPE(P_JSTR_ISCTYPE_STR)

#undef P_JSTR_ISCTYPE_STR
#undef P_JSTR_ISCTYPE_STR_IMPL

/* ASCII. */
JSTR_INLINE
JSTR_PURE
static char *
jstr_skip_ctype(const char *R s,
		const jstr_ctype_ty ctype)
{
#define P_JSTR_SKIP_CTYPE_IMPL(ctype_enum)             \
	do {                                           \
		while (jstr_isctype(*s++, ctype_enum)) \
			;                              \
		return (char *)s - 1;                  \
	} while (0)
	P_JSTR_SKIP_CTYPE_IMPL(ctype);
}

#define P_JSTR_SKIP_CTYPE(ctype, ctype_enum)        \
	/* ASCII. */                                \
	JSTR_INLINE                                 \
	JSTR_PURE                                   \
	static char *                               \
	jstr_skip_##ctype(const char *R s)          \
	{                                           \
		P_JSTR_SKIP_CTYPE_IMPL(ctype_enum); \
	}

P_JSTR_REPEAT_CTYPE(P_JSTR_SKIP_CTYPE)

#undef P_JSTR_SKIP_CTYPE_IMPL
#undef P_JSTR_SKIP_CTYPE

#define P_JSTR_SKIP_CTYPE_REV_IMPL(ctype, ctype_enum)                    \
	do {                                                             \
		while (begin != end && jstr_isctype(*end--, ctype_enum)) \
			;                                                \
		return (char *)end;                                      \
	} while (0)

#define P_JSTR_SKIP_CTYPE_REV(ctype, ctype_enum)               \
	/* ASCII. */                                           \
	JSTR_INLINE                                            \
	JSTR_PURE                                              \
	static char *                                          \
	jstr_skip_##ctype##_rev(const char *const begin,       \
				const char *end,               \
				const jstr_ctype_ty ctype)     \
	{                                                      \
		P_JSTR_SKIP_CTYPE_REV_IMPL(ctype, ctype_enum); \
	}

P_JSTR_REPEAT_CTYPE(P_JSTR_SKIP_CTYPE_REV)

#undef P_JSTR_SKIP_CTYPE_REV
#undef P_JSTR_SKIP_CTYPE_REV_IMPL

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

P_JSTR_END_DECLS

#undef P_JSTR_REPEAT_CTYPE
#undef R

#endif /* JSTR_CTYPE_H */
