/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 * This file is part of the jstring library.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#ifndef JSTR_MACROS_H
#	define JSTR_MACROS_H 1

#	ifndef JSTR_BUILT
#		error "Including jstr.h before building."
#	endif

#	include "jstr-config.h"
#	include "jstr-macros-arch.h"
#	include "jstr-macros-os.h"
#	include "jstr-pointer-arith.h"

#	ifdef __cplusplus
#		define JSTR__BEGIN_DECLS extern "C" {
#		define JSTR__END_DECLS   }
#	else
#		define JSTR__BEGIN_DECLS
#		define JSTR__END_DECLS
#	endif

JSTR__BEGIN_DECLS
#	include <assert.h>
#	include <errno.h>
#	include <stdio.h>
#	include <stdlib.h>
#	include <string.h>
JSTR__END_DECLS

#	if defined _LP64 || defined __LP64__
#		define JSTR_LP64 1
#	elif defined _ILP32 || defined __ILP32__
#		define JSTR_LP32 1
#	else
#		define JSTR_LP_UNKNOWN 1
#	endif

JSTR__BEGIN_DECLS
typedef enum {
	JSTR_RET_ERR = -1,
	JSTR_RET_SUCC = 0
} jstr_ret_ty;
JSTR__END_DECLS

#	if defined __GLIBC__ || JSTR_ENV_BSD
JSTR__BEGIN_DECLS
#		include <sys/cdefs.h>
JSTR__END_DECLS
#	endif

JSTR__BEGIN_DECLS
#	if JSTR_OS_SOLARIS
#		include <sys/int_types.h>
#	else
#		include <stdint.h>
#	endif
JSTR__END_DECLS

#	ifdef __GLIBC_PREREQ
#		define JSTR_GLIBC_PREREQ(maj, min) __GLIBC_PREREQ(maj, min)
#	elif defined __GLIBC__
#		define JSTR_GLIBC_PREREQ(maj, min) ((__GLIBC__ << 16) + __GLIBC_MINOR__ >= ((maj) << 16) + (min))
#	endif

#	ifdef __GNUC_PREREQ
#		define JSTR_GNUC_PREREQ(maj, min) __GNUC_PREREQ(maj, min)
#	elif defined __GNUC__
#		define JSTR_GNUC_PREREQ(maj, min) ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#	endif

#	if JSTR_ENV_BSD
JSTR__BEGIN_DECLS
#		include <sys/endian.h>
JSTR__END_DECLS
#	elif defined __GLIBC__ && ((JSTR_GLIBC_PREREQ(2, 19) && defined _BSD_SOURCE) || defined _DEFAULT_SOURCE)
JSTR__BEGIN_DECLS
#		include <endian.h>
JSTR__END_DECLS
#	elif JSTR_OS_MAC
#		include <machine/endian.h>
#	endif

#	if JSTR_DEBUG
#		undef JSTR_PANIC
#		define JSTR_PANIC 1
#	endif

#	define JSTR_SWAP(T, x, y)        \
		do {                      \
			T const _TMP = x; \
			x = y;            \
			y = _TMP;         \
		} while (0)

#	ifdef __BYTE_ORDER
#		define JSTR_WORD_ORDER __BYTE_ORDER
#	elif defined _BYTE_ORDER
#		define JSTR_WORD_ORDER _BYTE_ORDER
#	endif

#	ifdef __BIG_ENDIAN
#		define JSTR_BIG_ENDIAN __BIG_ENDIAN
#	elif defined _BIG_ENDIAN
#		define JSTR_BIG_ENDIAN _BIG_ENDIAN
#	endif

#	ifdef __LITTLE_ENDIAN
#		define JSTR_LITTLE_ENDIAN __LITTLE_ENDIAN
#	elif defined _LITTLE_ENDIAN
#		define JSTR_LITTLE_ENDIAN _LITTLE_ENDIAN
#	endif

#	ifdef JSTR_WORD_ORDER
#		if JSTR_WORD_ORDER == JSTR_BIG_ENDIAN                               \
		|| defined __BIG_ENDIAN__                                            \
		|| defined __ARMEB__ || defined __THUMBEB__ || defined __AARCH64EB__ \
		|| defined _MIBSEB || defined __MIBSEB || defined __MIBSEB__
#			undef JSTR_ENDIAN_BIG
#			undef JSTR_ENDIAN_LITTLE
#			define JSTR_ENDIAN_BIG 1
#		elif JSTR_WORD_ORDER == JSTR_LITTLE_ENDIAN                          \
		|| defined __LITTLE_ENDIAN__                                         \
		|| defined __ARMEL__ || defined __THUMBEL__ || defined __AARCH64EL__ \
		|| defined _MIPSEL || defined __MIPSEL || defined __MIPSEL__
#			undef JSTR_ENDIAN_LITTLE
#			undef JSTR_ENDIAN_BIG
#			define JSTR_ENDIAN_LITTLE 1
#		endif
#	endif /* byte_order */

#	if !JSTR_ENDIAN_LITTLE && !JSTR_ENDIAN_BIG
#		define JSTR_ENDIAN_UNKNOWN 1
#	endif

#	define jstr_err(msg)    jstr__err(JSTR_ASSERT_FILE, JSTR_ASSERT_LINE, JSTR_ASSERT_FUNC, msg)
#	define jstr_errdie(msg) jstr__errdie(JSTR_ASSERT_FILE, JSTR_ASSERT_LINE, JSTR_ASSERT_FUNC, msg)

#	define jstr_chk(ret)             jstr_unlikely(ret == -1)
#	define jstr_nullchk(p)           jstr_unlikely((p) == NULL)
#define JSTR_PAGE_SIZE 4096
#	define JSTR_ARRAY_COUNT(array)   (sizeof(array) / sizeof(array[0]))
#	define JSTR__CONCAT_HELPER(x, y) x##y
#	define JSTR_CONCAT(x, y)         JSTR__CONCAT_HELPER(x, y)
#	define JSTR_STRING(x)            #x

#	define JSTR_MAX(x, y)        (((x) > (y)) ? (x) : (y))
#	define JSTR_MIN(x, y)        (((x) < (y)) ? (x) : (y))
#	define JSTR_BETWEEN(x, a, b) (((a) <= (x)) && ((x) <= (b)))
#	define JSTR_MIN3(x, y, z)    (((x) < (y)) ? (((x) < (z)) ? (x) : (z)) : (((y) < (z)) ? (y) : (z)))
#	define JSTR_MAX3(x, y, z)    (((x) > (y)) ? (((x) > (z)) ? (x) : (z)) : (((y) > (z)) ? (y) : (z)))
#	define JSTR_MID3(x, y, z)    (((x) > (y)) ? (((x) < (z)) ? (x) : (z)) : (((y) < (z)) ? (y) : (z)))

#	define JSTR_IO_KB (1000)
#	define JSTR_IO_MB (JSTR_IO_KB * JSTR_IO_KB)
#	define JSTR_IO_GB (JSTR_IO_MB * JSTR_IO_KB)
#	define JSTR_IO_TB (JSTR_IO_GB * JSTR_IO_KB)
#	define JSTR_IO_PB (JSTR_IO_TB * JSTR_IO_KB)
#	define JSTR_IO_EB (JSTR_IO_PB * JSTR_IO_KB)
#	define JSTR_IO_ZB (JSTR_IO_EB * JSTR_IO_KB)
#	define JSTR_IO_YB (JSTR_IO_ZB * JSTR_IO_KB)
#	define JSTR_IO_RB (JSTR_IO_YB * JSTR_IO_KB)
#	define JSTR_IO_QB (JSTR_IO_RB * JSTR_IO_KB)

#	define JSTR_IO_KIB (1024)
#	define JSTR_IO_MIB (JSTR_IO_KB * JSTR_IO_KIB)
#	define JSTR_IO_GIB (JSTR_IO_MB * JSTR_IO_KIB)
#	define JSTR_IO_TIB (JSTR_IO_GB * JSTR_IO_KIB)
#	define JSTR_IO_PIB (JSTR_IO_TB * JSTR_IO_KIB)
#	define JSTR_IO_EIB (JSTR_IO_PB * JSTR_IO_KIB)
#	define JSTR_IO_ZIB (JSTR_IO_EB * JSTR_IO_KIB)
#	define JSTR_IO_YIB (JSTR_IO_ZB * JSTR_IO_KIB)
#	define JSTR_IO_RIB (JSTR_IO_YB * JSTR_IO_KIB)
#	define JSTR_IO_QIB (JSTR_IO_RB * JSTR_IO_KIB)

