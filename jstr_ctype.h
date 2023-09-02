#ifndef JSTR_CTYPE_DEF_H
#define JSTR_CTYPE_DEF_H 1

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "jstr_ctype_table.h"
#include "jstr_macros.h"
#include <string.h>

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
#if 1
	switch (_sz % 8) {
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
	const char *_end = _s + _sz;
	for (; jstr_likely(_s != _end);) {
		_s[0] = jstr_tolower_ascii(_s[0]);
		_s[1] = jstr_tolower_ascii(_s[1]);
		_s[2] = jstr_tolower_ascii(_s[2]);
		_s[3] = jstr_tolower_ascii(_s[3]);
		_s[4] = jstr_tolower_ascii(_s[4]);
		_s[5] = jstr_tolower_ascii(_s[5]);
		_s[6] = jstr_tolower_ascii(_s[6]);
		_s[7] = jstr_tolower_ascii(_s[7]);
		_s += 8;
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
jstr_tolower_str(char *JSTR_RST _s) JSTR_NOEXCEPT
{
#if JSTR_HAVE_ATTR_MAY_ALIAS && 0
	pjstr_op_ty *_sw = (pjstr_op_ty *)_s;
remainder:
	if (jstr_unlikely(_s[0] == '\0'))
		return;
	_s[0] = jstr_tolower_ascii(_s[0]);
	if (jstr_unlikely(_s[1] == '\0'))
		return;
	_s[1] = jstr_tolower_ascii(_s[1]);
	if (jstr_unlikely(_s[2] == '\0'))
		return;
	_s[2] = jstr_tolower_ascii(_s[2]);
	if (jstr_unlikely(_s[3] == '\0'))
		return;
	_s[3] = jstr_tolower_ascii(_s[3]);
	if (jstr_unlikely(_s[4] == '\0'))
		return;
	_s[4] = jstr_tolower_ascii(_s[4]);
	if (jstr_unlikely(_s[5] == '\0'))
		return;
	_s[5] = jstr_tolower_ascii(_s[5]);
	if (jstr_unlikely(_s[6] == '\0'))
		return;
	_s[6] = jstr_tolower_ascii(_s[6]);
	if (jstr_unlikely(_s[7] == '\0'))
		return;
	_s[7] = jstr_tolower_ascii(_s[7]);
	*_sw = PJSTR_PTR_ALIGN_DOWN(_sw, sizeof(*_sw));
	for (; !pjstr_has_zero(*_sw); ++_sw) {
		((char *)_sw)[0] = jstr_tolower_ascii(((char *)_sw)[0]);
		((char *)_sw)[1] = jstr_tolower_ascii(((char *)_sw)[1]);
		((char *)_sw)[2] = jstr_tolower_ascii(((char *)_sw)[2]);
		((char *)_sw)[3] = jstr_tolower_ascii(((char *)_sw)[3]);
		((char *)_sw)[4] = jstr_tolower_ascii(((char *)_sw)[4]);
		((char *)_sw)[5] = jstr_tolower_ascii(((char *)_sw)[5]);
		((char *)_sw)[6] = jstr_tolower_ascii(((char *)_sw)[6]);
		((char *)_sw)[7] = jstr_tolower_ascii(((char *)_sw)[7]);
	}
	_s = (char *)_sw;
	goto remainder;
#else
#	if 1
	for (;;) {
		if (jstr_unlikely(_s[0] == '\0'))
			break;
		_s[0] = jstr_tolower_ascii(_s[0]);
		if (jstr_unlikely(_s[1] == '\0'))
			break;
		_s[1] = jstr_tolower_ascii(_s[1]);
		if (jstr_unlikely(_s[2] == '\0'))
			break;
		_s[2] = jstr_tolower_ascii(_s[2]);
		if (jstr_unlikely(_s[3] == '\0'))
			break;
		_s[3] = jstr_tolower_ascii(_s[3]);
		if (jstr_unlikely(_s[4] == '\0'))
			break;
		_s[4] = jstr_tolower_ascii(_s[4]);
		if (jstr_unlikely(_s[5] == '\0'))
			break;
		_s[5] = jstr_tolower_ascii(_s[5]);
		if (jstr_unlikely(_s[6] == '\0'))
			break;
		_s[6] = jstr_tolower_ascii(_s[6]);
		if (jstr_unlikely(_s[7] == '\0'))
			break;
		_s[7] = jstr_tolower_ascii(_s[7]);
		_s += 8;
	}
#	else
	for (; *_s; ++_s)
		*_s = jstr_tolower_ascii(*_s);
#	endif
#endif
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_toupper_mem(char *JSTR_RST _s,
		 const size_t _sz) JSTR_NOEXCEPT
{
#if 1
	switch (_sz % 8) {
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
	const char *_end = _s + _sz;
	for (; jstr_likely(_s != _end);) {
		_s[0] = jstr_toupper_ascii(_s[0]);
		_s[1] = jstr_toupper_ascii(_s[1]);
		_s[2] = jstr_toupper_ascii(_s[2]);
		_s[3] = jstr_toupper_ascii(_s[3]);
		_s[4] = jstr_toupper_ascii(_s[4]);
		_s[5] = jstr_toupper_ascii(_s[5]);
		_s[6] = jstr_toupper_ascii(_s[6]);
		_s[7] = jstr_toupper_ascii(_s[7]);
		_s += 8;
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
#if JSTR_HAVE_ATTR_MAY_ALIAS && 0
	pjstr_op_ty *_sw = (pjstr_op_ty *)_s;
remainder:
	if (jstr_unlikely(_s[0] == '\0'))
		return;
	_s[0] = jstr_toupper_ascii(_s[0]);
	if (jstr_unlikely(_s[1] == '\0'))
		return;
	_s[1] = jstr_toupper_ascii(_s[1]);
	if (jstr_unlikely(_s[2] == '\0'))
		return;
	_s[2] = jstr_toupper_ascii(_s[2]);
	if (jstr_unlikely(_s[3] == '\0'))
		return;
	_s[3] = jstr_toupper_ascii(_s[3]);
	if (jstr_unlikely(_s[4] == '\0'))
		return;
	_s[4] = jstr_toupper_ascii(_s[4]);
	if (jstr_unlikely(_s[5] == '\0'))
		return;
	_s[5] = jstr_toupper_ascii(_s[5]);
	if (jstr_unlikely(_s[6] == '\0'))
		return;
	_s[6] = jstr_toupper_ascii(_s[6]);
	if (jstr_unlikely(_s[7] == '\0'))
		return;
	_s[7] = jstr_toupper_ascii(_s[7]);
	*_sw = PJSTR_PTR_ALIGN_UP(*_sw, sizeof(*_sw));
	for (; !pjstr_has_zero(*_sw); ++_sw) {
		((char *)_sw)[0] = jstr_toupper_ascii(((char *)_sw)[0]);
		((char *)_sw)[1] = jstr_toupper_ascii(((char *)_sw)[1]);
		((char *)_sw)[2] = jstr_toupper_ascii(((char *)_sw)[2]);
		((char *)_sw)[3] = jstr_toupper_ascii(((char *)_sw)[3]);
		((char *)_sw)[4] = jstr_toupper_ascii(((char *)_sw)[4]);
		((char *)_sw)[5] = jstr_toupper_ascii(((char *)_sw)[5]);
		((char *)_sw)[6] = jstr_toupper_ascii(((char *)_sw)[6]);
		((char *)_sw)[7] = jstr_toupper_ascii(((char *)_sw)[7]);
	}
	_s = (char *)_sw;
	goto remainder;
#else
#	if 1
	for (;;) {
		if (jstr_unlikely(_s[0] == '\0'))
			break;
		_s[0] = jstr_toupper_ascii(_s[0]);
		if (jstr_unlikely(_s[1] == '\0'))
			break;
		_s[1] = jstr_toupper_ascii(_s[1]);
		if (jstr_unlikely(_s[2] == '\0'))
			break;
		_s[2] = jstr_toupper_ascii(_s[2]);
		if (jstr_unlikely(_s[3] == '\0'))
			break;
		_s[3] = jstr_toupper_ascii(_s[3]);
		if (jstr_unlikely(_s[4] == '\0'))
			break;
		_s[4] = jstr_toupper_ascii(_s[4]);
		if (jstr_unlikely(_s[5] == '\0'))
			break;
		_s[5] = jstr_toupper_ascii(_s[5]);
		if (jstr_unlikely(_s[6] == '\0'))
			break;
		_s[6] = jstr_toupper_ascii(_s[6]);
		if (jstr_unlikely(_s[7] == '\0'))
			break;
		_s[7] = jstr_toupper_ascii(_s[7]);
		_s += 8;
	}
#	else
	for (; *_s; ++_s)
		*_s = jstr_toupper_ascii(*_s);
#	endif
#endif
}

#ifdef __clang__
#	pragma clang diagnositc pop
#elif defined __GNUC__
#	pragma GCC diagnostic pop
#endif

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#endif /* JSTR_CTYPE_DEF_H */
