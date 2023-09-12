#ifndef JSTR_CTYPE_H
#define JSTR_CTYPE_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <string.h>
PJSTR_END_DECLS

#include "jstr-ctype-table.h"
#include "jstr-macros.h"
#include "jstr-std-string.h"

#include "string-fza.h"
#include "string-fzb.h"
#include "string-fzc.h"
#include "string-fzi.h"
#include "string-misc.h"
#include "string-opthr.h"
#include "string-optype.h"
#include "string-shift.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isupper(const int c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)c] & JSTR_ISUPPER;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_islower(const int c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)c] & JSTR_ISLOWER;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isalnum(const int c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)c] & JSTR_ISALNUM;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isdigit(const int c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)c] & JSTR_ISDIGIT;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isxdigit(const int c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)c] & JSTR_ISXDIGIT;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isblank(const int c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)c] & JSTR_ISBLANK;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isgraph(const int c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)c] & JSTR_ISGRAPH;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_ispunct(const int c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)c] & JSTR_ISPUNCT;
}

/* ASCII */
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_NOTHROW
JSTR_WARN_UNUSED
static int
jstr_isspace(const int c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)c] & JSTR_ISSPACE;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isalpha(const int c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)c] & JSTR_ISALPHA;
}

/*
   ASCII.
   For example: jstr_isctype(C, JSTR_ISLOWER | JSTR_ISDIGIT) checks if C is either lowercase or a number.
   Types:
   JSTR_ISALNUM.
   JSTR_ISALPHA.
   JSTR_ISCNTRL.
   JSTR_ISDIGIT.
   JSTR_ISGRAPH.
   JSTR_ISLOWER.
   JSTR_ISPRINT.
   JSTR_ISPUNCT.
   JSTR_ISSPACE.
   JSTR_ISUPPER.
   JSTR_ISXDIGIT.
   JSTR_ISBLANK.
*/
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isctype(const int c, jstr_ctype_ty type) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)c] & type;
}

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

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isalnum_str(const char *R s) JSTR_NOEXCEPT
{
#define PJSTR_IS_CTYPE_STR(CTYPE)               \
	do {                                    \
		if (jstr_unlikely(*s == '\0')) \
			return 0;               \
		while (jstr_##CTYPE(*s++)      \
		       && jstr_##CTYPE(*s++)   \
		       && jstr_##CTYPE(*s++)   \
		       && jstr_##CTYPE(*s++))  \
			;                       \
		return (*s - 1) == '\0';       \
	} while (0)
	PJSTR_IS_CTYPE_STR(isalnum);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isalpha_str(const char *R s) JSTR_NOEXCEPT
{
	PJSTR_IS_CTYPE_STR(isalpha);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_islower_str(const char *R s) JSTR_NOEXCEPT
{
	PJSTR_IS_CTYPE_STR(islower);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isupper_str(const char *R s) JSTR_NOEXCEPT
{
	PJSTR_IS_CTYPE_STR(isupper);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isspace_str(const char *R s) JSTR_NOEXCEPT
{
	PJSTR_IS_CTYPE_STR(isspace);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isblank_str(const char *R s) JSTR_NOEXCEPT
{
	PJSTR_IS_CTYPE_STR(isblank);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isdigit_str(const char *R s) JSTR_NOEXCEPT
{
	PJSTR_IS_CTYPE_STR(isdigit);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isxdigit_str(const char *R s) JSTR_NOEXCEPT
{
	PJSTR_IS_CTYPE_STR(isxdigit);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_ispunct_str(const char *R s) JSTR_NOEXCEPT
{
	PJSTR_IS_CTYPE_STR(ispunct);
}

#ifdef __clang__
#	pragma clang diagnostic ignored "-Wunknown-pragmas"
#	pragma clang diagnostic push
#elif defined __GNUC__
#	pragma GCC diagnostic ignored "-Wanalyzer-allocation-size"
#	pragma GCC diagnostic ignored "-Wanalyzer-use-of-uninitialized-value"
#	pragma GCC diagnostic push
#endif

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_tolower_str_len(char *R s,
		     const size_t sz)
{
	unsigned char skip;
	switch (sz % 4) {
	case 3:
		skip = 3;
		goto do3;
	case 2:
		skip = 2;
		goto do2;
	case 1:
		skip = 1;
		goto do1;
	case 0:
		break;
do3:
		s[2] = jstr_tolower(s[2]);
do2:
		s[1] = jstr_tolower(s[1]);
do1:
		s[0] = jstr_tolower(s[0]);
		s += skip;
	}
	for (; (s[0] = jstr_tolower(s[0])); s += 4) {
		s[1] = jstr_tolower(s[1]);
		s[2] = jstr_tolower(s[2]);
		s[3] = jstr_tolower(s[3]);
	}
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_toupper_str_len(char *R s,
		     const size_t sz)
{
	unsigned char skip;
	switch (sz % 4) {
	case 3:
		skip = 3;
		goto do3;
	case 2:
		skip = 2;
		goto do2;
	case 1:
		skip = 1;
		goto do1;
	case 0:
		break;
do3:
		s[2] = jstr_toupper(s[2]);
do2:
		s[1] = jstr_toupper(s[1]);
do1:
		s[0] = jstr_toupper(s[0]);
		s += skip;
	}
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

PJSTR_END_DECLS

#undef R
#undef PJSTR_IS_CTYPE_STR

#endif /* JSTR_CTYPE_H */