#	define JSTR_ALPHA_VOWEL_LOWER_STR     "aiueo"
#	define JSTR_ALPHA_VOWEL_UPPER_STR     "AIUEO"
#	define JSTR_ALPHA_CONSONANT_LOWER_STR "bcdfghjklmnpqrstvwxyz"
#	define JSTR_ALPHA_CONSONANT_UPPER_STR "BCDFGHJKLMNPQRSTVWXYZ"
#	define JSTR_DIGIT_STR                 "0123456789"
#	define JSTR_SPACE_STR                 " \t\n\r\f\v"
#	define JSTR_PUNCT_STR                 "!\"#$%&\()*+,-./:;<=>?@[\\]^_`{|}~";
#	define JSTR_XDIGIT_LOWER_STR          JSTR_DIGIT_STR "abcdef"
#	define JSTR_XDIGIT_UPPER_STR          JSTR_DIGIT_STR "ABCDEF"
#	define JSTR_XDIGIT_STR                JSTR_DIGIT_STR "abcdefABCDEF"
#	define JSTR_ALPHA_VOWEL_STR           JSTR_ALPHA_VOWEL_LOWER_STR JSTR_ALPHA_VOWEL_UPPER_STR
#	define JSTR_ALPHA_CONSONANT_STR       JSTR_ALPHA_CONSONANT_LOWER_STR JSTR_ALPHA_CONSONANT_UPPER_STR
#	define JSTR_ALPHA_LOWER_STR           JSTR_ALPHA_VOWEL_LOWER_STR JSTR_ALPHA_CONSONANT_LOWER_STR
#	define JSTR_ALPHA_UPPER_STR           JSTR_ALPHA_VOWEL_UPPER_STR JSTR_ALPHA_CONSONANT_UPPER_STR
#	define JSTR_ALPHA_STR                 JSTR_ALPHA_LOWER_STR JSTR_ALPHA_UPPER_STR
#	define JSTR_ALNUM_LOWER_STR           JSTR_ALPHA_LOWER_STR JSTR_DIGIT_STR
#	define JSTR_ALNUM_UPPER_STR           JSTR_ALPHA_UPPER_STR JSTR_DIGIT_STR
#	define JSTR_ALNUM_STR                 JSTR_ALPHA_STR JSTR_DIGIT_STR
#	define JSTR_DIGIT_CASE \
	case '0':               \
	case '1':               \
	case '2':               \
	case '3':               \
	case '4':               \
	case '5':               \
	case '6':               \
	case '7':               \
	case '8':               \
	case '9':
#	define JSTR_ALPHA_VOWEL_LOWER_CASE \
	case 'a':                           \
	case 'i':                           \
	case 'u':                           \
	case 'e':                           \
	case 'o':
#	define JSTR_ALPHA_VOWEL_UPPER_CASE \
	case 'A':                           \
	case 'I':                           \
	case 'U':                           \
	case 'E':                           \
	case 'O':
#	define JSTR_ALPHA_CONSONANT_LOWER_CASE \
	case 'b':                               \
	case 'c':                               \
	case 'd':                               \
	case 'f':                               \
	case 'g':                               \
	case 'h':                               \
	case 'j':                               \
	case 'k':                               \
	case 'l':                               \
	case 'm':                               \
	case 'n':                               \
	case 'p':                               \
	case 'q':                               \
	case 'r':                               \
	case 's':                               \
	case 't':                               \
	case 'v':                               \
	case 'w':                               \
	case 'x':                               \
	case 'y':                               \
	case 'z':
#	define JSTR_ALPHA_CONSONANT_UPPER_CASE \
	case 'B':                               \
	case 'C':                               \
	case 'D':                               \
	case 'F':                               \
	case 'G':                               \
	case 'H':                               \
	case 'J':                               \
	case 'K':                               \
	case 'L':                               \
	case 'M':                               \
	case 'N':                               \
	case 'P':                               \
	case 'Q':                               \
	case 'R':                               \
	case 'S':                               \
	case 'T':                               \
	case 'V':                               \
	case 'W':                               \
	case 'X':                               \
	case 'Y':                               \
	case 'Z':
#	define JSTR_XDIGIT_CASE  \
	JSTR_DIGIT_CASE case 'a': \
	case 'b':                 \
	case 'c':                 \
	case 'd':                 \
	case 'e':                 \
	case 'f':                 \
	case 'A':                 \
	case 'B':                 \
	case 'C':                 \
	case 'D':                 \
	case 'E':                 \
	case 'F':
#	define JSTR_BLANK_CASE \
	case '\t':              \
	case ' ':
#	define JSTR_SPACE_CASE    \
	JSTR_BLANK_CASE case '\n': \
	case '\v':                 \
	case '\f':                 \
	case '\r':
#	define JSTR_GRAPH_CASE \
	case 33:                \
	case 34:                \
	case 35:                \
	case 36:                \
	case 37:                \
	case 38:                \
	case 39:                \
	case 40:                \
	case 41:                \
	case 42:                \
	case 43:                \
	case 44:                \
	case 45:                \
	case 46:                \
	case 47:                \
	case 48:                \
	case 49:                \
	case 50:                \
	case 51:                \
	case 52:                \
	case 53:                \
	case 54:                \
	case 55:                \
	case 56:                \
	case 57:                \
	case 58:                \
	case 59:                \
	case 60:                \
	case 61:                \
	case 62:                \
	case 63:                \
	case 64:                \
	case 65:                \
	case 66:                \
	case 67:                \
	case 68:                \
	case 69:                \
	case 70:                \
	case 71:                \
	case 72:                \
	case 73:                \
	case 74:                \
	case 75:                \
	case 76:                \
	case 77:                \
	case 78:                \
	case 79:                \
	case 80:                \
	case 81:                \
	case 82:                \
	case 83:                \
	case 84:                \
	case 85:                \
	case 86:                \
	case 87:                \
	case 88:                \
	case 89:                \
	case 90:                \
	case 91:                \
	case 92:                \
	case 93:                \
	case 94:                \
	case 95:                \
	case 96:                \
	case 97:                \
	case 98:                \
	case 99:                \
	case 100:               \
	case 101:               \
	case 102:               \
	case 103:               \
	case 104:               \
	case 105:               \
	case 106:               \
	case 107:               \
	case 108:               \
	case 109:               \
	case 110:               \
	case 111:               \
	case 112:               \
	case 113:               \
	case 114:               \
	case 115:               \
	case 116:               \
	case 117:               \
	case 118:               \
	case 119:               \
	case 120:               \
	case 121:               \
	case 122:               \
	case 123:               \
	case 124:               \
	case 125:               \
	case 126:
#	define JSTR_PRINT_CASE JSTR_GRAPH_CASE case 32:
#	define JSTR_CNTRL_CASE \
	case 0:                 \
	case 1:                 \
	case 2:                 \
	case 3:                 \
	case 4:                 \
	case 5:                 \
	case 6:                 \
	case 7:                 \
	case 8:                 \
	case 9:                 \
	case 10:                \
	case 11:                \
	case 12:                \
	case 13:                \
	case 14:                \
	case 15:                \
	case 16:                \
	case 17:                \
	case 18:                \
	case 19:                \
	case 20:                \
	case 21:                \
	case 22:                \
	case 23:                \
	case 24:                \
	case 25:                \
	case 26:                \
	case 27:                \
	case 28:                \
	case 29:                \
	case 30:                \
	case 31:                \
	case 127:
#	define JSTR_PUNCT_CASE \
	case '!':               \
	case '"':               \
	case '#':               \
	case '$':               \
	case '%':               \
	case '&':               \
	case '\'':              \
	case '(':               \
	case ')':               \
	case '*':               \
	case '+':               \
	case ',':               \
	case '-':               \
	case '.':               \
	case '/':               \
	case ':':               \
	case ';':               \
	case '<':               \
	case '=':               \
	case '>':               \
	case '?':               \
	case '@':               \
	case '[':               \
	case '\\':              \
	case ']':               \
	case '^':               \
	case '_':               \
	case '`':               \
	case '{':               \
	case '|':               \
	case '}':               \
	case '~':
#	define JSTR_ALPHA_LOWER_CASE JSTR_VOWEL_LOWER_CASE JSTR_CONSONANT_LOWER_CASE
#	define JSTR_ALPHA_UPPER_CASE JSTR_VOWEL_UPPER_CASE JSTR_CONSONANT_UPPER_CASE
#	define JSTR_VOWEL_CASE       JSTR_VOWEL_ALPHA_UPPER_CASE JSTR_VOWEL_ALPHA_LOWER_CASE
#	define JSTR_CONSONANT_CASE   JSTR_CONSONANT_ALPHA_UPPER_CASE JSTR_CONSONANT_ALPHA_LOWER_CASE
#	define JSTR_ALPHA_CASE       JSTR_ALPHA_LOWER_CASE JSTR_ALPHA_UPPER_CASE
#	define JSTR_ALNUM_CASE       JSTR_DIGIT_CASE JSTR_ALPHA_CASE

