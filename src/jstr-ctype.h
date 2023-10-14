#ifndef JSTR_CTYPE_H
#define JSTR_CTYPE_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <errno.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr-ctype-table.h"
#include "jstr-macros.h"
#include "jstr-std-string.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

/*
   ASCII.
   Will NOT handle EOF correctly.
   toupper(EOF) != EOF;
*/
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_toupper(const int c)
JSTR_NOEXCEPT
{
	return pjstr_ctype_toupper[(unsigned char)c];
}

/*
   ASCII.
   Will NOT handle EOF correctly.
   tolower(EOF) != EOF;
*/
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_tolower(const int c)
JSTR_NOEXCEPT
{
	return pjstr_ctype_tolower[(unsigned char)c];
}

#define PJSTR_REPEAT_CTYPE(FUNC)    \
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
	     const jstr_ctype_ty type)
JSTR_NOEXCEPT
{
	return pjstr_ctype[(unsigned char)c] & type;
}

#define PJSTR_ISCTYPE(ctype, ctype_enum)            \
	/* ASCII. */                                \
	JSTR_INLINE                                 \
	JSTR_FUNC_CONST                             \
	static int                                  \
	jstr_is##ctype(const int c)                 \
	JSTR_NOEXCEPT                               \
	{                                           \
		return jstr_isctype(c, ctype_enum); \
	}

PJSTR_REPEAT_CTYPE(PJSTR_ISCTYPE);

#undef PJSTR_ISCTYPE

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

#define PJSTR_SKIP_CTYPE(ctype, ctype_enum)            \
	/* ASCII. */                                   \
	JSTR_INLINE                                    \
	JSTR_PURE                                      \
	static char *                                  \
	jstr_skip_##ctype(const char *R s)             \
	JSTR_NOEXCEPT                                  \
	{                                              \
		return jstr_skip_ctype(s, ctype_enum); \
	}

PJSTR_REPEAT_CTYPE(PJSTR_SKIP_CTYPE)

#undef PJSTR_SKIP_CTYPE

/* ASCII. */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isctype_str(const char *R s,
		 const jstr_ctype_ty ctype)
JSTR_NOEXCEPT
{
	return jstr_likely(*s) && *(jstr_skip_ctype(s, ctype) + 1) == '\0';
}

#define PJSTR_ISCTYPE_STR(ctype, ctype_enum)            \
	/* ASCII. */                                    \
	JSTR_INLINE                                     \
	JSTR_FUNC_PURE                                  \
	static int                                      \
	jstr_is##ctype##_str(const char *R s)           \
	JSTR_NOEXCEPT                                   \
	{                                               \
		return jstr_isctype_str(s, ctype_enum); \
	}

PJSTR_REPEAT_CTYPE(PJSTR_ISCTYPE_STR)

#undef PJSTR_ISCTYPE_STR

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

#define PJSTR_SKIP_CTYPE_REV(ctype, ctype_enum)                     \
	/* ASCII. */                                                \
	JSTR_INLINE                                                 \
	JSTR_PURE                                                   \
	static char *                                               \
	jstr_skip_##ctype##_rev(const char *const begin,            \
				const char *end)                    \
	JSTR_NOEXCEPT                                               \
	{                                                           \
		return jstr_skip_ctype_rev(begin, end, ctype_enum); \
	}

PJSTR_REPEAT_CTYPE(PJSTR_SKIP_CTYPE_REV)

#undef PJSTR_SKIP_CTYPE_REV

/* ASCII. */
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_toupper_str(char *R s)
JSTR_NOEXCEPT
{
	while ((*s = jstr_toupper(*s)))
		++s;
}

/* ASCII. */
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_tolower_str(char *R s)
JSTR_NOEXCEPT
{
	while ((*s = jstr_tolower(*s)))
		++s;
}

PJSTR_END_DECLS

#undef PJSTR_REPEAT_CTYPE
#undef R

#endif /* JSTR_CTYPE_H */
