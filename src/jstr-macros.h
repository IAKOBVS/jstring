/* See LICENSE file for copyright and license details. */

#ifndef JSTR_MACROS_H
#define JSTR_MACROS_H 1

#include "jstr-config.h"
#include "jstr-pointer-arith.h"
#include <assert.h>
#include <features.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/cdefs.h>

#ifdef __cplusplus
#	define PJSTR_BEGIN_DECLS extern "C" {
#	define PJSTR_END_DECLS   }
#else
#	define PJSTR_BEGIN_DECLS
#	define PJSTR_END_DECLS
#endif

PJSTR_BEGIN_DECLS
typedef enum {
	JSTR_ERR = 0,
	JSTR_SUCC
} jstr_ret_ty;
PJSTR_END_DECLS

#define jstr_chk(ret) jstr_unlikely(ret == JSTR_ERR)

#if JSTR_DEBUG
#	define JSTR_PRINT_LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#	define JSTR_PRINT_LOG(...) \
		do {                \
		} while (0)
#endif

#define JSTR_CONCAT_HELPER(x, y) x##y
#define JSTR_CONCAT(x, y)        JSTR_CONCAT_HELPER(x, y)
#define JSTR_STRINGIFY(x)        #x

#define JSTR_ARRAY_SIZE(array) (sizeof(array) / sizeof(*(array)))

#define JSTR_ALIGN_UP_STR(base)       JSTR_ALIGN_UP((uintptr_t)base, PJSTR_MALLOC_ALIGNMENT)
#define JSTR_ALIGN_DOWN_STR(base)     JSTR_ALIGN_DOWN((uintptr_t)base, PJSTR_MALLOC_ALIGNMENT)
#define JSTR_PTR_IS_ALIGNED_STR(base) JSTR_PTR_IS_ALIGNED(base, PJSTR_MALLOC_ALIGNMENT)

#ifdef __cplusplus
template <typename T, typename Other>
static T
PJSTR_CAST(T, Other other)
{
	return (T)other;
}
#else
#	define PJSTR_CAST(T, other) (other)
#endif /* CAST */

#define JSTR_MEMSET_ARRAY(array, c) ((sizeof(array) == 256)                      \
	                             ? (memset((array), (c), 64),                \
	                                memset((array) + 64, (c), 64),           \
	                                memset((array) + 64 + 64, (c), 64),      \
	                                memset((array) + 64 + 64 + 64, (c), 64)) \
	                             : memset((array), (c), sizeof((array))))
#define JSTR_BZERO_ARRAY(array) JSTR_MEMSET_ARRAY(array, 0)

#if !(defined __STDC_VERSION__ && __STDC_VERSION__ > 201000L && __STDC_NO_VLA__)
#	define JSTR_HAVE_VLA 1
#endif

#if defined _GNU_SOURCE || defined alloca
#	define JSTR_HAVE_ALLOCA 1
#endif

#define JSTR_PAGE_SIZE 4096

#ifdef __GLIBC_PREREQ
#	define JSTR_GLIBC_PREREQ(maj, min) __GLIBC_PREREQ(maj, min)
#elif defined __GLIBC__
#	define JSTR_GLIBC_PREREQ(maj, min) \
		((__GLIBC__ << 16) + __GLIBC_MINOR__ >= ((maj) << 16) + (min))
#else
#	define JSTR_GLIBC_PREREQ(maj, min) 0
#endif

#define JSTR_MAX(x, y)        (((x) > (y)) ? (x) : (y))
#define JSTR_MIN(x, y)        (((x) < (y)) ? (x) : (y))
#define JSTR_BETWEEN(x, a, b) (((a) <= (x)) && ((x) <= (b)))
#define JSTR_MIN3(x, y, z)    (((x) < (y)) ? (((x) < (z)) ? (x) : (z)) : (((y) < (z)) ? (y) : (z)))
#define JSTR_MAX3(x, y, z)    (((x) > (y)) ? (((x) > (z)) ? (x) : (z)) : (((y) > (z)) ? (y) : (z)))
#define JSTR_MID3(x, y, z)    (((x) > (y)) ? (((x) < (z)) ? (x) : (z)) : (((y) < (z)) ? (y) : (z)))

#if !defined __cplusplus && defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L
#	define JSTR_HAVE_GENERIC 1
#endif /* HAVE_GENERIC */

#if (defined __GNUC__ && (__GNUC__ >= 4)) \
|| (defined __clang__ && (__clang_major__ >= 3))
#	define JSTR_HAVE_TYPEOF 1
#endif /* HAVE_TYPEOF */

#if JSTR_HAVE_TYPEOF && JSTR_HAVE_GENERIC
#	define JSTR_SAME_TYPE(x, y) _Generic((x), \
	__typeof__(y): 1,                          \
	default: 0)
#	define PJSTR_IS_TYPE(T, x) _Generic((x), \
	T: 1,                                     \
	default: 0)
#else
#	define JSTR_SAME_TYPE(x, y) 1
#	define PJSTR_IS_TYPE(T, x)  1
#endif /* HAVE_TYPEOF && HAVE_GENERIC */

#ifdef static_assert
#	include <assert.h>
#	define JSTR_HAVE_STATIC_ASSERT 1
#	define JSTR_ASSERT(expr, msg)  static_assert(expr, msg)
#elif defined _Static_assert || defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L
#	include <assert.h>
#	define JSTR_HAVE_STATIC_ASSERT 1
#	define JSTR_ASSERT(expr, msg)  _Static_assert(expr, msg)
#else
#	define JSTR_ASSERT(expr, msg) \
		do {                   \
		} while (0)
#endif /* static_assert */

#if (defined __GNUC__ || defined __clang__) && JSTR_HAVE_GENERIC
#	define JSTR_GENERIC_CASE_SIZE(expr)                           \
		int : expr,                                            \
		      unsigned int : expr,                             \
		                     size_t : expr,                    \
		                              long : expr,             \
		                                     long long : expr, \
		                                                 unsigned long long : expr
#	define JSTR_GENERIC_CASE_STR(bool_) \
		char * : bool_,              \
		         const char * : bool_
#	define JSTR_GENERIC_CASE_STR_STACK(bool_, s) \
		char(*)[sizeof(s)] : 1,               \
		const char(*)[sizeof(s)] : 1
#	define JSTR_GENERIC_CASE_CHAR(bool_) \
		char : bool_,                 \
		       const char : bool_