#	define JSTR_MEMSET_ARRAY(array, c) ((sizeof(array) == 256) ? (memset((array), (c), 64), memset((array) + 64, (c), 64), memset((array) + 64 + 64, (c), 64), memset((array) + 64 + 64 + 64, (c), 64)) : memset((array), (c), sizeof((array))))
#	define JSTR_BZERO_ARRAY(array)     JSTR_MEMSET_ARRAY(array, 0)

#	ifdef static_assert
#		define JSTR_HAVE_STATIC_ASSERT       1
#		define JSTR_STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#	elif defined _Static_assert || defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L
#		define JSTR_HAVE_STATIC_ASSERT       1
#		define JSTR_STATIC_ASSERT(expr, msg) _Static_assert(expr, msg)
#	else
#		define JSTR_STATIC_ASSERT(expr, msg)
#	endif /* static_assert */

#	if JSTR_DEBUG
#		define JSTR_ASSERT_DEBUG(expr, msg)          \
			do {                                  \
				if (jstr_unlikely(!(expr))) { \
					jstr_err(msg);        \
					assert(expr);         \
				}                             \
			} while (0)
#		define JSTR_ASSERT_DEBUG_PRINTF(expr, ...)           \
			do {                                          \
				if (jstr_unlikely(!(expr))) {         \
					jstr_err("");                 \
					fprintf(stderr, __VA_ARGS__); \
					assert(expr);                 \
				}                                     \
			} while (0)
#		define JSTR_DEBUG_PRINT(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#		define JSTR_DEBUG_JSTRING(s, sz, cap)          \
			do {                                    \
				fprintf(stderr, "%s.\n", s);    \
				fprintf(stderr, "%zu.\n", sz);  \
				fprintf(stderr, "%zu.\n", cap); \
			} while (0)
#	else
#		define JSTR_ASSERT_DEBUG(expr, msg) \
			do {                         \
			} while (0)
#		define JSTR_ASSERT_DEBUG_PRINTF(expr, ...) \
			do {                                \
			} while (0)
#		define JSTR_DEBUG_JSTRING(s, sz, cap) \
			do {                           \
			} while (0)
#		define JSTR_DEBUG_PRINT(fmt, ...) \
			do {                       \
			} while (0)
#	endif

#	if JSTR_PANIC
#		define JSTR_RETURN_ERR(errcode) \
			do {                     \
				jstr_errdie(""); \
				return errcode;  \
			} while (0)
#	else
#		define JSTR_RETURN_ERR(errcode) return errcode
#	endif

#	ifdef __cplusplus
template <typename T, typename Other>
static T
JSTR__CAST(T, Other other)
{
	return (T)other;
}
#	else
#		define JSTR__CAST(T, other) (other)
#	endif /* cast */

#	if !(defined __STDC_VERSION__ && __STDC_VERSION__ > 201000L && !defined __STDC_NO_VLA__)
#		define JSTR_HAVE_VLA 1
#	endif

#	ifdef __GLIBC__
#		include <alloca.h>
#	endif
#	include <stdlib.h>
#	ifdef alloca
#		define JSTR_HAVE_ALLOCA 1
#	endif

#	if !defined __cplusplus && defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L
#		define JSTR_HAVE_GENERIC 1
#	endif /* have_generic */

#	if JSTR_GNUC_PREREQ(4, 0) \
	|| (defined __clang__ && (__clang_major__ >= 3))
#		define JSTR_HAVE_TYPEOF 1
#	endif /* have_typeof */

#	if JSTR_HAVE_TYPEOF && JSTR_HAVE_GENERIC
#		define JSTR_SAME_TYPE(x, y) _Generic((x), __typeof__(y): 1, default: 0)
#		define JSTR__IS_TYPE(T, x)  _Generic((x), T: 1, default: 0)
#	else
#		define JSTR_SAME_TYPE(x, y) 1
#		define JSTR__IS_TYPE(T, x)  1
#	endif /* have_typeof && have_generic */

#	if JSTR_HAVE_GENERIC
#		define JSTR_GENERIC_CASE_SIZE(bool_)               int : bool_, unsigned int : bool_, size_t : bool_, long : bool_, long long : bool_, unsigned long long : bool_
#		define JSTR_GENERIC_CASE_STR(bool_)                char * : bool_, const char * : bool_
#		define JSTR_GENERIC_CASE_STR_STACK(bool_, s)       char (*)[sizeof(s)] : bool_, const char (*)[sizeof(s)] : bool_
#		define JSTR_GENERIC_CASE_CHAR(bool_)               char : bool_, const char : bool_
#		define JSTR__IS_SIZE(expr)                         _Generic((expr), JSTR_GENERIC_CASE_SIZE(1), default: 0)
#		define JSTR__IS_STR(expr)                          _Generic((expr), JSTR_GENERIC_CASE_STR(1), default: 0)
#		define JSTR__IS_STR_STACK(expr)                    _Generic((expr), JSTR_GENERIC_CASE_STR_STACK(1, expr), default: 0)
#		define JSTR__IS_CHAR(expr)                         _Generic((expr), JSTR_GENERIC_CASE_CHAR(1), default: 0)
#		define JSTR_STATIC_ASSERT_IS_SIZE(expr)            JSTR_STATIC_ASSERT(JSTR__IS_SIZE(expr), "Passing non-size_type as number argument!");
#		define JSTR_STATIC_ASSERT_IS_STR(expr)             JSTR_STATIC_ASSERT(JSTR__IS_STR(expr), "Passing non-string as string argument!");
#		define JSTR_STATIC_ASSERT_IS_CHAR(expr)            JSTR_STATIC_ASSERT(JSTR__IS_CHAR(expr), "Passing non-char as char argument!");
#		define JSTR_STATIC_ASSERT_TYPECHECK(expr_ty, expr) JSTR_STATIC_ASSERT(JSTR_SAME_TYPE(expr_ty, expr), "Passing the wrong data type!");
#	else
#		define JSTR_GENERIC_CASE_SIZE(bool_)
#		define JSTR_GENERIC_CASE_STR(bool_)
#		define JSTR_GENERIC_CASE_STR_STACK(bool_, s)
#		define JSTR_GENERIC_CASE_CHAR(bool_)
#		define JSTR__IS_SIZE(expr)
#		define JSTR__IS_STR(expr)
#		define JSTR__IS_STR_STACK(expr)
#		define JSTR__IS_CHAR(expr)
#		define JSTR_STATIC_ASSERT_IS_SIZE(expr)
#		define JSTR_STATIC_ASSERT_IS_STR(expr)
#		define JSTR_STATIC_ASSERT_IS_CHAR(expr)
#		define JSTR_STATIC_ASSERT_TYPECHECK(expr_ty, expr)
#	endif /* have_generic */

#	if defined __cplusplus && __cplusplus > 199711L
#		define JSTR_NOEXCEPT noexcept
#	else
#		define JSTR_NOEXCEPT
#	endif /* noexcept */

#	ifdef __ASSERT_FUNCTION
#		define JSTR_ASSERT_FUNC __ASSERT_FUNCTION
#	elif defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#		define JSTR_ASSERT_FUNC __func__
#	else
#		define JSTR_ASSERT_FUNC ""
#	endif

#	ifdef __ASSERT_FILE
#		define JSTR_ASSERT_FILE __ASSERT_FILE
#	else
#		define JSTR_ASSERT_FILE __FILE__
#	endif

#	ifdef __ASSERT_LINE
#		define JSTR_ASSERT_LINE __ASSERT_LINE
#	else
#		define JSTR_ASSERT_LINE __LINE__
#	endif

#	if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#		define JSTR_RESTRICT restrict
#	elif defined __GNUC__ || defined __clang__
#		define JSTR_RESTRICT __restrict
#	elif defined _MSC_VER
#		define JSTR_RESTRICT __restrict
#	else
#		define JSTR_RESTRICT
#	endif /* restrict */

#	ifdef __glibc_has_attribute
#		define JSTR_HAS_ATTRIBUTE(attr) __glibc_has_attribute(attr)
#	elif defined __has_attribute
#		define JSTR_HAS_ATTRIBUTE(attr) __has_attribute(attr)
#	else
#		define JSTR_HAS_ATTRIBUTE(attr) 0
#	endif /* has_attribute */

