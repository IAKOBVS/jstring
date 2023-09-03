#ifndef JSTR_CTYPE_DEF_H
#define JSTR_CTYPE_DEF_H 1

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#include <string.h>
#ifdef __cplusplus
}
#endif /* __cpluslus */

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
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISUPPER;
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_islower(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISLOWER;
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_isalnum(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISALNUM;
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_isdigit(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISDIGIT;
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_isxdigit(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISXDIGIT;
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_isblank(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISBLANK;
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_isgraph(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISGRAPH;
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_ispunct(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_ctype[(unsigned char)_c] & JSTR_ISPUNCT;
}

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

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
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
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
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
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_tolower_ascii(const int _c) JSTR_NOEXCEPT
{
	return pjstr_table_tolower_ascii[(unsigned char)_c];
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_isalnum_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_s == '\0'))
		return 1;
	while (jstr_isalnum(*_s++))
		;
	return (*_s - 1) == '\0';
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_isalpha_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_s == '\0'))
		return 1;
	while (jstr_isalpha(*_s++))
		;
	return (*_s - 1) == '\0';
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_islower_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_s == '\0'))
		return 1;
	while (jstr_islower(*_s++))
		;
	return (*_s - 1) == '\0';
}

JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_isupper_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_s == '\0'))
		return 1;
	while (jstr_isupper(*_s++))
		;
	return *(_s - 1) == '\0';
}

JSTR_INLINE
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_NOTHROW
JSTR_WARN_UNUSED
static int
jstr_isspace_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	return *(_s + strspn(_s, "\t\n\v\f\r ")) == '\0';
}

JSTR_INLINE
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_NOTHROW
JSTR_WARN_UNUSED
static int
jstr_isblank_str(const char *JSTR_RST _s) JSTR_NOEXCEPT
{
	return *(_s + strspn(_s, " \t")) == '\0';
}

JSTR_INLINE
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_NOTHROW
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
#	pragma GCC diagnostic push
#endif

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_tolower_mem(char *JSTR_RST _s,
		 const size_t _sz) JSTR_NOEXCEPT
{
	/* It seems that 8 iterations is not much faster. */
#define PJSTR_UNROLL_ITERATIONS 4
#if 1
	enum {
		it = PJSTR_UNROLL_ITERATIONS,
	};
	switch (_sz % it) {
#	if PJSTR_UNROLL_ITERATIONS == 8
	case 7:
		*_s = jstr_tolower_ascii(*_s);
		++_s;
		/* fallthrough */
	case 6:
		*_s = jstr_tolower_ascii(*_s);
		++_s;
		/* fallthrough */
	case 5:
		*_s = jstr_tolower_ascii(*_s);
		++_s;
		/* fallthrough */
	case 4:
		*_s = jstr_tolower_ascii(*_s);
		++_s;
		/* fallthrough */
#	endif
	case 3:
		*_s = jstr_tolower_ascii(*_s);
		++_s;
		/* fallthrough */
	case 2:
		*_s = jstr_tolower_ascii(*_s);
		++_s;
		/* fallthrough */
	case 1:
		*_s = jstr_tolower_ascii(*_s);
		++_s;
		/* fallthrough */
	case 0: break;
	}
	const char *const _end = _s + _sz;
	for (; jstr_likely(_s != _end); _s += it) {
		_s[0] = jstr_tolower_ascii(_s[0]);
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
#else
	for (; *_s; ++_s)
		*_s = jstr_tolower_ascii(*_s);
#endif
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_toupper_mem(char *JSTR_RST _s,
		 const size_t _sz) JSTR_NOEXCEPT
{
	enum { it = PJSTR_UNROLL_ITERATIONS };
#if 1
	switch (_sz % it) {
#	if PJSTR_UNROLL_ITERATIONS == 8
	case 7:
		*_s = jstr_toupper_ascii(*_s);
		++_s;
		/* fallthrough */
	case 6:
		*_s = jstr_toupper_ascii(*_s);
		++_s;
		/* fallthrough */
	case 5:
		*_s = jstr_toupper_ascii(*_s);
		++_s;
		/* fallthrough */
	case 4:
		*_s = jstr_toupper_ascii(*_s);
		++_s;
		/* fallthrough */
#	endif
	case 3:
		*_s = jstr_toupper_ascii(*_s);
		++_s;
		/* fallthrough */
	case 2:
		*_s = jstr_toupper_ascii(*_s);
		++_s;
		/* fallthrough */
	case 1:
		*_s = jstr_toupper_ascii(*_s);
		++_s;
		/* fallthrough */
	case 0: break;
	}
	const char *const _end = _s + _sz;
	for (; jstr_likely(_s < _end); _s += it) {
		_s[0] = jstr_toupper_ascii(_s[0]);
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
#else
	for (; *_s; ++_s)
		*_s = jstr_toupper_ascii(*_s);
#endif
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_toupper_str(char *JSTR_RST _s) JSTR_NOEXCEPT
{
	size_t _len = jstr_strnlen(_s, 512);
	if (_len < 512) {
		jstr_toupper_mem(_s, jstr_strnlen(_s, 512));
		return;
	}
	do {
		jstr_toupper_mem(_s += _len, _len);
		_len = jstr_strnlen(_s += _len, 2048);
	} while (_len == 2048);
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
jstr_tolower_str(char *JSTR_RST _s) JSTR_NOEXCEPT
{
	size_t _len = jstr_strnlen(_s, 512);
	if (_len < 512) {
		jstr_tolower_mem(_s, jstr_strnlen(_s, 512));
		return;
	}
	do {
		jstr_tolower_mem(_s += _len, _len);
		_len = jstr_strnlen(_s += _len, 2048);
	} while (_len == 2048);
}

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#endif /* JSTR_CTYPE_DEF_H */
