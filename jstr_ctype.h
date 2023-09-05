#ifndef JSTR_CTYPE_DEF_H
#define JSTR_CTYPE_DEF_H 1

#include "jstr_macros.h"

JSTR_BEGIN_DECLS
#include <string.h>
JSTR_END_DECLS

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

JSTR_BEGIN_DECLS

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
   Will NOT handle EOF correctly.
   toupper_ascii(EOF) != EOF;
*/
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_toupper_ascii(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_toupper_ascii[(unsigned char)_c];
}

/*
   Will NOT handle EOF correctly.
   tolower_ascii(EOF) != EOF;
*/
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_tolower_ascii(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_tolower_ascii[(unsigned char)_c];
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isalnum_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_s == '\0'))
		return 1;
	while (jstr_isalnum(*_s++))
		;
	return (*_s - 1) == '\0';
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isalpha_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_s == '\0'))
		return 1;
	while (jstr_isalpha(*_s++))
		;
	return (*_s - 1) == '\0';
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_islower_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_s == '\0'))
		return 1;
	while (jstr_islower(*_s++))
		;
	return (*_s - 1) == '\0';
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isupper_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_s == '\0'))
		return 1;
	while (jstr_isupper(*_s++))
		;
	return *(_s - 1) == '\0';
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isspace_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	return *(_s + strspn(_s, "\t\n\v\f\r ")) == '\0';
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isblank_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	return *(_s + strspn(_s, " \t")) == '\0';
}

/* ASCII */
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isdigit_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	return *(_s + strspn(_s, "0123456789")) == '\0';
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
jstr_tolower_len(char *JSTR_RST _s,
		 const size_t _sz)
{
	/* It seems that 8 iterations is not much faster. */
#define PJSTR_UNROLL_ITERATIONS 4
#if PJSTR_NO_UNROLL
	for (; (*_s = jstr_tolower_ascii(*_s)); ++_s)
		;
#else
	enum {
		it = PJSTR_UNROLL_ITERATIONS,
	};
	int skip;
	switch (_sz % it) {
#	if PJSTR_UNROLL_ITERATIONS == 8
	case 7:
		skip = 7;
		goto do7;
	case 6:
		skip = 6;
		goto do6;
	case 5:
		skip = 5;
		goto do5;
	case 4:
		skip = 4;
		goto do4;
#	endif
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
		goto START_LOOP;
		break;
#	if PJSTR_UNROLL_ITERATIONS == 8
do7:
		_s[6] = jstr_tolower_ascii(_s[6]);
do6:
		_s[5] = jstr_tolower_ascii(_s[5]);
do5:
		_s[4] = jstr_tolower_ascii(_s[4]);
do4:
		_s[3] = jstr_tolower_ascii(_s[3]);
#	endif
do3:
		_s[2] = jstr_tolower_ascii(_s[2]);
do2:
		_s[1] = jstr_tolower_ascii(_s[1]);
do1:
		_s[0] = jstr_tolower_ascii(_s[0]);
	}
	_s += skip;
START_LOOP:;
	for (; (_s[0] = jstr_tolower_ascii(_s[0])); _s += it) {
		_s[1] = jstr_tolower_ascii(_s[1]);
		_s[2] = jstr_tolower_ascii(_s[2]);
		_s[3] = jstr_tolower_ascii(_s[3]);
#	if PJSTR_UNROLL_ITERATIONS == 8
		_s[4] = jstr_tolower_ascii(_s[4]);
		_s[5] = jstr_tolower_ascii(_s[5]);
		_s[6] = jstr_tolower_ascii(_s[6]);
		_s[7] = jstr_tolower_ascii(_s[7]);
#	endif
	}
#endif
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_toupper_len(char *JSTR_RST _s,
		 const size_t _sz)
{
	/* It seems that 8 iterations is not much faster. */
#define PJSTR_UNROLL_ITERATIONS 4
#if PJSTR_NO_UNROLL
	for (; (*_s = jstr_toupper_ascii(*_s)); ++_s)
		;
#else
	enum {
		it = PJSTR_UNROLL_ITERATIONS,
	};
	int skip;
	switch (_sz % it) {
#	if PJSTR_UNROLL_ITERATIONS == 8
	case 7:
		skip = 7;
		goto do7;
	case 6:
		skip = 6;
		goto do6;
	case 5:
		skip = 5;
		goto do5;
	case 4:
		skip = 4;
		goto do4;
#	endif
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
		goto START_LOOP;
		break;
#	if PJSTR_UNROLL_ITERATIONS == 8
do7:
		_s[6] = jstr_toupper_ascii(_s[6]);
do6:
		_s[5] = jstr_toupper_ascii(_s[5]);
do5:
		_s[4] = jstr_toupper_ascii(_s[4]);
do4:
		_s[3] = jstr_toupper_ascii(_s[3]);
#	endif
do3:
		_s[2] = jstr_toupper_ascii(_s[2]);
do2:
		_s[1] = jstr_toupper_ascii(_s[1]);
do1:
		_s[0] = jstr_toupper_ascii(_s[0]);
	}
	_s += skip;
START_LOOP:;
	for (; (_s[0] = jstr_toupper_ascii(_s[0])); _s += it) {
		_s[1] = jstr_toupper_ascii(_s[1]);
		_s[2] = jstr_toupper_ascii(_s[2]);
		_s[3] = jstr_toupper_ascii(_s[3]);
#	if PJSTR_UNROLL_ITERATIONS == 8
		_s[4] = jstr_toupper_ascii(_s[4]);
		_s[5] = jstr_toupper_ascii(_s[5]);
		_s[6] = jstr_toupper_ascii(_s[6]);
		_s[7] = jstr_toupper_ascii(_s[7]);
#	endif
	}
#endif
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
#if PSJTR_NO_UNROLL
	for (; (*_s = jstr_toupper_ascii(*_s)); ++_s)
		;
#else
	size_t _len = jstr_strnlen(_s, 512);
	if (_len < 512) {
		jstr_toupper_len(_s, jstr_strnlen(_s, 512));
		return;
	}
	do {
		jstr_toupper_len(_s += _len, _len);
		_len = jstr_strnlen(_s += _len, 2048);
	} while (_len == 2048);
#endif
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_tolower_str(char *JSTR_RST _s) JSTR_NOEXCEPT
{
#if PSJTR_NO_UNROLL
	for (; *_s = jstr_tolower_ascii(*_s); ++_s)
		;
#else
	size_t _len = jstr_strnlen(_s, 512);
	if (_len < 512) {
		jstr_tolower_len(_s, jstr_strnlen(_s, 512));
		return;
	}
	do {
		jstr_tolower_len(_s += _len, _len);
		_len = jstr_strnlen(_s += _len, 2048);
	} while (_len == 2048);
#endif
}

JSTR_END_DECLS

#endif /* JSTR_CTYPE_DEF_H */