#	ifdef __glibc_has_builtin
#		define JSTR_HAS_BUILTIN(name) __glibc_has_builtin(name)
#	elif defined __has_builtin
#		define JSTR_HAS_BUILTIN(name) __has_builtin(name)
#	else
#		define JSTR_HAS_BUILTIN(name) 0
#	endif /* has_builtin */

#	ifdef __glibc_has_extension
#		define JSTR_HAS_EXTENSION(ext) __glibc_has_extension(ext)
#	elif defined __has_extension
#		define JSTR_HAS_EXTENSION(ext) __has_extension(ext)
#	else
#		define JSTR_HAS_EXTENSION(ext)
#	endif /* has_extension */

#	if defined __glibc_unlikely && defined __glibc_likely
#		define jstr_likely(x)   __glibc_likely(x)
#		define jstr_unlikely(x) __glibc_unlikely(x)
#	elif ((defined __GNUC__ && (__GNUC__ >= 3)) || defined __clang__) && JSTR_HAS_BUILTIN(__builtin_expect)
#		define jstr_likely(x)   __builtin_expect((x), 1)
#		define jstr_unlikely(x) __builtin_expect((x), 0)
#	else
#		define jstr_likely(x)   (x)
#		define jstr_unlikely(x) (x)
#	endif /* unlikely */

#	ifndef JSTR__ATTR_INLINE
#		ifdef __inline
#			define JSTR__ATTR_INLINE __inline
#		elif (defined __cplusplus || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L))
#			define JSTR__ATTR_INLINE inline
#		else
#			define JSTR__ATTR_INLINE
#		endif
#	endif

#	if defined _MSC_VER
#		define JSTR_ATTR_NOINLINE __declspec(noinline)
#		define JSTR_ATTR_PURE     __declspec(noalias)
#		define JSTR_ATTR_CONST    __declspec(restrict)
#		define JSTR_ATTR_NOTHROW  __declspec(nothrow)
#	else
#		if JSTR_HAS_ATTRIBUTE(__format__)
#			define JSTR_ATTR_FORMAT(archetype, string_index, first_to_check) __attribute__((__format__(archetype, string_index, first_to_check)))
#		endif
#		ifdef __always_inline
#			define JSTR_ATTR_INLINE __always_inline
#		elif JSTR_HAS_ATTRIBUTE(__always_inline__)
#			define JSTR_ATTR_INLINE __attribute__((__always_inline__)) JSTR__ATTR_INLINE
#		endif
#		ifdef __attribute_noinline__
#			define JSTR_ATTR_NOINLINE __attribute_noinline__
#		elif JSTR_HAS_ATTRIBUTE(__noinline__)
#			define JSTR_ATTR_NOINLINE __attribute__((__noinline__))
#		endif
#		ifdef __attribute_pure__
#			define JSTR_ATTR_PURE __attribute_pure__
#		elif JSTR_HAS_ATTRIBUTE(__pure__)
#			define JSTR_ATTR_PURE __attribute__((__pure__))
#		endif
#		ifdef __attribute_const__
#			define JSTR_ATTR_CONST __attribute_const__
#		elif JSTR_HAS_ATTRIBUTE(__const__)
#			define JSTR_ATTR_CONST __attribute__((__const__))
#		endif
#		ifdef __attribute_flatten__
#			define JSTR_ATTR_FLATTEN __attribute_flatten__
#		elif JSTR_HAS_ATTRIBUTE(__flatten__)
#			define JSTR_ATTR_FLATTEN __attribute__((__flatten__))
#		endif
#		ifdef __COLD
#			define JSTR_ATTR_COLD __COLD
#		elif JSTR_HAS_ATTRIBUTE(__cold__)
#			define JSTR_ATTR_COLD __attribute__((__cold__))
#		endif
#		if JSTR_HAS_ATTRIBUTE(__sentinel__)
#			define JSTR_ATTR_SENTINEL __attribute__((__sentinel__))
#		endif
#		if defined __attribute_nonnull__ && defined __nonnull
#			define JSTR_ATTR_NONNULL(params) __nonnull(params)
#		elif JSTR_HAS_ATTRIBUTE(__nonnull__)
#			define JSTR_ATTR_NONNULL_ALL     __attribute__((__nonnull__))
#			define JSTR_ATTR_NONNULL(params) __attribute__((__nonnull__ params))
#		endif
#		if JSTR_HAS_ATTRIBUTE(__malloc__)
#			define JSTR_ATTR_MALLOC                                   __attribute__((__malloc__))
#			define JSTR_ATTR_MALLOC_DEALLOC(deallocator)              __attribute__((__malloc__, deallocator))
#			define JSTR_ATTR_MALLOC_DEALLOC_PTR(deallocator, ptr_idx) __attribute__((__malloc__, deallocator, ptr_idx))
#		endif
#		if JSTR_HAS_ATTRIBUTE(__returns_nonnull__)
#			define JSTR_ATTR_RETURNS_NONNULL __attribute__((__returns_nonnull__))
#		endif
#		ifdef __attribute_warn_unused_result__
#			define JSTR_ATTR_WARN_UNUSED __attribute_warn_unused_result__
#		elif JSTR_HAS_ATTRIBUTE(__warn_unused_result__)
#			define JSTR_ATTR_WARN_UNUSED __attribute__((__warn_unused_result__))
#		endif
#		if JSTR_HAS_ATTRIBUTE(__deprecated__)
#			define JSTR_ATTR_DEPRECATED(msg) __attribute__((__deprecated__(msg)))
#		endif
#		if JSTR_HAS_ATTRIBUTE(__nothrow__)
#			define JSTR_ATTR_NOTHROW __attribute__((__nothrow__))
#		endif
#		if JSTR_HAS_ATTRIBUTE(__may_alias__)
#			define JSTR_ATTR_MAY_ALIAS __attribute__((__may_alias__))
#			ifndef JSTR_HAVE_ATTR_MAY_ALIAS
#				define JSTR_HAVE_ATTR_MAY_ALIAS 1
#			endif
#		endif
#		ifdef __attr_access
#			define JSTR_ATTR_ACCESS(x) __attr_access(x)
#		elif JSTR_HAS_ATTRIBUTE(__access__)
#			define JSTR_ATTR_ACCESS(x) __attribute__((__access__ x))
#		endif
#		ifdef __attribute_maybe_unused__
#			define JSTR_ATTR_MAYBE_UNUSED __attribute_maybe_unused__
#		elif JSTR_HAS_ATTRIBUTE(__unused__)
#			define JSTR_ATTR_MAYBE_UNUSED __attribute__((__unused__))
#		endif
#		if JSTR_HAS_BUILTIN(__builtin_constant_p)
#			define JSTR_ATTR_BUILTIN_CONSTANT_P(p) __builtin_constant_p(p)
#		endif
#		if JSTR_HAS_ATTRIBUTE(__no_sanitize_address__)
#			define JSTR_ATTR_NO_SANITIZE_ADDRESS __attribute__((__no_sanitize_address__))
#		elif JSTR_HAS_ATTRIBUTE(__no_sanitize__)
#			define JSTR_ATTR_NO_SANITIZE_ADDRESS __attribute__((__no_sanitize__(("address"))))
#		endif
#	endif