#	define PJSTR_IS_SIZE(expr) _Generic((expr), \
	JSTR_GENERIC_CASE_SIZE(1),                   \
	default: 0)
#	define PJSTR_IS_STR(expr) _Generic((expr), \
	JSTR_GENERIC_CASE_STR(1),                   \
	default: 0)
#	define PJSTR_IS_STR_STACK(expr) _Generic((expr), \
	JSTR_GENERIC_CASE_STR_STACK(1, expr),             \
	default: 0)
#	define PJSTR_IS_CHAR(expr) _Generic((expr), \
	JSTR_GENERIC_CASE_CHAR(1),                   \
	default: 0)
#	define JSTR_ASSERT_IS_SIZE(expr) \
		JSTR_ASSERT(PJSTR_IS_SIZE(expr), "Passing non-number as number argument!");
#	define JSTR_ASSERT_IS_STR(expr) \
		JSTR_ASSERT(PJSTR_IS_STR(expr), "Passing non-string as string argument!");
#	define JSTR_ASSERT_IS_CHAR(expr) \
		JSTR_ASSERT(PJSTR_IS_CHAR(expr), "Passing non-char as char argument!");
#	define JSTR_ASSERT_TYPECHECK(expr_ty, expr) \
		JSTR_ASSERT(JSTR_SAME_TYPE(expr_ty, expr), "Passing the wrong data type!");
#else
#	define JSTR_GENERIC_CASE_SIZE(expr)
#	define JSTR_GENERIC_CASE_STR(bool)
#	define JSTR_GENERIC_CASE_CHAR(bool)
#	define PJSTR_IS_SIZE(expr)
#	define PJSTR_IS_STR(expr)
#	define PJSTR_IS_STR_STACK(expr)
#	define PJSTR_IS_CHAR(expr)
#	define JSTR_ASSERT_IS_SIZE(expr)
#	define JSTR_ASSERT_IS_STR(expr)
#	define JSTR_ASSERT_IS_CHAR(expr)
#	define JSTR_ASSERT_TYPECHECK(expr_ty, expr)
#endif /* (Gnuc || clang) && HAVE_GENERIC */

#if defined __cplusplus && __cplusplus > 199711L
#	define JSTR_NOEXCEPT noexcept
#else
#	define JSTR_NOEXCEPT
#endif /* NOEXCEPT */

#ifdef __ASSERT_FUNCTION
#	define JSTR_ASSERT_FUNC __ASSERT_FUNCTION
#else
#	define JSTR_ASSERT_FUNC __func__
#endif

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#	define JSTR_RESTRICT restrict
#elif defined __GNUC__ || defined __clang__
#	define JSTR_RESTRICT __restrict__
#elif defined _MSC_VER
#	define JSTR_RESTRICT __restrict
#else
#	define JSTR_RESTRICT
#endif /* RESTRICT */

#ifdef __glibc_has_attribute
#	define JSTR_HAS_ATTRIBUTE(attr) __glibc_has_attribute(attr)
#elif defined __has_attribute
#	define JSTR_HAS_ATTRIBUTE(attr) __has_attribute(attr)
#else
#	define JSTR_HAS_ATTRIBUTE(attr) 0
#endif /* HAS_ATTRIBUTE */

#ifdef __glibc_has_builtin
#	define JSTR_HAS_BUILTIN(name) __glibc_has_builtin(name)
#elif defined __has_builtin
#	define JSTR_HAS_BUILTIN(name) __has_builtin(name)
#else
#	define JSTR_HAS_BUILTIN(name) 0
#endif /* HAS_BUILTIN */

#ifdef __glibc_has_extension
#	define JSTR_HAS_EXTENSION(ext) __glibc_has_extension(ext)
#elif defined __has_extension
#	define JSTR_HAS_EXTENSION(ext) __has_extension(ext)
#else
#	define JSTR_HAS_EXTENSION(ext) 0
#endif /* HAS_EXTENSION */

#if defined __glibc_unlikely && defined __glibc_likely
#	define jstr_likely(x)   __glibc_likely(x)
#	define jstr_unlikely(x) __glibc_unlikely(x)
#elif (defined __GNUC__ && (__GNUC__ >= 3)) || (defined __clang__)
#	if JSTR_HAS_BUILTIN(__builtin_expect)
#		define jstr_likely(x)   __builtin_expect((x), 1)
#		define jstr_unlikely(x) __builtin_expect((x), 0)
#	endif
#else
#	define jstr_likely(x)   (x)
#	define jstr_unlikely(x) (x)
#endif /* unlikely */

