#ifndef JSTR_CTYPE_H
#define JSTR_CTYPE_H 1

#include "jstr-macros.h"

P_JSTR_BEGIN_DECLS
#include <errno.h>
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

#define P_JSTR_REPEAT_CTYPE(FUNC)   \
	FUNC(alpha, JSTR_ISALPHA)   \
	FUNC(lower, JSTR_ISLOWER)   \
	FUNC(upper, JSTR_ISUPPER)   \
	FUNC(space, JSTR_ISSPACE)   \
	FUNC(blank, JSTR_ISBLANK)   \
	FUNC(digit, JSTR_ISDIGIT)   \
	FUNC(xdigit, JSTR_ISXDIGIT) \
	FUNC(punct, JSTR_ISPUNCT)   \
	FUNC(graph, JSTR_ISGRAPH)   \
	FUNC(print, JSTR_ISPRINT)   \
	FUNC(cntrl, JSTR_ISCNTRL)   \
	FUNC(word, JSTR_ISWORD)

/*
   ASCII.
   For example: jstr_isctype(C, JSTR_ISLOWER | JSTR_ISDIGIT) checks if C is either lowercase or a digit.
*/
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isctype(const int c,
	     const jstr_ctype_ty type) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)c] & type;
}

#define P_JSTR_ISCTYPE(ctype, ctype_enum)           \
	/* ASCII. */                                \
	JSTR_INLINE                                 \
	JSTR_FUNC_CONST                             \
	static int                                  \
	jstr_is##ctype(const int c) JSTR_NOEXCEPT   \
	{                                           \
		return jstr_isctype(c, ctype_enum); \
	}

P_JSTR_REPEAT_CTYPE(P_JSTR_ISCTYPE);

#undef P_JSTR_ISCTYPE

/* ASCII. */
JSTR_INLINE
JSTR_PURE
static char *
jstr_skip_ctype(const char *R s,
		const jstr_ctype_ty ctype)
{
	if (jstr_unlikely(ctype & JSTR_ISCNTRL)) {
		while (*s != '\0'
		       && jstr_isctype(*s++, ctype))
			;
		return (char *)s - 1;
	}
	while (jstr_isctype(*s++, ctype))
		;
	return (char *)s - 1;
}

#define P_JSTR_SKIP_CTYPE(ctype, ctype_enum)           \
	/* ASCII. */                                   \
	JSTR_INLINE                                    \
	JSTR_PURE                                      \
	static char *                                  \
	jstr_skip_##ctype(const char *R s)             \
	{                                              \
		return jstr_skip_ctype(s, ctype_enum); \
	}

P_JSTR_REPEAT_CTYPE(P_JSTR_SKIP_CTYPE)

#undef P_JSTR_SKIP_CTYPE

/* ASCII. */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isctype_str(const char *R s,
		 const jstr_ctype_ty ctype) JSTR_NOEXCEPT
{
	return jstr_likely(*s) && *(jstr_skip_ctype(s, ctype) + 1) == '\0';
}

#define P_JSTR_ISCTYPE_STR(ctype, ctype_enum)               \
	/* ASCII. */                                        \
	JSTR_INLINE                                         \
	JSTR_FUNC_PURE                                      \
	static int                                          \
	jstr_is##ctype##_str(const char *R s) JSTR_NOEXCEPT \
	{                                                   \
		return jstr_isctype_str(s, ctype_enum);     \
	}

P_JSTR_REPEAT_CTYPE(P_JSTR_ISCTYPE_STR)

#undef P_JSTR_ISCTYPE_STR

/* ASCII. */
JSTR_INLINE
JSTR_PURE
static char *
jstr_skip_ctype_rev(const char *const begin,
		    const char *end,
		    const jstr_ctype_ty ctype)
{
	while (begin != end
	       && jstr_isctype(*end--, ctype))
		;
	return (char *)end;
}

#define P_JSTR_SKIP_CTYPE_REV(ctype, ctype_enum)                    \
	/* ASCII. */                                                \
	JSTR_INLINE                                                 \
	JSTR_PURE                                                   \
	static char *                                               \
	jstr_skip_##ctype##_rev(const char *const begin,            \
				const char *end)                    \
	{                                                           \
		return jstr_skip_ctype_rev(begin, end, ctype_enum); \
	}

P_JSTR_REPEAT_CTYPE(P_JSTR_SKIP_CTYPE_REV)

#undef P_JSTR_SKIP_CTYPE_REV

/* ASCII. */
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_toupper_str(char *R s) JSTR_NOEXCEPT
{
	while ((*s = jstr_toupper(*s)))
		++s;
}

/* ASCII. */
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

/*
   ASCII.
   Will NOT handle EOF correctly.
   tolower(EOF) != EOF;
*/
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

/* ASCII.
   Will not correctly handle EOF.
   toupper(EOF) != EOF.
*/
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