#	ifndef JSTR_ATTR_INLINE
#		define JSTR_ATTR_INLINE JSTR__ATTR_INLINE
#	endif
#	ifndef JSTR_ATTR_NOINLINE
#		define JSTR_ATTR_NOINLINE
#	endif
#	ifndef JSTR_ATTR_PURE
#		define JSTR_ATTR_PURE
#	endif
#	ifndef JSTR_ATTR_CONST
#		define JSTR_ATTR_CONST
#	endif
#	ifndef JSTR_ATTR_NOTHROW
#		define JSTR_ATTR_NOTHROW
#	endif
#	ifndef JSTR_ATTR_NOINLINE
#		define JSTR_ATTR_NOINLINE
#	endif
#	ifndef JSTR_ATTR_FLATTEN
#		define JSTR_ATTR_FLATTEN
#	endif
#	ifndef JSTR_ATTR_COLD
#		define JSTR_ATTR_COLD
#	endif
#	ifndef JSTR_ATTR_SENTINEL
#		define JSTR_ATTR_SENTINEL
#	endif
#	ifndef JSTR_ATTR_NONNULL_ALL
#		define JSTR_ATTR_NONNULL_ALL
#	endif
#	ifndef JSTR_NONNULL
#		define JSTR_NONNULL(args)
#	endif
#	ifndef JSTR_ATTR_MALLOC
#		define JSTR_ATTR_MALLOC
#	endif
#	ifndef JSTR_ATTR_MALLOC_DEALLOC
#		define JSTR_ATTR_MALLOC_DEALLOC(deallocator)
#	endif
#	ifndef JSTR_ATTR_MALLOC_DEALLOC_PTR
#		define JSTR_ATTR_MALLOC_DEALLOC_PTR(deallocator, ptr_idx)
#	endif
#	ifndef JSTR_ATTR_RETURNS_NONNULL
#		define JSTR_ATTR_RETURNS_NONNULL
#	endif
#	ifndef JSTR_ATTR_WARN_UNUSED
#		define JSTR_ATTR_WARN_UNUSED
#	endif
#	ifndef JSTR_ATTR_DEPRECATED
#		define JSTR_ATTR_DEPRECATED(msg, replacement)
#	endif
#	ifndef JSTR_ATTR_FORMAT
#		define JSTR_ATTR_FORMAT(archetype, string_index, first_to_check)
#	endif
#	ifndef JSTR_ATTR_COLD
#		define JSTR_ATTR_COLD
#	endif
#	ifndef JSTR_ATTR_SENTINEL
#		define JSTR_ATTR_SENTINEL
#	endif
#	ifndef JSTR_ATTR_NONNULL_ALL
#		define JSTR_ATTR_NONNULL_ALL
#	endif
#	ifndef JSTR_NONNULL
#		define JSTR_NONNULL(args)
#	endif
#	ifndef JSTR_ATTR_RETURNS_NONNULL
#		define JSTR_ATTR_RETURNS_NONNULL
#	endif
#	ifndef JSTR_ATTR_WARN_UNUSED
#		define JSTR_ATTR_WARN_UNUSED
#	endif
#	ifndef JSTR_ATTR_DEPRECATED
#		define JSTR_ATTR_DEPRECATED(msg, replacement)
#	endif
#	ifndef JSTR_ATTR_MAY_ALIAS
#		define JSTR_ATTR_MAY_ALIAS
#	endif
#	ifndef JSTR_ATTR_MAYBE_UNUSED
#		define JSTR_ATTR_MAYBE_UNUSED
#	endif
#	ifndef JSTR_ATTR_BUILTIN_CONSTANT_P
#	endif
#	ifndef JSTR_ATTR_ACCESS
#		define JSTR_ATTR_ACCESS(x)
#	endif
#	ifndef JSTR_ATTR_NO_SANITIZE_ADDRESS
#		define JSTR_ATTR_NO_SANITIZE_ADDRESS
#	endif

#	define JSTR_ATTR_WUR           JSTR_ATTR_WARN_UNUSED
#	define JSTR_FUNC_VOID_MAY_NULL JSTR_ATTR_NOTHROW JSTR_ATTR_MAYBE_UNUSED
#	define JSTR_FUNC_VOID          JSTR_FUNC_VOID_MAY_NULL JSTR_ATTR_NONNULL_ALL
#	define JSTR_FUNC_CONST         JSTR_FUNC_VOID JSTR_ATTR_WUR JSTR_ATTR_CONST
#	define JSTR_FUNC_PURE          JSTR_FUNC_VOID JSTR_ATTR_WUR JSTR_ATTR_PURE
#	define JSTR_FUNC_PURE_MAY_NULL JSTR_FUNC_VOID_MAY_NULL JSTR_ATTR_WUR JSTR_ATTR_PURE
#	if JSTR_PANIC
#		define JSTR_FUNC_MAY_NULL    JSTR_FUNC_VOID_MAY_NULL
#		define JSTR_FUNC             JSTR_FUNC_VOID
#		define JSTR_FUNC_RET_NONNULL JSTR_FUNC_VOID JSTR_ATTR_RETURNS_NONNULL
#	else
#		define JSTR_FUNC_MAY_NULL    JSTR_FUNC_VOID_MAY_NULL JSTR_ATTR_WUR
#		define JSTR_FUNC             JSTR_FUNC_MAY_NULL JSTR_ATTR_NONNULL_ALL
#		define JSTR_FUNC_RET_NONNULL JSTR_FUNC JSTR_ATTR_RETURNS_NONNULL
#	endif

JSTR_FUNC_VOID_MAY_NULL
JSTR_NONNULL((1))
JSTR_NONNULL((3))
JSTR_ATTR_NOINLINE
JSTR_ATTR_COLD
static void
jstr__errdie(const char *JSTR_RESTRICT filename, const unsigned int line, const char *JSTR_RESTRICT func, const char *JSTR_RESTRICT msg)
JSTR_NOEXCEPT
{
	fprintf(stderr, "%s:%u:%s:%s:%s\n", filename, line, func, strerror(errno), msg);
	exit(EXIT_FAILURE);
}

JSTR_FUNC_VOID_MAY_NULL
JSTR_NONNULL((1))
JSTR_NONNULL((3))
JSTR_ATTR_NOINLINE
JSTR_ATTR_COLD
static void
jstr__err(const char *JSTR_RESTRICT filename, const unsigned int line, const char *JSTR_RESTRICT func, const char *JSTR_RESTRICT msg)
JSTR_NOEXCEPT
{
	fprintf(stderr, "%s:%u:%s:%s:%s\n", filename, line, func, strerror(errno), msg);
}

#	if defined _POSIX_SOURCE || defined _POSIX_C_SOURCE || defined _XOPEN_SOURCE || defined _GNU_SOURCE || defined _BSD_SOURCE
#		define JSTR_HAVE_FILENO 1
#	endif

#	if (defined __GLIBC__ && JSTR_GLIBC_PREREQ(2, 20) && defined _DEFAULT_SOURCE) \
	|| defined _BSD_SOURCE
#		define JSTR_HAVE_STRCASECMP  1
#		define JSTR_HAVE_STRNCASECMP 1
#	endif /* Bsd || Default */

#	if defined _XOPEN_SOURCE || defined _ISOC2X_SOURCE
#		define JSTR_HAVE_MEMCCPY 1
#	endif /* Misc || Xopen */

#	ifdef _GNU_SOURCE
#		define JSTR_HAVE_MEMMEM            1
#		define JSTR_HAVE_MEMRCHR           1
#		define JSTR_HAVE_STRCHRNUL         1
#		define JSTR_HAVE_FGETS_UNLOCKED    1
#		define JSTR_HAVE_FPUTS_UNLOCKED    1
#		define JSTR_HAVE_GETWC_UNLOCKED    1
#		define JSTR_HAVE_GETWCHAR_UNLOCKED 1
#		define JSTR_HAVE_FGETWC_UNLOCKED   1
#		define JSTR_HAVE_FPUTWC_UNLOCKED   1
#		define JSTR_HAVE_PUTWCHAR_UNLOCKED 1
#		define JSTR_HAVE_FGETWS_UNLOCKED   1
#		define JSTR_HAVE_FPUTWS_UNLOCKED   1
#		define JSTR_HAVE_WMEMPCPY          1
#		define JSTR_HAVE_MEMPCPY           1
#		define JSTR_HAVE_STRCASESTR        1
#	endif /* Gnu */

#	if (defined __GLIBC__ && JSTR_GLIBC_PREREQ(2, 24) && (_POSIX_C_SOURCE - 0) >= 199309L)                                                 \
	|| ((defined __GLIBC__ && __GLIBC__ == 2 && __GLIBC_MINOR__ <= 19) && defined _SVID_SOURCE || defined _BSD_SOURCE) \
	|| (defined __GLIBC__ && __GLIBC__ == 2 && __GLIBC_MINOR__ <= 23 && defined _POSIX_C_SOURCE)
#		define JSTR_HAVE_GETC_UNLOCKED    1
#		define JSTR_HAVE_GETCHAR_UNLOCKED 1
#		define JSTR_HAVE_PUTC_UNLOCKED    1
#		define JSTR_HAVE_PUTCHAR_UNLOCKED 1
#	endif /* Posix || Bsd  */

#	if (defined __GLIBC__ && JSTR_GLIBC_PREREQ(2, 19) && defined _DEFAULT_SOURCE) \
	|| defined _SVID_SOURCE || defined _BSD_SOURCE
#		define JSTR_HAVE_FREAD_UNLOCKED    1
#		define JSTR_HAVE_FWRITE_UNLOCKED   1
#		define JSTR_HAVE_FPUTC_UNLOCKED    1
#		define JSTR_HAVE_FGETC_UNLOCKED    1
#		define JSTR_HAVE_CLEARERR_UNLOCKED 1
#		define JSTR_HAVE_FEOF_UNLOCKED     1
#		define JSTR_HAVE_FERROR_UNLOCKED   1
#		define JSTR_HAVE_FILENO_UNLOCKED   1
#		define JSTR_HAVE_FFLUSH_UNLOCKED   1
#	endif /* Default || Svid || Bsd */