#if (defined __GNUC__ || defined __clang__) && (defined __glibc_has_attribute || defined __has_attribute)
#	if JSTR_HAS_ATTRIBUTE(__format__)
#		define JSTR_FORMAT(archetype, string_index, first_to_check) __attribute__((__format__(archetype, string_index, first_to_check)))
#	elif JSTR_HAS_ATTRIBUTE(format)
#		define JSTR_FORMAT(archetype, string_index, first_to_check) __attribute__((format(archetype, string_index, first_to_check)))
#	else
#		define JSTR_FORMAT(archetype, string_index, first_to_check)
#	endif
#	if JSTR_HAS_ATTRIBUTE(__always_inline__)
#		define JSTR_INLINE __attribute__((__always_inline__)) inline
#	elif JSTR_HAS_ATTRIBUTE(always_inline)
#		define JSTR_INLINE always_inline((always_inline)) inline
#	else
#		define JSTR_INLINE inline
#	endif
#	if JSTR_HAS_ATTRIBUTE(__noinline__)
#		define JSTR_NOINLINE __attribute__((__noinline__))
#	elif JSTR_HAS_ATTRIBUTE(noinline)
#		define JSTR_NOINLINE __attribute__((noinline))
#	else
#		define JSTR_NOINLINE
#	endif
#	if JSTR_HAS_ATTRIBUTE(__pure__)
#		define JSTR_PURE __attribute__((__pure__))
#	elif JSTR_HAS_ATTRIBUTE(pure)
#		define JSTR_PURE __attribute__((pure))
#	else
#		define JSTR_PURE
#	endif
#	if JSTR_HAS_ATTRIBUTE(__const__)
#		define JSTR_CONST __attribute__((__const__))
#	elif JSTR_HAS_ATTRIBUTE(const)
#		define JSTR_CONST __attribute__((const))
#	else
#		define JSTR_CONST
#	endif
#	if JSTR_HAS_ATTRIBUTE(__flatten__)
#		define JSTR_FLATTEN __attribute__((__flatten__))
#	elif JSTR_HAS_ATTRIBUTE(flatten)
#		define JSTR_FLATTEN __attribute__((flatten))
#	else
#		define JSTR_FLATTEN
#	endif
#	if JSTR_HAS_ATTRIBUTE(__cold__)
#		define JSTR_COLD __attribute__((__cold__))
#	elif JSTR_HAS_ATTRIBUTE(cold)
#		define JSTR_COLD __attribute__((cold))
#	else
#		define JSTR_COLD
#	endif
#	if JSTR_HAS_ATTRIBUTE(__sentinel__)
#		define JSTR_SENTINEL __attribute__((__sentinel__))
#	elif JSTR_HAS_ATTRIBUTE(sentinel)
#		define JSTR_SENTINEL __attribute__((sentinel))
#	else
#		define JSTR_SENTINEL
#	endif
#	if JSTR_HAS_ATTRIBUTE(__nonnull__)
#		define JSTR_NONNULL_ALL   __attribute__((__nonnull__))
#		define JSTR_NONNULL(args) __attribute__((__nonnull__(args)))
#	elif JSTR_HAS_ATTRIBUTE(nonnull)
#		define JSTR_NONNULL_ALL   __attribute__((nonnull))
#		define JSTR_NONNULL(args) __attribute__((nonnull(args)))
#	else
#		define JSTR_NONNULL_ALL
#		define JSTR_NONNULL(args)
#	endif
#	if JSTR_HAS_ATTRIBUTE(__malloc__)
#		define JSTR_MALLOC                                   __attribute__((__malloc__))
#		define JSTR_MALLOC_DEALLOC(deallocator)              __attribute__((__malloc__, deallocator))
#		define JSTR_MALLOC_DEALLOC_PTR(deallocator, ptr_idx) __attribute__((__malloc__, deallocator, ptr_idx))
#	elif JSTR_HAS_ATTRIBUTE(malloc)
#		define JSTR_MALLOC                                   __attribute__((malloc))
#		define JSTR_MALLOC_DEALLOC(deallocator)              __attribute__((malloc, deallocator))
#		define JSTR_MALLOC_DEALLOC_PTR(deallocator, ptr_idx) __attribute__((malloc, deallocator, ptr_idx))
#	else
#		define JSTR_MALLOC
#		define JSTR_MALLOC_DEALLOC(deallocator)
#		define JSTR_MALLOC_DEALLOC_PTR(deallocator, ptr_idx)
#	endif
#	if JSTR_HAS_ATTRIBUTE(__returns_nonnull__)
#		define JSTR_RETURNS_NONNULL __attribute__((__returns_nonnull__))
#	elif JSTR_HAS_ATTRIBUTE(returns_nonnull)
#		define JSTR_RETURNS_NONNULL __attribute__((returns_nonnull))
#	else
#		define JSTR_RETURNS_NONNULL
#	endif
#	if JSTR_HAS_ATTRIBUTE(__warn_unused_result__)
#		define JSTR_WARN_UNUSED __attribute__((__warn_unused_result__))
#	elif JSTR_HAS_ATTRIBUTE(warn_unused_result)
#		define JSTR_WARN_UNUSED __attribute__((warn_unused_result))
#	else
#		define JSTR_WARN_UNUSED
#	endif
#	if JSTR_HAS_ATTRIBUTE(__deprecated__)
#		define JSTR_DEPRECATED(msg) __attribute__((__deprecated__(msg)))
#	elif JSTR_HAS_ATTRIBUTE(deprecated)
#		define JSTR_DEPRECATED(msg) __attribute__((deprecated(msg)))
#	else
#		define JSTR_DEPRECATED(msg, replacement)
#	endif
#	if JSTR_HAS_ATTRIBUTE(__nothrow__)
#		define JSTR_NOTHROW __attribute__((__nothrow__))
#	elif JSTR_HAS_ATTRIBUTE(nothrow)
#		define JSTR_NOTHROW __attribute__((nothrow))
#	else
#		define JSTR_NOTHROW
#	endif
#	if JSTR_HAS_ATTRIBUTE(__may_alias__)
#		define JSTR_MAY_ALIAS           __attribute__((__may_alias__))
#		define JSTR_HAVE_ATTR_MAY_ALIAS 1
#	elif JSTR_HAS_ATTRIBUTE(may_alias)
#		define JSTR_MAY_ALIAS           __attribute__((may_alias))
#		define JSTR_HAVE_ATTR_MAY_ALIAS 1
#	else
#		define JSTR_MAY_ALIAS
#	endif
#	if JSTR_HAS_ATTRIBUTE(__unused__)
#		define JSTR_MAYBE_UNUSED __attribute__((__unused__))
#	elif JSTR_HAS_ATTRIBUTE(unused)
#		define JSTR_MAYBE_UNUSED __attribute__((unused))
#	else
#		define JSTR_MAYBE_UNUSED
#	endif
#	if JSTR_HAS_BUILTIN(__builtin_constant_p)
#		define JSTR_BUILTIN_CONSTANT_P(p) __builtin_constant_p(p)
#	else
#		define JSTR_BUILTIN_CONSTANT_P(p) 0
#	endif
#else
#	if defined _MSC_VER
#		define JSTR_INLINE   __forceinline inline
#		define JSTR_NOINLINE __declspec(noinline)
#		define JSTR_PURE     __declspec(noalias)
#		define JSTR_CONST    __declspec(restrict)
#		define JSTR_NOTHROW  __declspec(nothrow)
#		define JSTR_NOINLINE __delspec(noinline)
#	else
#		define JSTR_INLINE inline
#		define JSTR_NOINLINE
#		define JSTR_PURE
#		define JSTR_CONST
#		define JSTR_NOTHROW
#		define JSTR_NOINLINE
#	endif
#	define JSTR_FLATTEN
#	define JSTR_COLD
#	define JSTR_SENTINEL
#	define JSTR_NONNULL_ALL
#	define JSTR_NONNULL(args)
#	define JSTR_MALLOC
#	define JSTR_MALLOC_DEALLOC(deallocator)
#	define JSTR_MALLOC_DEALLOC_PTR(deallocator, ptr_idx)
#	define JSTR_RETURNS_NONNULL
#	define JSTR_WARN_UNUSED
#	define JSTR_DEPRECATED(msg, replacement)
#	define JSTR_FORMAT(archetype, string_index, first_to_check)
#	define JSTR_COLD
#	define JSTR_SENTINEL
#	define JSTR_NONNULL_ALL
#	define JSTR_NONNULL(args)
#	define JSTR_RETURNS_NONNULL
#	define JSTR_WARN_UNUSED
#	define JSTR_DEPRECATED(msg, replacement)
#	define JSTR_MAY_ALIAS
#	define JSTR_MAYBE_UNUSED
#	define JSTR_BUILTIN_CONSTANT_P(p) 0
#endif /* Gnuc || clang || msvc */

