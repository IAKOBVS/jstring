#ifndef JSTR_CTYPE_DEF_H
#define JSTR_CTYPE_DEF_H 1

#include "jstr_macros.h"

PJSTR_BEGIN_DECLS
#include <string.h>
PJSTR_END_DECLS

#include "jstr_ctype_table.h"
#include "jstr_macros.h"
#include "jstr_std_string.h"

#include "string-fza.h"
#include "string-fzb.h"
#include "string-fzc.h"
#include "string-fzi.h"
#include "string-misc.h"
#include "string-opthr.h"
#include "string-optype.h"
#include "string-shift.h"

PJSTR_BEGIN_DECLS

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isupper(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISUPPER;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_islower(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISLOWER;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isalnum(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISALNUM;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isdigit(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISDIGIT;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isxdigit(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISXDIGIT;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isblank(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISBLANK;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isgraph(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISGRAPH;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_ispunct(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISPUNCT;
}

/* ASCII */
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_NOTHROW
JSTR_WARN_UNUSED
static int
jstr_isspace(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISSPACE;
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isalpha(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISALPHA;
}

/*
   ASCII.
   Will NOT handle EOF correctly.
   toupper(EOF) != EOF;
*/
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_toupper(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_toupper[(unsigned char)_c];
}

/*
   ASCII.
   Will NOT handle EOF correctly.
   tolower(EOF) != EOF;
*/
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_tolower(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_tolower[(unsigned char)_c];
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isalnum_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
#define PJSTR_IS_CTYPE_STR(CTYPE)               \
	do {                                    \
		if (jstr_unlikely(*_s == '\0')) \
			return 0;               \
		while (jstr_##CTYPE(*_s++)      \
		       && jstr_##CTYPE(*_s++)   \
		       && jstr_##CTYPE(*_s++)   \
		       && jstr_##CTYPE(*_s++))  \
			;                       \
		return (*_s - 1) == '\0';       \
	} while (0)
	PJSTR_IS_CTYPE_STR(isalnum);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isalpha_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	PJSTR_IS_CTYPE_STR(isalpha);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_islower_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	PJSTR_IS_CTYPE_STR(islower);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isupper_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	PJSTR_IS_CTYPE_STR(isupper);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isspace_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	PJSTR_IS_CTYPE_STR(isspace);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isblank_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	PJSTR_IS_CTYPE_STR(isblank);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isdigit_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	PJSTR_IS_CTYPE_STR(isdigit);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isxdigit_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	PJSTR_IS_CTYPE_STR(isxdigit);
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_ispunct_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
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
jstr_tolower_str_len(char *JSTR_RST _s,
		     const size_t _sz)
{
	unsigned char _skip;
	switch (_sz % 4) {
	case 3:
		_skip = 3;
		goto do3;
	case 2:
		_skip = 2;
		goto do2;
	case 1:
		_skip = 1;
		goto do1;
	case 0:
		goto start_loop;
do3:
		_s[2] = jstr_tolower(_s[2]);
do2:
		_s[1] = jstr_tolower(_s[1]);
do1:
		_s[0] = jstr_tolower(_s[0]);
	}
	_s += _skip;
start_loop:;
	for (; (_s[0] = jstr_tolower(_s[0])); _s += 4) {
		_s[1] = jstr_tolower(_s[1]);
		_s[2] = jstr_tolower(_s[2]);
		_s[3] = jstr_tolower(_s[3]);
	}
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_toupper_str_len(char *JSTR_RST _s,
		     const size_t _sz)
{
	unsigned char _skip;
	switch (_sz % 4) {
	case 3:
		_skip = 3;
		goto do3;
	case 2:
		_skip = 2;
		goto do2;
	case 1:
		_skip = 1;
		goto do1;
	case 0:
		goto start_loop;
do3:
		_s[2] = jstr_toupper(_s[2]);
do2:
		_s[1] = jstr_toupper(_s[1]);
do1:
		_s[0] = jstr_toupper(_s[0]);
	}
	_s += _skip;
start_loop:;
	for (; (_s[0] = jstr_toupper(_s[0])); _s += 4) {
		_s[1] = jstr_toupper(_s[1]);
		_s[2] = jstr_toupper(_s[2]);
		_s[3] = jstr_toupper(_s[3]);
	}
}

#ifdef __clang__
#	pragma clang diagnositc pop
#elif defined __GNUC__
#	pragma GCC diagnostic pop
#endif

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_toupper_str(char *JSTR_RST _s) JSTR_NOEXCEPT
{
	while ((*_s = jstr_toupper(*_s)))
		++_s;
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_tolower_str(char *JSTR_RST _s) JSTR_NOEXCEPT
{
	while ((*_s = jstr_tolower(*_s)))
		++_s;
}

PJSTR_END_DECLS

#undef PJSTR_IS_CTYPE_STR

#endif /* JSTR_CTYPE_DEF_H */