#	if (defined __GLIBC__ && JSTR_GLIBC_PREREQ(2, 10) && (_POSIX_C_SOURCE - 0) >= 200809L) \
	|| defined _GNU_SOURCE
#		define JSTR_HAVE_STPCPY  1
#		define JSTR_HAVE_STRNLEN 1
#		define JSTR_HAVE_STRNDUP 1
#	endif /* Posix || Gnu */

#	if (defined _XOPEN_SOURCE && (_XOPEN_SOURCE - 0) >= 500)          \
	|| (defined __GLIBC__ && JSTR_GLIBC_PREREQ(2, 12) && (__POSIX_C_SOURCE - 0) >= 200809L) \
	|| defined _BSD_SOURCE || defined _SVID_SOURCE
#		define JSTR_HAVE_STRDUP 1
#	endif /* Xopen || Bsd || Svid || Posix */

#	ifdef _GNU_SOURCE
#		define JSTR_HAVE_STRDUPA  1
#		define JSTR_HAVE_STRNDUPA 1
#	endif /* Gnu */

#	if (defined __GLIBC__ && (__GLIBC__ < 2 || __GLIBC__ == 2 && __GLIBC_MINOR__ <= 19) && defined _BSD_SOURCE || defined _SVID_SOURCE) \
	|| (defined _POSIX_C_SOURCE && (_POSIX_C_SOURCE - 0) >= 2)
#		define JSTR_HAVE_POPEN  1
#		define JSTR_HAVE_PCLOSE 1
#	endif

#	if (defined __GLIBC__ && JSTR_GLIBC_PREREQ(2, 20) && defined _DEFAULT_SOURCE)                                                                    \
	|| (defined __GLIBC__ && JSTR_GLIBC_PREREQ(2, 19) && defined _SVID_SOURCE)                                                                        \
	|| (defined __GLIBC__ && JSTR_GLIBC_PREREQ(2, 17) && ((_XOPEN_SOURCE - 0) >= 500 && !((_POSIX_C_SOURCE - 0) >= 200809L)))                         \
	|| (defined __GLIBC__ && JSTR_GLIBC_PREREQ(2, 12) && ((_XOPEN_SOURCE - 0) >= 500 && !((_POSIX_C_SOURCE - 0) >= 200112L)) || defined _SVID_SOURCE) \
	|| (defined _SVID_SOURCE || (_XOPEN_SOURCE - 0) >= 500)
#		define JSTR_HAVE_GCVT 1
#		define JSTR_HAVE_ECVT 1
#		define JSTR_HAVE_FCVT 1
#	endif

#	if (defined __GLIBC__ && JSTR_GLIBC_PREREQ(2, 19) && defined _DEFAULT_SOURCE) \
	|| defined _SVID_SOURCE
#		define JSTR_HAVE_QECVT 1
#		define JSTR_HAVE_QFCVT 1
#		define JSTR_HAVE_QGCVT 1
#	endif

#	if (defined __GLIBC__ && JSTR_GLIBC_PREREQ(2, 19) && defined _DEFAULT_SOURCE) \
	|| (defined _SVID_SOURCE || defined _BSD_SOURCE)
#		define JSTR_HAVE_QECVT_R 1
#		define JSTR_HAVE_QFCVT_R 1
#		define JSTR_HAVE_ECVT_R  1
#		define JSTR_HAVE_FCVT_R  1
#	endif

#	if defined _DEFAULT_SOURCE || !defined __USE_XOPEN2K8
#		define JSTR_HAVE_BCMP  1
#		define JSTR_HAVE_BCOPY 1
#		define JSTR_HAVE_BZERO 1
#	endif

#	ifdef _ATFILE_SOURCE
#		define JSTR_HAVE_ATFILE 1
#	endif

#	if defined _XOPEN_SOURCE && (defined _XOPEN_SOURCE_EXTENDED || (_XOPEN_SOURCE - 0) >= 500)
#		define JSTR_HAVE_FCHDIR 1
#	endif /* Xopen extended || Xopen 2k8 */

#	if defined _XOPEN_SOURCE && (_XOPEN_SOURCE - 0) >= 700
#		define JSTR_HAVE_FDOPENDIR 1
#	endif /* Xopen 2k8 */

#	if defined _POSIX_C_SOURCE && _POSIX_C_SOURCE >= 199309L
#		define JSTR_HAVE_NANOSLEEP
#endif /* Posix 199309L */

#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_READ) && JSTR_HAVE_FREAD_UNLOCKED
#		define jstr_io_fread(ptr, size, n, stream) fread_unlocked(ptr, size, n, stream)
#	else
#		define jstr_io_fread(ptr, size, n, stream) fread(ptr, size, n, stream)
#	endif
#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_READ) && JSTR_HAVE_FGETC_UNLOCKED
#		define jstr_io_fgetc(stream) fgetc_unlocked(stream)
#	else
#		define jstr_io_fgetc(stream) fgetc(stream)
#	endif
#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_READ) && JSTR_HAVE_FGETWS_UNLOCKED
#		define jstr_io_fgetws(ws, n, stream) fgetws_unlocked(ws, n, stream)
#	else
#		define jstr_io_fgetws(ws, n, stream) fgetws(ws, n, stream)
#	endif
#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_READ) && JSTR_HAVE_GETC_UNLOCKED
#		define jstr_io_getc(stream) getc_unlocked(stream)
#	else
#		define jstr_io_getc(stream) getc(stream)
#	endif
#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_READ) && JSTR_HAVE_GETCHAR_UNLOCKED
#		define jstr_io_getchar() getchar_unlocked()
#	else
#		define jstr_io_getchar() getchar()
#	endif
#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_READ) && JSTR_HAVE_FGETS_UNLOCKED
#		define jstr_io_fgets(s, n, stream) fgets_unlocked(s, n, stream)
#	else
#		define jstr_io_fgets(s, n, stream) fgets(s, n, stream)
#	endif
#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_READ) && JSTR_HAVE_GETWC_UNLOCKED
#		define jstr_io_getwc(stream) getwc_unlocked(stream)
#	else
#		define jstr_io_getwc(stream) getwc(stream)
#	endif
#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_READ) && JSTR_HAVE_GETWCHAR_UNLOCKED
#		define jstr_io_getwchar() getwchar_unlocked()
#	else
#		define jstr_io_getwchar() getwchar()
#	endif
#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_READ) && JSTR_HAVE_FGETWC_UNLOCKED
#		define jstr_io_fgetwc(stream) fgetwc_unlocked(stream)
#	else
#		define jstr_io_fgetwc(stream) fgetwc(stream)
#	endif

#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_WRITE) && JSTR_HAVE_FPUTWC_UNLOCKED
#		define jstr_io_fputwc(wc, stream) fputwc_unlocked(wc, stream)
#	else
#		define jstr_io_fputwc(wc, stream) fputwc(wc, stream)
#	endif
#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_WRITE) && JSTR_HAVE_PUTWCHAR_UNLOCKED
#		define jstr_io_putwchar(wc) putwchar_unlocked(wc)
#	else
#		define jstr_io_putwchar(wc) putwchar(wc)
#	endif
#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_WRITE) && JSTR_HAVE_FPUTS_UNLOCKED
#		define jstr_io_fputs(s, stream) fputs_unlocked(s, stream)
#	else
#		define jstr_io_fputs(s, stream) fputs(s, stream)
#	endif
#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_WRITE) && JSTR_HAVE_FPUTWS_UNLOCKED
#		define jstr_io_fputws(ws, stream) fputws_unlocked(ws, stream)
#	else
#		define jstr_io_fputws(ws, stream) fputws(ws, stream)
#	endif
#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_WRITE) && JSTR_HAVE_PUTC_UNLOCKED
#		define jstr_io_putc(c, stream) putc_unlocked(c, stream)
#	else
#		define jstr_io_putc(c, stream) putc(c, stream)
#	endif
#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_WRITE) && JSTR_HAVE_PUTCHAR_UNLOCKED
#		define jstr_io_putchar(c) putchar_unlocked(c)
#	else
#		define jstr_io_putchar(c) putchar(c)
#	endif
#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_WRITE) && JSTR_HAVE_FWRITE_UNLOCKED
#		define jstr_io_fwrite(ptr, size, n, stream) fwrite_unlocked(ptr, size, n, stream)
#	else
#		define jstr_io_fwrite(ptr, size, n, stream) fwrite(ptr, size, n, stream)
#	endif
#	if (JSTR_USE_UNLOCKED_IO || JSTR_USE_UNLOCKED_IO_WRITE) && JSTR_HAVE_FPUTC_UNLOCKED
#		define jstr_io_fputc(c, stream) fputc_unlocked(c, stream)
#	else
#		define jstr_io_fputc(c, stream) fputc(c, stream)
#	endif