#define JSTR_FUNC_VOID_MAY_NULL JSTR_NOTHROW JSTR_MAYBE_UNUSED
#define JSTR_FUNC_VOID          JSTR_FUNC_VOID_MAY_NULL JSTR_NONNULL_ALL
#define JSTR_FUNC_MAY_NULL      JSTR_FUNC_VOID_MAY_NULL JSTR_WARN_UNUSED
#define JSTR_FUNC               JSTR_FUNC_MAY_NULL JSTR_NONNULL_ALL
#define JSTR_FUNC_CONST         JSTR_FUNC JSTR_CONST
#define JSTR_FUNC_PURE          JSTR_FUNC JSTR_PURE
#define JSTR_FUNC_PURE_MAY_NULL JSTR_FUNC_MAY_NULL JSTR_PURE
#define JSTR_FUNC_RET_NONNULL   JSTR_FUNC JSTR_RETURNS_NONNULL

#define JSTR_ALPHA_VOWEL_LOWER_STR     "aiueo"
#define JSTR_ALPHA_VOWEL_UPPER_STR     "AIUEO"
#define JSTR_ALPHA_CONSONANT_LOWER_STR "bcdfghjklmnpqrstvwxyz"
#define JSTR_ALPHA_CONSONANT_UPPER_STR "BCDFGHJKLMNPQRSTVWXYZ"
#define JSTR_DIGIT_STR                 "0123456789"
#define JSTR_SPACE_STR                 " \t\n\r\f\v"
#define JSTR_PUNCT_STR                 "!\"#$%&\()*+,-./:;<=>?@[\\]^_`{|}~";
#define JSTR_XDIGIT_LOWER_STR          JSTR_DIGIT_STR "abcdef"
#define JSTR_XDIGIT_UPPER_STR          JSTR_DIGIT_STR "ABCDEF"
#define JSTR_XDIGIT_STR                JSTR_DIGIT_STR "abcdefABCDEF"
#define JSTR_ALPHA_VOWEL_STR           JSTR_ALPHA_VOWEL_LOWER_STR JSTR_ALPHA_VOWEL_UPPER_STR
#define JSTR_ALPHA_CONSONANT_STR       JSTR_ALPHA_CONSONANT_LOWER_STR JSTR_ALPHA_CONSONANT_UPPER_STR
#define JSTR_ALPHA_LOWER_STR           JSTR_ALPHA_VOWEL_LOWER_STR JSTR_ALPHA_CONSONANT_LOWER_STR
#define JSTR_ALPHA_UPPER_STR           JSTR_ALPHA_VOWEL_UPPER_STR JSTR_ALPHA_CONSONANT_UPPER_STR
#define JSTR_ALPHA_STR                 JSTR_ALPHA_LOWER_STR JSTR_ALPHA_UPPER_STR
#define JSTR_ALNUM_LOWER_STR           JSTR_ALPHA_LOWER_STR JSTR_DIGIT_STR
#define JSTR_ALNUM_UPPER_STR           JSTR_ALPHA_UPPER_STR JSTR_DIGIT_STR
#define JSTR_ALNUM_STR                 JSTR_ALPHA_STR JSTR_DIGIT_STR

#define JSTR_ALPHA_VOWEL_LOWER_CASE \
case 'a':                           \
case 'i':                           \
case 'u':                           \
case 'e':                           \
case 'o':

#define JSTR_ALPHA_VOWEL_UPPER_CASE \
case 'A':                           \
case 'I':                           \
case 'U':                           \
case 'E':                           \
case 'O':

#define JSTR_ALPHA_CONSONANT_LOWER_CASE \
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

#define JSTR_ALPHA_CONSONANT_UPPER_CASE \
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

#define JSTR_DIGIT_CASE \
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

#define JSTR_ALPHA_LOWER_CASE \
	JSTR_VOWEL_LOWER_CASE JSTR_CONSONANT_LOWER_CASE

#define JSTR_ALPHA_UPPER_CASE \
	JSTR_VOWEL_UPPER_CASE JSTR_CONSONANT_UPPER_CASE

#define JSTR_XDIGIT_CASE \
	JSTR_DIGIT_CASE  \
case 'a':                \
case 'b':                \
case 'c':                \
case 'd':                \
case 'e':                \
case 'f':                \
case 'A':                \
case 'B':                \
case 'C':                \
case 'D':                \
case 'E':                \
case 'F':

#define JSTR_BLANK_CASE \
case '\t':              \
case ' ':

#define JSTR_SPACE_CASE \
	JSTR_BLANK_CASE \
case '\n':              \
case '\v':              \
case '\f':              \
case '\r':

#define JSTR_GRAPH_CASE \
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

#define JSTR_PRINT_CASE \
	JSTR_GRAPH_CASE \
case 32:

#define JSTR_CNTRL_CASE \
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

#define JSTR_PUNCT_CASE \
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
                        \
case ':':               \
case ';':               \
case '<':               \
case '=':               \
case '>':               \
case '?':               \
case '@':               \
                        \
case '[':               \
case '\\':              \
case ']':               \
case '^':               \
case '_':               \
case '`':               \
                        \
case '{':               \
case '|':               \
case '}':               \
case '~':

#define JSTR_VOWEL_CASE             \
	JSTR_VOWEL_ALPHA_UPPER_CASE \
	JSTR_VOWEL_ALPHA_LOWER_CASE
#define JSTR_CONSONANT_CASE             \
	JSTR_CONSONANT_ALPHA_UPPER_CASE \
	JSTR_CONSONANT_ALPHA_LOWER_CASE
#define JSTR_ALPHA_CASE       \
	JSTR_ALPHA_LOWER_CASE \
	JSTR_ALPHA_UPPER_CASE
#define JSTR_ALNUM_CASE \
	JSTR_DIGIT_CASE \
	JSTR_ALPHA_CASE

/* This is to enable the ISO C11 extension.  */
#if (defined _ISOC11_SOURCE || defined _ISOC2X_SOURCE \
     || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L))
#	define JSTR_USE_ISOC11 1
#endif

/* This is to enable the ISO C99 extension.  */
#if (defined _ISOC99_SOURCE || defined _ISOC11_SOURCE \
     || defined _ISOC2X_SOURCE                        \
     || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L))
#	define JSTR_USE_ISOC99 1
#endif

/* This is to enable the ISO C90 Amendment 1:1995 extension.  */
#if (defined _ISOC99_SOURCE || defined _ISOC11_SOURCE \
     || defined _ISOC2X_SOURCE                        \
     || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199409L))
#	define JSTR_USE_ISOC95 1
#endif

#ifdef __cplusplus
/* This is to enable compatibility for ISO C++17.  */
#	if __cplusplus >= 201703L
#		define JSTR_USE_ISOC11 1
#	endif
/* This is to enable compatibility for ISO C++11.
   Check the temporary macro for now, too.  */
#	if __cplusplus >= 201103L || defined __GXX_EXPERIMENTAL_CXX0X__
#		define JSTR_USE_ISOCXX11 1
#		define JSTR_USE_ISOC99   1
#	endif
#endif

/* If none of the ANSI/POSIX macros are defined, or if _DEFAULT_SOURCE
   is defined, use POSIX.1-2008 (or another version depending on
   _XOPEN_SOURCE).  */
#ifdef _DEFAULT_SOURCE
#	if !defined _POSIX_SOURCE && !defined _POSIX_C_SOURCE
#		define JSTR_USE_POSIX_IMPLICITLY 1
#	endif
#endif

#if (defined _POSIX_SOURCE                                \
     || (defined _POSIX_C_SOURCE && _POSIX_C_SOURCE >= 1) \
     || defined _XOPEN_SOURCE)
#	define JSTR_USE_POSIX 1
#endif

#if defined _POSIX_C_SOURCE && _POSIX_C_SOURCE >= 2 || defined _XOPEN_SOURCE
#	define JSTR_USE_POSIX2 1
#endif

#if defined _POSIX_C_SOURCE && (_POSIX_C_SOURCE - 0) >= 199309L
#	define JSTR_USE_POSIX199309 1
#endif

#if defined _POSIX_C_SOURCE && (_POSIX_C_SOURCE - 0) >= 199506L
#	define JSTR_USE_POSIX199506 1
#endif

#if defined _POSIX_C_SOURCE && (_POSIX_C_SOURCE - 0) >= 200112L
#	define JSTR_USE_XOPEN2K 1
#	undef JSTR_USE_ISOC95
#	define JSTR_USE_ISOC95 1
#	undef JSTR_USE_ISOC99
#	define JSTR_USE_ISOC99 1
#endif

#ifdef _XOPEN_SOURCE
#	define JSTR_USE_XOPEN 1
#	if (_XOPEN_SOURCE - 0) >= 500
#		define JSTR_USE_XOPEN_EXTENDED 1
#		define JSTR_USE_UNIX98         1
#		if (_XOPEN_SOURCE - 0) >= 600
#			if (_XOPEN_SOURCE - 0) >= 700
#				define JSTR_USE_XOPEN2K8    1
#				define JSTR_USE_XOPEN2K8XSI 1
#			endif
#			define JSTR_USE_XOPEN2K    1
#			define JSTR_USE_XOPEN2KXSI 1
#			undef JSTR_USE_ISOC95
#			define JSTR_USE_ISOC95 1
#			undef JSTR_USE_ISOC99
#			define JSTR_USE_ISOC99 1
#		endif
#	else
#		ifdef _XOPEN_SOURCE_EXTENDED
#			define JSTR_USE_XOPEN_EXTENDED 1
#		endif
#	endif
#endif

#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS == 64
#	define JSTR_USE_FILE_OFFSET64 1
#endif

#if defined _DEFAULT_SOURCE
#	define JSTR_USE_MISC 1
#endif

#ifdef _ATFILE_SOURCE
#	define JSTR_USE_ATFILE 1
#endif

#ifdef _DYNAMIC_STACK_SIZE_SOURCE
#	define JSTR_USE_DYNAMIC_STACK_SIZE 1
#endif

#ifdef _GNU_SOURCE
#	define JSTR_USE_GNU 1
#endif

#if ((defined __GLIBC__ && __GLIBC__ < 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ <= 19)) && defined _BSD_SOURCE) \
|| defined _DEFAULT_SOURCE
#	define JSTR_HAVE_STRCASECMP  1
#	define JSTR_HAVE_STRNCASECMP 1
#endif

#ifdef _XOPEN_SOURCE
#	define JSTR_HAVE_MEMCCPY 1
#endif /* Misc || Xopen */

#ifdef _GNU_SOURCE
#	define JSTR_HAVE_MEMMEM            1
#	define JSTR_HAVE_MEMRCHR           1
#	define JSTR_HAVE_STRCHRNUL         1
#	define JSTR_HAVE_FGETS_UNLOCKED    1
#	define JSTR_HAVE_FPUTS_UNLOCKED    1
#	define JSTR_HAVE_GETWC_UNLOCKED    1
#	define JSTR_HAVE_GETWCHAR_UNLOCKED 1
#	define JSTR_HAVE_FGETWC_UNLOCKED   1
#	define JSTR_HAVE_FPUTWC_UNLOCKED   1
#	define JSTR_HAVE_PUTWCHAR_UNLOCKED 1
#	define JSTR_HAVE_FGETWS_UNLOCKED   1
#	define JSTR_HAVE_FPUTWS_UNLOCKED   1
#	define JSTR_HAVE_WMEMPCPY          1
#	define JSTR_HAVE_MEMPCPY           1
#	define JSTR_HAVE_STRCASESTR        1
#endif /* Gnu */

#if ((JSTR_GLIBC_PREREQ(2, 24)) && _POSIX_C_SOURCE >= 199309L)                                                     \
|| ((defined __GLIBC__ && __GLIBC__ == 2 && __GLIBC_MINOR__ <= 19) && defined _SVID_SOURCE || defined _BSD_SOURCE) \
|| (defined __GLIBC__ && __GLIBC__ == 2 && __GLIBC_MINOR__ <= 23 && defined _POSIX_C_SOURCE)
#	define JSTR_HAVE_GETC_UNLOCKED    1
#	define JSTR_HAVE_GETCHAR_UNLOCKED 1
#	define JSTR_HAVE_PUTC_UNLOCKED    1
#	define JSTR_HAVE_PUTCHAR_UNLOCKED 1
#endif /* Posix || Bsd  */