#	if JSTR_USE_UNLOCKED_IO && JSTR_HAVE_CLEARERR_UNLOCKED
#		define jstr_io_clearerr(stream) clearerr_unlocked(stream)
#	else
#		define jstr_io_clearerr(stream) clearerr(stream)
#	endif
#	if JSTR_USE_UNLOCKED_IO && JSTR_HAVE_FEOF_UNLOCKED
#		define jstr_io_feof(stream) feof_unlocked(stream)
#	else
#		define jstr_io_feof(stream) feof(stream)
#	endif
#	if JSTR_USE_UNLOCKED_IO && JSTR_HAVE_FERROR_UNLOCKED
#		define jstr_io_ferror(stream) ferror_unlocked(stream)
#	else
#		define jstr_io_ferror(stream) ferror(stream)
#	endif
#	if JSTR_USE_UNLOCKED_IO && JSTR_HAVE_FILENO_UNLOCKED
#		define jstr_io_fileno(stream) fileno_unlocked(stream)
#	else
#		define jstr_io_fileno(stream) fileno(stream)
#	endif
#	if JSTR_USE_UNLOCKED_IO && JSTR_HAVE_FFLUSH_UNLOCKED
#		define jstr_io_fflush(stream) fflush_unlocked(stream)
#	else
#		define jstr_io_fflush(stream) fflush(stream)
#	endif

#	ifdef __GLIBC__
#		ifdef _DIRENT_HAVE_D_TYPE
#			define JSTR_HAVE_DIRENT_D_TYPE 1
#		endif
#		ifdef _DIRENT_HAVE_D_RECLEN
#			define JSTR_HAVE_DIRENT_D_RECLEN 1
#		endif
#		ifdef _DIRENT_HAVE_D_NAMLEN
#			define JSTR_HAVE_DIRENT_D_NAMLEN 1
#		endif
#	endif /* dirent_have */

/* Check optimizations. */
#	ifdef __GLIBC__
#		if JSTR_HAVE_MEMMEM && JSTR_ARCH_S390
#			define JSTR_HAVE_MEMMEM_OPTIMIZED 1
#		endif
#		if JSTR_ARCH_S390 || JSTR_ARCH_SPARC || JSTR_ARCH_X86_32 || JSTR_ARCH_X86_64 || JSTR_ARCH_ARM64 || JSTR_ARCH_ARM6 || JSTR_ARCH_ARM6T2 || JSTR_ARCH_ALPHA || JSTR_ARCH_I386 || JSTR_ARCH_IA64 || JSTR_ARCH_LOONGARCH64 || JSTR_ARCH_M68K || JSTR_ARCH_POWERPC64 || JSTR_ARCH_POWERPC || JSTR_ARCH_POWERPC4 || JSTR_ARCH_POWERPC7 || JSTR_ARCH_POWERPC8 || JSTR_ARCH_POWERPC9 || JSTR_ARCH_POWERPC10
#			define JSTR_HAVE_MEMCHR_OPTIMIZED 1
#		endif
#		if JSTR_ARCH_X86_64 || JSTR_ARCH_S390
#			define JSTR_HAVE_STRSTR_OPTIMIZED 1
#		endif
#		if JSTR_ARCH_X86_64 || JSTR_ARCH_S390 || JSTR_ARCH_I386 || JSTR_ARCH_SPARC || JSTR_ARCH_POWERPC64 || JSTR_ARCH_POWERPC8
#			define JSTR_HAVE_STRCSPN_OPTIMIZED 1
#		endif
#		if JSTR_ARCH_X86_64 || JSTR_ARCH_S390 || JSTR_ARCH_I386 || JSTR_ARCH_SPARC
#			define JSTR_HAVE_STRPBRK_OPTIMIZED 1
#		endif
#		if JSTR_ARCH_ARM64 || JSTR_ARCH_ALPHA || JSTR_ARCH_I386 || JSTR_ARCH_IA64 || JSTR_ARCH_M68K || JSTR_ARCH_POWERPC64 || JSTR_ARCH_POWERPC || JSTR_ARCH_POWERPC7 || JSTR_ARCH_POWERPC8 || JSTR_ARCH_POWERPC4 || JSTR_ARCH_S390 || JSTR_ARCH_X86_64 || JSTR_ARCH_X86_32 || JSTR_ARCH_LOONGARCH64 || JSTR_ARCH_SPARC
#			define JSTR_HAVE_STRCHR_OPTIMIZED 1
#		endif
#		if JSTR_ARCH_ARM64 || JSTR_ARCH_ARM6 || JSTR_ARCH_ALPHA || JSTR_ARCH_I386 || JSTR_ARCH_POWERPC64 || JSTR_ARCH_POWERPC || JSTR_ARCH_POWERPC7 || JSTR_ARCH_POWERPC8 || JSTR_ARCH_POWERPC9 || JSTR_ARCH_IA64 || JSTR_ARCH_I386 || JSTR_ARCH_S390 || JSTR_ARCH_SPARC || JSTR_ARCH_X86_64 || JSTR_ARCH_X86_32 || JSTR_ARCH_LOONGARCH64 || JSTR_ARCH_CSKY
#			define JSTR_HAVE_STRCPY_OPTIMIZED 1
#		endif
#		if JSTR_ARCH_ARM6T2 || JSTR_ARCH_ARM6 || JSTR_ARCH_ARM || JSTR_ARCH_POWERPC64 || JSTR_ARCH_POWERPC || JSTR_ARCH_POWERPC7 || JSTR_ARCH_POWERPC8 || JSTR_ARCH_POWERPC9 || JSTR_ARCH_POWERPC4 || JSTR_ARCH_LOONGARCH64 || JSTR_ARCH_ALPHA || JSTR_ARCH_I386 || JSTR_ARCH_IA64 || JSTR_ARCH_X86_64 || JSTR_ARCH_X86_32 || JSTR_ARCH_SH || JSTR_ARCH_SPARC || JSTR_ARCH_CSKY
#			define JSTR_HAVE_STRLEN_OPTIMIZED 1
#		endif
#	endif /* have_optimized */

#	if JSTR_GNUC_PREREQ(7, 1) || defined __clang__
#		define JSTR_HAVE_BUILTIN_MEMCMP 1
#	endif

#	if JSTR_ARCH_I386 || JSTR_ARCH_X86_64 || defined __s390x__ || JSTR_ARCH_ARM64
#		define JSTR_HAVE_UNALIGNED_ACCESS 1
#	endif