#if (JSTR_GLIBC_PREREQ(2, 19) && defined _DEFAULT_SOURCE) \
|| defined _SVID_SOURCE || defined _BSD_SOURCE
#	define JSTR_HAVE_FREAD_UNLOCKED    1
#	define JSTR_HAVE_FWRITE_UNLOCKED   1
#	define JSTR_HAVE_FPUTC_UNLOCKED    1
#	define JSTR_HAVE_FGETC_UNLOCKED    1
#	define JSTR_HAVE_CLEARERR_UNLOCKED 1
#	define JSTR_HAVE_FEOF_UNLOCKED     1
#	define JSTR_HAVE_FERROR_UNLOCKED   1
#	define JSTR_HAVE_FILENO_UNLOCKED   1
#	define JSTR_HAVE_FFLUSH_UNLOCKED   1
#endif /* Default || Svid || Bsd */

#if JSTR_GLIBC_PREREQ(2, 10) && _POSIX_C_SOURCE >= 200809L \
|| defined _GNU_SOURCE
#	define JSTR_HAVE_STPCPY  1
#	define JSTR_HAVE_STRNLEN 1
#	define JSTR_HAVE_STRNDUP 1
#endif /* Posix || Gnu */

#if (defined _XOPEN_SOURCE && _XOPEN_SOURCE >= 500)                                                                \
|| (defined __GLIBC__ && __GLIBC__ == 2 && __GLIBC_MINOR__ <= 19 && (defined _BSD_SOURCE || defined _SVID_SOURCE)) \
|| (JSTR_GLIBC_PREREQ(2, 12) && __POSIX_C_SOURCE >= 200809L)
#	define JSTR_HAVE_STRDUP 1
#endif /* Xopen || Bsd || Svid || Posix */

#ifdef _GNU_SOURCE
#	define JSTR_HAVE_STRDUPA  1
#	define JSTR_HAVE_STRNDUPA 1
#endif /* Gnu */

#if (defined __GLIBC__ && (__GLIBC__ < 2 || __GLIBC__ == 2 && __GLIBC_MINOR__ <= 19) && defined _BSD_SOURCE || defined _SVID_SOURCE) \
|| (defined _POSIX_C_SOURCE && _POSIX_C_SOURCE >= 2)
#	define JSTR_HAVE_POPEN  1
#	define JSTR_HAVE_PCLOSE 1
#endif

#if (JSTR_GLIBC_PREREQ(2, 20) && defined _DEFAULT_SOURCE)                                                        \
|| (JSTR_GLIBC_PREREQ(2, 19) && defined _SVID_SOURCE)                                                            \
|| (JSTR_GLIBC_PREREQ(2, 17) && (_XOPEN_SOURCE >= 500 && !(_POSIX_C_SOURCE >= 200809L)))                         \
|| (JSTR_GLIBC_PREREQ(2, 12) && (_XOPEN_SOURCE >= 500 && !(_POSIX_C_SOURCE >= 200112L)) || defined _SVID_SOURCE) \
|| (defined _SVID_SOURCE || _XOPEN_SOURCE >= 500)
#	define JSTR_HAVE_GCVT 1
#	define JSTR_HAVE_ECVT 1
#	define JSTR_HAVE_FCVT 1
#endif

#if (JSTR_GLIBC_PREREQ(2, 19) && defined _DEFAULT_SOURCE) \
|| (defined __GLIBC__ && defined _SVID_SOURCE)
#	define JSTR_HAVE_QECVT 1
#	define JSTR_HAVE_QFCVT 1
#	define JSTR_HAVE_QGCVT 1
#endif

#if (JSTR_GLIBC_PREREQ(2, 19) && defined _DEFAULT_SOURCE) \
|| (defined __GLIBC__ && (_SVID_SOURCE || defined _BSD_SOURCE))
#	define JSTR_HAVE_QECVT_R 1
#	define JSTR_HAVE_QFCVT_R 1
#	define JSTR_HAVE_ECVT_R  1
#	define JSTR_HAVE_FCVT_R  1
#endif

#if defined _DEFAULT_SOURCE || !defined __USE_XOPEN2K8
#	define JSTR_HAVE_BCMP  1
#	define JSTR_HAVE_BCOPY 1
#	define JSTR_HAVE_BZERO 1
#endif

#ifdef _ATFILE_SOURCE
#	define JSTR_HAVE_ATFILE 1
#endif

#if JSTR_USE_XOPEN_EXTENDED || defined JSTR_USE_XOPEN2K8
#	define JSTR_HAVE_FCHDIR 1
#endif

#if JSTR_USE_XOPEN2K8
#	define JSTR_HAVE_FDOPENDIR 1
#endif

#if (defined __x86_64 || defined __x86_64__) && (defined _ILP32 || defined __ILP32__)
#	define JSTR_ARCH_X86_32 1
#elif defined __amd64__ || defined __amd64 \
|| defined _M_AMD64 || defined __x86_64__  \
|| defined __x86_64 || defined _M_X64      \
|| defined _M_X64
#	define JSTR_ARCH_X86_64 1
#elif defined __aarch64__ || defined _M_ARM64
#	define JSTR_ARCH_ARM64 1
#elif defined __arm__ || defined __arm \
|| defined __thumb__ || defined _ARM   \
|| defined _M_ARM || defined _M_ARM_T
#	define JSTR_ARCH_ARM 1
#	if defined __ARM_ARCH_2__
#		define JSTR_ARCH_ARM2 1
#	elif defined __ARM_ARCH_3__ || defined __ARM_ARCH_3M__
#		define JSTR_ARCH_ARM3 1
#	elif defined __ARM_ARCH_4T__ || defined __TARGET_ARM_4T
#		define JSTR_ARCH_ARM4T 1
#	elif defined __ARM_ARCH_5__ || defined __ARM_ARCH_5E__ \
	|| defined __ARM_ARCH_5T__ || defined __ARM_ARCH_5TE__  \
	|| defined __ARM_ARCH_5TEJ__
#		define JSTR_ARCH_ARM5 1
#	elif defined __ARM_ARCH_6T2__
#		define JSTR_ARCH_ARM6T2 1
#	elif defined __ARM_ARCH_6__ || defined __ARM_ARCH_6J__ \
	|| defined __ARM_ARCH_6Z__ || defined __ARM_ARCH_6ZK__  \
	|| defined __ARM_ARCH_6K__
#		define JSTR_ARCH_ARM6 1
#	elif defined __ARM_ARCH_7__ || defined __ARM_ARCH_7A__ \
	|| defined __ARM_ARCH_7R__ || defined __ARM_ARCH_7M__   \
	|| defined __ARM_ARCH_7EM__ || defined __ARM_ARCH_7S__
#		define JSTR_ARCH_ARM7 1
#	endif
#elif defined mips || defined __mips__ \
|| defined __mips || defined __MIPS__
#	define JSTR_ARCH_MIPS 1
#elif defined __sh__
#	define JSTR_ARCH_SH 1
#	if defined __sh1__
#		define JSTR_ARCH_SH1 1
#	elif defined __sh2__
#		define JSTR_ARCH_SH2 1
#	elif defined __sh3__
#		define JSTR_ARCH_SH3 1
#	elif defined __sh4__
#		define JSTR_ARCH_SH4 1
#	elif defined __sh5__
#		define JSTR_ARCH_SH5 1
#	endif
#elif defined __powerpc || defined __powerpc__ \
|| defined __POWERPC__ || defined __ppc__      \
|| defined __PPC__ || defined _ARCH_PPC
#	define JSTR_ARCH_POWERPC 1
#elif defined __PPC64__ || defined __ppc64__ \
|| defined _ARCH_PPC64 || defined __powerpc64__
#	define JSTR_ARCH_POWERPC64 1
#elif defined _ARCH_PWR4
#	define JSTR_ARCH_POWERPC4 1
#elif defined _ARCH_PWR5X
#	define JSTR_ARCH_POWERPC5X 1
#elif defined _ARCH_PWR6
#	define JSTR_ARCH_POWERPC6 1
#elif defined _ARCH_PWR7
#	define JSTR_ARCH_POWERPC7 1
#elif defined _ARCH_PWR8
#	define JSTR_ARCH_POWERPC8 1
#elif defined _ARCH_PWR9
#	define JSTR_ARCH_POWERPC9 1
#elif defined __sparc__ || defined __sparc
#	define JSTR_ARCH_SPARC 1
#	if defined __sparc_v8__ || defined __sparcv8
#		define JSTR_ARCH_SPARCV8 1
#	elif defined __sparc_v9__ || defined __sparcv9
#		define JSTR_ARCH_SPARCV9 1
#	endif
#elif defined __m68k__ || defined M68000 || defined __MC68K__
#	define JSTR_ARCH_M68K 1
#elif defined __alpha__ || defined __alpha || defined _M_ALPHA
#	define JSTR_ARCH_ALPHA 1
#	if defined __alpha_ev4__
#		define JSTR_ARCH_ALPHA 1
#	elif defined __alpha_ev5__
#	elif defined __alpha_ev6__
#	endif
#elif defined __hppa__ || defined __hppa || defined __HPPA__
#	define JSTR_ARCH_HPPA 1
#	if defined _PA_RISC1_0
#		define JSTR_ARCH_HPPA_RISC1_0 1
#	elif defined _PA_RISC1_1 || defined __HPPA11__ || defined __PA7100__
#		define JSTR_ARCH_HPPA_RISC1_1 1
#	elif defined _PA_RISC2_0 || defined __RISC2_0__ \
	|| defined __HPPA20__ || defined __PA8000__
#		define JSTR_ARCH_HPPA_RISC2_0 1
#	endif
#elif defined __riscv_zbb || defined __riscv_xtheadbb
#	define JSTR_ARCH_RISCV 1
#elif defined __s390x__ || defined __s390__
#	define JSTR_ARCH_S390 1
#elif defined __ia64__ || defined _IA64 \
|| defined __IA64__ || defined __ia64   \
|| defined _M_IA64 || defined __itanium__
#	define JSTR_ARCH_IA64 1
#elif defined i386 || defined __i386__ \
|| defined __i386 || defined _M_IX86
#	define JSTR_ARCH_I386 1
#else
#	define JSTR_ARCH_GENERIC 1
#endif

#ifdef __BYTE_ORDER
#	if __BYTE_ORDER == __BIG_ENDIAN \
	|| defined __BIG_ENDIAN__        \
	|| defined __ARMEB__             \
	|| defined __THUMBEB__           \
	|| defined __AARCH64EB__         \
	|| defined _MIBSEB               \
	|| defined __MIBSEB              \
	|| defined __MIBSEB__
#		undef JSTR_ENDIAN_BIG
#		undef JSTR_ENDIAN_LITTLE
#		define JSTR_ENDIAN_BIG    1
#		define JSTR_ENDIAN_LITTLE 0
#	elif __BYTE_ORDER == __LITTLE_ENDIAN \
	|| defined __LITTLE_ENDIAN__          \
	|| defined __ARMEL__                  \
	|| defined __THUMBEL__                \
	|| defined __AARCH64EL__              \
	|| defined _MIPSEL                    \
	|| defined __MIPSEL                   \
	|| defined __MIPSEL__
#		undef JSTR_ENDIAN_LITTLE
#		undef JSTR_ENDIAN_BIG
#		define JSTR_ENDIAN_LITTLE 1
#		define JSTR_ENDIAN_BIG    0
#	endif
#endif