#	ifndef JSTR_LP64
#		define JSTR_LP64 0
#	endif
#	ifndef JSTR_LP32
#		define JSTR_LP32 0
#	endif
#	ifndef JSTR_LP_UNKNOWN
#		define JSTR_LP_UNKNOWN 0
#	endif
#	ifndef JSTR_PANIC
#		define JSTR_PANIC 0
#	endif
#	ifndef JSTR_ENDIAN_BIG
#		define JSTR_ENDIAN_BIG 0
#	endif
#	ifndef JSTR_ENDIAN_LITTLE
#		define JSTR_ENDIAN_LITTLE 0
#	endif
#	ifndef JSTR_ENDIAN_UNKNOWN
#		define JSTR_ENDIAN_UNKNOWN 0
#	endif
#	ifndef JSTR_HAVE_STATIC_ASSERT
#		define JSTR_HAVE_STATIC_ASSERT 0
#	endif
#	ifndef JSTR_HAVE_STATIC_ASSERT
#		define JSTR_HAVE_STATIC_ASSERT 0
#	endif
#	ifndef JSTR_HAVE_VLA
#		define JSTR_HAVE_VLA 0
#	endif
#	ifndef JSTR_HAVE_ALLOCA
#		define JSTR_HAVE_ALLOCA 0
#	endif
#	ifndef JSTR_HAVE_GENERIC
#		define JSTR_HAVE_GENERIC 0
#	endif
#	ifndef JSTR_HAVE_TYPEOF
#		define JSTR_HAVE_TYPEOF 0
#	endif
#	ifndef JSTR_HAVE_ATTR_MAY_ALIAS
#		define JSTR_HAVE_ATTR_MAY_ALIAS 0
#	endif
#	ifndef JSTR_HAVE_FILENO
#		define JSTR_HAVE_FILENO 0
#	endif
#	ifndef JSTR_HAVE_STRCASECMP
#		define JSTR_HAVE_STRCASECMP 0
#	endif
#	ifndef JSTR_HAVE_STRNCASECMP
#		define JSTR_HAVE_STRNCASECMP 0
#	endif
#	ifndef JSTR_HAVE_MEMCCPY
#		define JSTR_HAVE_MEMCCPY 0
#	endif
#	ifndef JSTR_HAVE_MEMMEM
#		define JSTR_HAVE_MEMMEM 0
#	endif
#	ifndef JSTR_HAVE_MEMRCHR
#		define JSTR_HAVE_MEMRCHR 0
#	endif
#	ifndef JSTR_HAVE_STRCHRNUL
#		define JSTR_HAVE_STRCHRNUL 0
#	endif
#	ifndef JSTR_HAVE_FGETS_UNLOCKED
#		define JSTR_HAVE_FGETS_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_FPUTS_UNLOCKED
#		define JSTR_HAVE_FPUTS_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_GETWC_UNLOCKED
#		define JSTR_HAVE_GETWC_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_GETWCHAR_UNLOCKED
#		define JSTR_HAVE_GETWCHAR_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_FGETWC_UNLOCKED
#		define JSTR_HAVE_FGETWC_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_FPUTWC_UNLOCKED
#		define JSTR_HAVE_FPUTWC_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_PUTWCHAR_UNLOCKED
#		define JSTR_HAVE_PUTWCHAR_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_FGETWS_UNLOCKED
#		define JSTR_HAVE_FGETWS_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_FPUTWS_UNLOCKED
#		define JSTR_HAVE_FPUTWS_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_WMEMPCPY
#		define JSTR_HAVE_WMEMPCPY 0
#	endif
#	ifndef JSTR_HAVE_MEMPCPY
#		define JSTR_HAVE_MEMPCPY 0
#	endif
#	ifndef JSTR_HAVE_STRCASESTR
#		define JSTR_HAVE_STRCASESTR 0
#	endif
#	ifndef JSTR_HAVE_GETC_UNLOCKED
#		define JSTR_HAVE_GETC_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_GETCHAR_UNLOCKED
#		define JSTR_HAVE_GETCHAR_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_PUTC_UNLOCKED
#		define JSTR_HAVE_PUTC_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_PUTCHAR_UNLOCKED
#		define JSTR_HAVE_PUTCHAR_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_FREAD_UNLOCKED
#		define JSTR_HAVE_FREAD_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_FWRITE_UNLOCKED
#		define JSTR_HAVE_FWRITE_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_FPUTC_UNLOCKED
#		define JSTR_HAVE_FPUTC_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_FGETC_UNLOCKED
#		define JSTR_HAVE_FGETC_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_CLEARERR_UNLOCKED
#		define JSTR_HAVE_CLEARERR_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_FEOF_UNLOCKED
#		define JSTR_HAVE_FEOF_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_FERROR_UNLOCKED
#		define JSTR_HAVE_FERROR_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_FILENO_UNLOCKED
#		define JSTR_HAVE_FILENO_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_FFLUSH_UNLOCKED
#		define JSTR_HAVE_FFLUSH_UNLOCKED 0
#	endif
#	ifndef JSTR_HAVE_STPCPY
#		define JSTR_HAVE_STPCPY 0
#	endif
#	ifndef JSTR_HAVE_STRNLEN
#		define JSTR_HAVE_STRNLEN 0
#	endif
#	ifndef JSTR_HAVE_STRNDUP
#		define JSTR_HAVE_STRNDUP 0
#	endif
#	ifndef JSTR_HAVE_STRDUP
#		define JSTR_HAVE_STRDUP 0
#	endif
#	ifndef JSTR_HAVE_STRDUPA
#		define JSTR_HAVE_STRDUPA 0
#	endif
#	ifndef JSTR_HAVE_STRNDUPA
#		define JSTR_HAVE_STRNDUPA 0
#	endif
#	ifndef JSTR_HAVE_POPEN
#		define JSTR_HAVE_POPEN 0
#	endif
#	ifndef JSTR_HAVE_PCLOSE
#		define JSTR_HAVE_PCLOSE 0
#	endif
#	ifndef JSTR_HAVE_GCVT
#		define JSTR_HAVE_GCVT 0
#	endif
#	ifndef JSTR_HAVE_ECVT
#		define JSTR_HAVE_ECVT 0
#	endif
#	ifndef JSTR_HAVE_FCVT
#		define JSTR_HAVE_FCVT 0
#	endif
#	ifndef JSTR_HAVE_QECVT
#		define JSTR_HAVE_QECVT 0
#	endif
#	ifndef JSTR_HAVE_QFCVT
#		define JSTR_HAVE_QFCVT 0
#	endif
#	ifndef JSTR_HAVE_QGCVT
#		define JSTR_HAVE_QGCVT 0
#	endif
#	ifndef JSTR_HAVE_QECVT_R
#		define JSTR_HAVE_QECVT_R 0
#	endif
#	ifndef JSTR_HAVE_QFCVT_R
#		define JSTR_HAVE_QFCVT_R 0
#	endif
#	ifndef JSTR_HAVE_ECVT_R
#		define JSTR_HAVE_ECVT_R 0
#	endif
#	ifndef JSTR_HAVE_FCVT_R
#		define JSTR_HAVE_FCVT_R 0
#	endif
#	ifndef JSTR_HAVE_BCMP
#		define JSTR_HAVE_BCMP 0
#	endif
#	ifndef JSTR_HAVE_BCOPY
#		define JSTR_HAVE_BCOPY 0
#	endif
#	ifndef JSTR_HAVE_BZERO
#		define JSTR_HAVE_BZERO 0
#	endif
#	ifndef JSTR_HAVE_ATFILE
#		define JSTR_HAVE_ATFILE 0
#	endif
#	ifndef JSTR_HAVE_FCHDIR
#		define JSTR_HAVE_FCHDIR 0
#	endif
#	ifndef JSTR_HAVE_FDOPENDIR
#		define JSTR_HAVE_FDOPENDIR 0
#	endif
#	ifndef JSTR_HAVE_DIRENT_D_TYPE
#		define JSTR_HAVE_DIRENT_D_TYPE 0
#	endif
#	ifndef JSTR_HAVE_DIRENT_D_RECLEN
#		define JSTR_HAVE_DIRENT_D_RECLEN 0
#	endif
#	ifndef JSTR_HAVE_DIRENT_D_NAMLEN
#		define JSTR_HAVE_DIRENT_D_NAMLEN 0
#	endif
#	ifndef JSTR_HAVE_MEMMEM_OPTIMIZED
#		define JSTR_HAVE_MEMMEM_OPTIMIZED 0
#	endif
#	ifndef JSTR_HAVE_MEMCHR_OPTIMIZED
#		define JSTR_HAVE_MEMCHR_OPTIMIZED 0
#	endif
#	ifndef JSTR_HAVE_STRSTR_OPTIMIZED
#		define JSTR_HAVE_STRSTR_OPTIMIZED 0
#	endif
#	ifndef JSTR_HAVE_STRCASESTR_OPTIMIZED
#		define JSTR_HAVE_STRCASESTR_OPTIMIZED 0
#	endif
#	ifndef JSTR_HAVE_STRCSPN_OPTIMIZED
#		define JSTR_HAVE_STRCSPN_OPTIMIZED 0
#	endif
#	ifndef JSTR_HAVE_STRPBRK_OPTIMIZED
#		define JSTR_HAVE_STRPBRK_OPTIMIZED 0
#	endif
#	ifndef JSTR_HAVE_STRCHR_OPTIMIZED
#		define JSTR_HAVE_STRCHR_OPTIMIZED 0
#	endif
#	ifndef JSTR_HAVE_STRCPY_OPTIMIZED
#		define JSTR_HAVE_STRCPY_OPTIMIZED 0
#	endif
#	ifndef JSTR_HAVE_STRLEN_OPTIMIZED
#		define JSTR_HAVE_STRLEN_OPTIMIZED 0
#	endif
#	ifndef JSTR_HAVE_BUILTIN_MEMCMP
#		define JSTR_HAVE_BUILTIN_MEMCMP 0
#	endif
#	ifndef JSTR_HAVE_UNALIGNED_ACCESS
#		define JSTR_HAVE_UNALIGNED_ACCESS 0
#	endif

#endif /* JSTR_MACROS_H */