#ifdef JSTR_USE_UNLOCKED_IO
#	if JSTR_HAVE_FGETS_UNLOCKED
#		define fgets(s, n, stream) fgets_unlocked(s, n, stream)
#	endif
#	if JSTR_HAVE_FPUTS_UNLOCKED
#		define fputs(s, stream) fputs_unlocked(s, stream)
#	endif
#	if JSTR_HAVE_GETWC_UNLOCKED
#		define getwc(stream) getwc_unlocked(stream)
#	endif
#	if JSTR_HAVE_GETWCHAR_UNLOCKED
#		define getwchar() getwchar_unlocked()
#	endif
#	if JSTR_HAVE_FGETWC_UNLOCKED
#		define fgetwc(stream) fgetwc_unlocked(stream)
#	endif
#	if JSTR_HAVE_FPUTWC_UNLOCKED
#		define fputwc(wc, stream) fputwc_unlocked(wc, stream)
#	endif
#	if JSTR_HAVE_PUTWCHAR_UNLOCKED
#		define putwchar(wc) putwchar_unlocked(wc)
#	endif
#	if JSTR_HAVE_FGETWS_UNLOCKED
#		define fgetws(ws, n, stream) fgetws_unlocked(ws, n, stream)
#	endif
#	if JSTR_HAVE_FPUTWS_UNLOCKED
#		define fputws(ws, stream) fputws_unlocked(ws, stream)
#	endif
#	if JSTR_HAVE_GETC_UNLOCKED
#		define getc(stream) getc_unlocked(stream)
#	endif
#	if JSTR_HAVE_GETCHAR_UNLOCKED
#		define getchar() getchar_unlocked()
#	endif
#	if JSTR_HAVE_PUTC_UNLOCKED
#		define putc(c, stream) putc_unlocked(c, stream)
#	endif
#	if JSTR_HAVE_PUTCHAR_UNLOCKED
#		define putchar(c) putchar_unlocked(c)
#	endif
#	if JSTR_HAVE_FREAD_UNLOCKED
#		define fread(ptr, size, n, stream) fread_unlocked(ptr, size, n, stream)
#	endif
#	if JSTR_HAVE_FWRITE_UNLOCKED
#		define fwrite(ptr, size, n, stream) fwrite_unlocked(ptr, size, n, stream)
#	endif
#	if JSTR_HAVE_FPUTC_UNLOCKED
#		define fputc(c, stream) fputc_unlocked(c, stream)
#	endif
#	if JSTR_HAVE_FGETC_UNLOCKED
#		define fgetc(stream) fgetc_unlocked(stream)
#	endif
#	if JSTR_HAVE_CLEARERR_UNLOCKED
#		define clearerr(stream) clearerr_unlocked(stream)
#	endif
#	if JSTR_HAVE_FEOF_UNLOCKED
#		define feof(stream) feof_unlocked(stream)
#	endif
#	if JSTR_HAVE_FERROR_UNLOCKED
#		define ferror(stream) ferror_unlocked(stream)
#	endif
#	if JSTR_HAVE_FILENO_UNLOCKED
#		define fileno(stream) fileno_unlocked(stream)
#	endif
#	if JSTR_HAVE_FFLUSH_UNLOCKED
#		define fflush(stream) fflush_unlocked(stream)
#	endif
#endif

#if defined __GLIBC__ && _DIRENT_HAVE_D_TYPE
#	define JSTR_HAVE_DIRENT_D_TYPE 1
#else
#	define JSTR_HAVE_DIRENT_D_TYPE 0
#endif

#if defined __GLIBC__ && _DIRENT_HAVE_D_RECLEN
#	define JSTR_HAVE_DIRENT_D_RECLEN 1
#else
#	define JSTR_HAVE_DIRENT_D_RECLEN 0
#endif

#if defined __GLIBC__ && _DIRENT_HAVE_D_NAMLEN
#	define JSTR_HAVE_DIRENT_D_NAMLEN 1
#else
#	define JSTR_HAVE_DIRENT_D_NAMLEN 0
#endif

#if defined __GLIBC__ && JSTR_HAVE_MEMMEM && JSTR_ARCH_S390
#	define JSTR_HAVE_MEMMEM_OPTIMIZED 1
#endif

#if defined __GLIBC__ && (JSTR_ARCH_X86_64 || JSTR_ARCH_POWERPC7 || JSTR_ARCH_POWERPC64 || JSTR_ARCH_S390)
#	define JSTR_HAVE_STRSTR_OPTIMIZED 1
#endif

/* Only use libc strcasestr when it is implemented in assembly. */
#if defined __GLIBC__ && JSTR_HAVE_STRCASESTR && (JSTR_ARCH_POWERPC64 || JSTR_ARHC_POWERPC8)
#	define JSTR_HAVE_STRCASESTR_OPTIMIZED 1
#endif

#if defined __GLIBC__ && (JSTR_ARCH_X86_64 || JSTR_ARCH_S390 || JSTR_ARCH_I386 || JSTR_ARCH_SPARC || JSTR_ARCH_POWERPC64 || JSTR_ARCH_POWERPC8)
#	define JSTR_HAVE_STRCSPN_OPTIMIZED 1
#endif

#if defined __GLIBC__ && (JSTR_ARCH_X86_64 || JSTR_ARCH_S390 || JSTR_ARCH_I386 || JSTR_ARCH_SPARC)
#	define JSTR_HAVE_STRPBRK_OPTIMIZED 1
#endif

enum {
	/* Needle length over which memmem would be faster than strstr. */
	JSTR_MEMMEM_THRES = 18,
	/* Number of iterations over which an optimized strcspn would be faster than a byte loop. */
	JSTR_STRCASECHR_THRES = 24
};

/* Only use memmem for long needles or when it is implemented in assembly.
   It seems to be slower than strstr for short needles. */
#if JSTR_HAVE_MEMMEM
#	if JSTR_HAVE_MEMMEM_OPTIMIZED || !JSTR_HAVE_STRSTR_OPTIMIZED
#		define JSTR_MEMMEM(hs, hslen, ne, nelen) memmem(hs, hslen, ne, nelen)
#	else
#		define JSTR_MEMMEM(hs, hslen, ne, nelen) ((nelen < JSTR_MEMMEM_THRES) ? strstr(hs, ne) : memmem(hs, hslen, ne, nelen))
#	endif
#else
#	define JSTR_MEMMEM(hs, hslen, ne, nelen) strstr(hs, ne)
#endif

/* Check builtins. */
#if JSTR_ARCH_ALPHA
#	if (JSTR_HAS_BUILTIN(__builtin_alpha_cmpbge) || defined __builtin_alpha_cmpbge) \
	&& (JSTR_HAS_BUILTIN(__builtin_ctzl) || defined __builtin_ctzl)                  \
	&& (JSTR_HAS_BUILTIN(__builtin_clzl) || defined __builtin_clzl)
#		define JSTR_HAVE_WORD_AT_A_TIME 1
#	endif
#elif JSTR_ARCH_POWERPC6
#	if (JSTR_HAS_BUILTIN(__builtin_cmpb) || defined __builtin_cmpb)
#		define JSTR_HAVE_WORD_AT_A_TIME 1
#	endif
#else /* JSTR_ARCH_GENERIC */
#	if (JSTR_HAS_BUILTIN(__builtin_clzl) || defined __builtin_clzl)  \
	&& (JSTR_HAS_BUILTIN(__builtin_clzll) || defined __builtin_clzll) \
	&& (JSTR_HAS_BUILTIN(__builtin_ctzl) || defined __builtin_ctzl)   \
	&& (JSTR_HAS_BUILTIN(__builtin_ctzll) || defined __builtin_ctzll)
#		define JSTR_HAVE_WORD_AT_A_TIME 1
#	endif
#endif

#endif /* JSTR_MACROS_H */
