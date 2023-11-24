/* See LICENSE file for copyright and license details. */

#ifndef JSTR_MACROS_H
#define JSTR_MACROS_H 1

#include "_jstr-features.h"
#include "jstr-config.h"
#include "jstr-ptr-arith.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#if JSTR_DEBUG
#	undef JSTR_PANIC
#	define JSTR_PANIC 1
#endif

#define JSTR_PAGE_SIZE            4096
#define JSTR_ARRAY_SIZE(array)    (sizeof(array) / sizeof(array[0]))
#define PJSTR_CONCAT_HELPER(x, y) x##y
#define JSTR_CONCAT(x, y)         PJSTR_CONCAT_HELPER(x, y)
#define JSTR_STRING(x)            #x
#define jstr_chk(ret)             jstr_unlikely(ret == JSTR_RET_ERR)

#define JSTR_MEMSET_ARRAY(array, c) ((JSTR_ARRAY_SIZE(array) == 256)             \
	                             ? (memset((array), (c), 64),                \
	                                memset((array) + 64, (c), 64),           \
	                                memset((array) + 64 + 64, (c), 64),      \
	                                memset((array) + 64 + 64 + 64, (c), 64)) \
	                             : memset((array), (c), sizeof((array))))
#define JSTR_BZERO_ARRAY(array) JSTR_MEMSET_ARRAY(array, 0)

#ifdef __cplusplus
#	define PJSTR_BEGIN_DECLS extern "C" {
#	define PJSTR_END_DECLS   }
#else
#	define PJSTR_BEGIN_DECLS
#	define PJSTR_END_DECLS
#endif

#ifdef __GLIBC__
PJSTR_BEGIN_DECLS
#	include <sys/cdefs.h>
PJSTR_END_DECLS
#endif

PJSTR_BEGIN_DECLS
typedef enum {
	JSTR_RET_ERR = -1,
	JSTR_RET_SUCC = 0
} jstr_ret_ty;
PJSTR_END_DECLS

#if JSTR_DEBUG
#	define JSTR_ASSERT_DEBUG(expr, msg)        \
		do {                                \
			if (jstr_unlikely(!(expr))) \
				jstr_errdie(msg);   \
		} while (0)
#else
/* clang-format off */
#	define JSTR_ASSERT_DEBUG(expr, msg) do {} while (0)
/* clang-format on */
#endif

#if JSTR_PANIC
#	define PJSTR_EXIT_MAYBE()  jstr_errdie("")
#	define PJSTR_ERR_MAYBE()   jstr_err("")
#	define JSTR_PRINT_LOG(...) fprintf(stderr, __VA_ARGS__)
#	define PJSTR_MALLOC_ERR(p, do_on_malloc_err)     \
		do {                                      \
			if (jstr_unlikely((p) == NULL)) { \
				jstr_errdie("");          \
				do_on_malloc_err;         \
			}                                 \
		} while (0)
#else
/* clang-format off */
#	define PJSTR_EXIT_MAYBE() do {} while (0)
#	define PJSTR_ERR_MAYBE() do {} while (0)
#	define JSTR_PRINT_LOG(...) do {} while (0)
/* clang-format on */
#	define PJSTR_MALLOC_ERR(p, do_on_malloc_err)     \
		do {                                      \
			if (jstr_unlikely((p) == NULL)) { \
				do_on_malloc_err;         \
			}                                 \
		} while (0)
#endif

#define JSTR_RETURN_ERR(errcode)    \
	do {                        \
		PJSTR_EXIT_MAYBE(); \
		return errcode;     \
	} while (0)

#ifdef __cplusplus
template <typename T, typename Other>
static T
PJSTR_CAST(T, Other other)
{
	return (T)other;
}
#else
#	define PJSTR_CAST(T, other) (other)
#endif /* cast */

#if !(defined __STDC_VERSION__ && __STDC_VERSION__ > 201000L && __STDC_NO_VLA__)
#	define JSTR_HAVE_VLA 1
#endif

#if defined _GNU_SOURCE || defined alloca
#	define JSTR_HAVE_ALLOCA 1
#endif

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
#endif /* have_generic */

#if (defined __GNUC__ && (__GNUC__ >= 4)) \
|| (defined __clang__ && (__clang_major__ >= 3))
#	define JSTR_HAVE_TYPEOF 1
#endif /* have_typeof */

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
#endif /* have_typeof && have_generic */

#ifdef static_assert
PJSTR_BEGIN_DECLS
#	include <assert.h>
PJSTR_END_DECLS
#	define JSTR_HAVE_STATIC_ASSERT       1
#	define JSTR_STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#elif defined _Static_assert || defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L
PJSTR_BEGIN_DECLS
#	include <assert.h>
PJSTR_END_DECLS
#	define JSTR_HAVE_STATIC_ASSERT       1
#	define JSTR_STATIC_ASSERT(expr, msg) _Static_assert(expr, msg)
#else
/* clang-format off */
#	define JSTR_STATIC_ASSERT(expr, msg) do {} while (0)
/* clang-format on */
#endif /* static_assert */

#if JSTR_HAVE_GENERIC
#	define JSTR_GENERIC_CASE_SIZE(bool_)               int : bool_, unsigned int : bool_, size_t : bool_, long : bool_, long long : bool_, unsigned long long : bool_
#	define JSTR_GENERIC_CASE_STR(bool_)                char * : bool_, const char * : bool_
#	define JSTR_GENERIC_CASE_STR_STACK(bool_, s)       char(*)[sizeof(s)] : bool_, const char(*)[sizeof(s)] : bool_
#	define JSTR_GENERIC_CASE_CHAR(bool_)               char : bool_, const char : bool_
#	define PJSTR_IS_SIZE(expr)                         _Generic((expr), JSTR_GENERIC_CASE_SIZE(1), default: 0)
#	define PJSTR_IS_STR(expr)                          _Generic((expr), JSTR_GENERIC_CASE_STR(1), default: 0)
#	define PJSTR_IS_STR_STACK(expr)                    _Generic((expr), JSTR_GENERIC_CASE_STR_STACK(1, expr), default: 0)
#	define PJSTR_IS_CHAR(expr)                         _Generic((expr), JSTR_GENERIC_CASE_CHAR(1), default: 0)
#	define JSTR_STATIC_ASSERT_IS_SIZE(expr)            JSTR_STATIC_ASSERT(PJSTR_IS_SIZE(expr), "Passing non-size_type as number argument!");
#	define JSTR_STATIC_ASSERT_IS_STR(expr)             JSTR_STATIC_ASSERT(PJSTR_IS_STR(expr), "Passing non-string as string argument!");
#	define JSTR_STATIC_ASSERT_IS_CHAR(expr)            JSTR_STATIC_ASSERT(PJSTR_IS_CHAR(expr), "Passing non-char as char argument!");
#	define JSTR_STATIC_ASSERT_TYPECHECK(expr_ty, expr) JSTR_STATIC_ASSERT(JSTR_SAME_TYPE(expr_ty, expr), "Passing the wrong data type!");
#else
#	define JSTR_GENERIC_CASE_SIZE(bool_)
#	define JSTR_GENERIC_CASE_STR(bool_)
#	define JSTR_GENERIC_CASE_STR_STACK(bool_, s)
#	define JSTR_GENERIC_CASE_CHAR(bool_)
#	define PJSTR_IS_SIZE(expr)
#	define PJSTR_IS_STR(expr)
#	define PJSTR_IS_STR_STACK(expr)
#	define PJSTR_IS_CHAR(expr)
#	define JSTR_STATIC_ASSERT_IS_SIZE(expr)
#	define JSTR_STATIC_ASSERT_IS_STR(expr)
#	define JSTR_STATIC_ASSERT_IS_CHAR(expr)
#	define JSTR_STATIC_ASSERT_TYPECHECK(expr_ty, expr)
#endif /* have_generic */

#if defined __cplusplus && __cplusplus > 199711L
#	define JSTR_NOEXCEPT noexcept
#else
#	define JSTR_NOEXCEPT
#endif /* noexcept */

#ifdef __ASSERT_FUNCTION
#	define JSTR_STATIC_ASSERT_FUNC __ASSERT_FUNCTION
#else
#	define JSTR_STATIC_ASSERT_FUNC __func__
#endif

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#	define JSTR_RESTRICT restrict
#elif defined __GNUC__ || defined __clang__
#	define JSTR_RESTRICT __restrict
#elif defined _MSC_VER
#	define JSTR_RESTRICT __restrict
#else
#	define JSTR_RESTRICT
#endif /* restrict */

#ifdef __glibc_has_attribute
#	define JSTR_HAS_ATTRIBUTE(attr) __glibc_has_attribute(attr)
#elif defined __has_attribute
#	define JSTR_HAS_ATTRIBUTE(attr) __has_attribute(attr)
#else
#	define JSTR_HAS_ATTRIBUTE(attr) 0
#endif /* has_attribute */

#ifdef __glibc_has_builtin
#	define JSTR_HAS_BUILTIN(name) __glibc_has_builtin(name)
#elif defined __has_builtin
#	define JSTR_HAS_BUILTIN(name) __has_builtin(name)
#else
#	define JSTR_HAS_BUILTIN(name) 0
#endif /* has_builtin */

#ifdef __glibc_has_extension
#	define JSTR_HAS_EXTENSION(ext) __glibc_has_extension(ext)
#elif defined __has_extension
#	define JSTR_HAS_EXTENSION(ext) __has_extension(ext)
#else
#	define JSTR_HAS_EXTENSION(ext) 0
#endif /* has_extension */

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

#ifndef PJSTR_ATTR_INLINE
#	ifdef __inline
#		define PJSTR_ATTR_INLINE __inline
#	elif (defined __cplusplus \
	       || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L))
#		define PJSTR_ATTR_INLINE inline
#	else
#		define PJSTR_ATTR_INLINE
#	endif
#endif

#if defined _MSC_VER
#	define JSTR_ATTR_NOINLINE __declspec(noinline)
#	define JSTR_ATTR_PURE     __declspec(noalias)
#	define JSTR_ATTR_CONST    __declspec(restrict)
#	define JSTR_ATTR_NOTHROW  __declspec(nothrow)
#	define JSTR_ATTR_NOINLINE __delspec(noinline)
#else
#	if JSTR_HAS_ATTRIBUTE(__format__)
#		define JSTR_ATTR_FORMAT(archetype, string_index, first_to_check) __attribute__((__format__(archetype, string_index, first_to_check)))
#	endif
#	ifdef __always_inline
#		define JSTR_ATTR_INLINE __always_inline
#	elif JSTR_HAS_ATTRIBUTE(__always_inline__)
#		define JSTR_ATTR_INLINE __attribute__((__always_inline__)) PJSTR_ATTR_INLINE
#	endif
#	ifdef __attribute_noinline__
#		define JSTR_ATTR_NOINLINE __attribute_noinline__
#	elif JSTR_HAS_ATTRIBUTE(__noinline__)
#		define JSTR_ATTR_NOINLINE __attribute__((__noinline__))
#	endif
#	ifdef __attribute_pure__
#		define JSTR_ATTR_PURE __attribute_pure__
#	elif JSTR_HAS_ATTRIBUTE(__pure__)
#		define JSTR_ATTR_PURE __attribute__((__pure__))
#	endif
#	ifdef __attribute_const__
#		define JSTR_ATTR_CONST __attribute_const__
#	elif JSTR_HAS_ATTRIBUTE(__const__)
#		define JSTR_ATTR_CONST __attribute__((__const__))
#	endif
#	ifdef __attribute_flatten__
#		define JSTR_ATTR_FLATTEN __attribute_flatten__
#	elif JSTR_HAS_ATTRIBUTE(__flatten__)
#		define JSTR_ATTR_FLATTEN __attribute__((__flatten__))
#	endif
#	ifdef __COLD
#		define JSTR_ATTR_COLD __COLD
#	elif JSTR_HAS_ATTRIBUTE(__cold__)
#		define JSTR_ATTR_COLD __attribute__((__cold__))
#	endif
#	if JSTR_HAS_ATTRIBUTE(__sentinel__)
#		define JSTR_ATTR_SENTINEL __attribute__((__sentinel__))
#	endif
#	if defined __attribute_nonnull__ && defined __nonnull
#		define JSTR_ATTR_NONNULL(params) __nonnull(params)
#	elif JSTR_HAS_ATTRIBUTE(__nonnull__)
#		define JSTR_ATTR_NONNULL_ALL     __attribute__((__nonnull__))
#		define JSTR_ATTR_NONNULL(params) __attribute__((__nonnull__ params))
#	endif
#	if JSTR_HAS_ATTRIBUTE(__malloc__)
#		define JSTR_ATTR_MALLOC                                   __attribute__((__malloc__))
#		define JSTR_ATTR_MALLOC_DEALLOC(deallocator)              __attribute__((__malloc__, deallocator))
#		define JSTR_ATTR_MALLOC_DEALLOC_PTR(deallocator, ptr_idx) __attribute__((__malloc__, deallocator, ptr_idx))
#	endif
#	if JSTR_HAS_ATTRIBUTE(__returns_nonnull__)
#		define JSTR_ATTR_RETURNS_NONNULL __attribute__((__returns_nonnull__))
#	endif
#	ifdef __attribute_warn_unused_result__
#		define JSTR_ATTR_WARN_UNUSED __attribute_warn_unused_result__
#	elif JSTR_HAS_ATTRIBUTE(__warn_unused_result__)
#		define JSTR_ATTR_WARN_UNUSED __attribute__((__warn_unused_result__))
#	endif
#	if JSTR_HAS_ATTRIBUTE(__deprecated__)
#		define JSTR_ATTR_DEPRECATED(msg) __attribute__((__deprecated__(msg)))
#	endif
#	if JSTR_HAS_ATTRIBUTE(__nothrow__)
#		define JSTR_ATTR_NOTHROW __attribute__((__nothrow__))
#	endif
#	if JSTR_HAS_ATTRIBUTE(__may_alias__)
#		define JSTR_ATTR_MAY_ALIAS      __attribute__((__may_alias__))
#		define JSTR_HAVE_ATTR_MAY_ALIAS 1
#	endif
#	ifdef __attr_access
#		define JSTR_ATTR_ACCESS(x) __attr_access(x)
#	elif JSTR_HAS_ATTRIBUTE(__access__)
#		define JSTR_ATTR_ACCESS(x) __attribute__((__access__ x))
#	endif
#	ifdef __attribute_maybe_unused__
#		define JSTR_MAYBE_UNUSED __attribute_maybe_unused__
#	elif JSTR_HAS_ATTRIBUTE(__unused__)
#		define JSTR_MAYBE_UNUSED __attribute__((__unused__))
#	endif
#	if JSTR_HAS_BUILTIN(__builtin_constant_p)
#		define JSTR_ATTR_BUILTIN_CONSTANT_P(p) __builtin_constant_p(p)
#	endif
#endif
#ifndef JSTR_ATTR_INLINE
#	define JSTR_ATTR_INLINE PJSTR_ATTR_INLINE
#endif
#ifndef JSTR_ATTR_NOINLINE
#	define JSTR_ATTR_NOINLINE
#endif
#ifndef JSTR_ATTR_PURE
#	define JSTR_ATTR_PURE
#endif
#ifndef JSTR_ATTR_CONST
#	define JSTR_ATTR_CONST
#endif
#ifndef JSTR_ATTR_NOTHROW
#	define JSTR_ATTR_NOTHROW
#endif
#ifndef JSTR_ATTR_NOINLINE
#	define JSTR_ATTR_NOINLINE
#endif
#ifndef JSTR_ATTR_FLATTEN
#	define JSTR_ATTR_FLATTEN
#endif
#ifndef JSTR_ATTR_COLD
#	define JSTR_ATTR_COLD
#endif
#ifndef JSTR_ATTR_SENTINEL
#	define JSTR_ATTR_SENTINEL
#endif
#ifndef JSTR_ATTR_NONNULL_ALL
#	define JSTR_ATTR_NONNULL_ALL
#endif
#ifndef JSTR_NONNULL
#	define JSTR_NONNULL(args)
#endif
#ifndef JSTR_ATTR_MALLOC
#	define JSTR_ATTR_MALLOC
#endif
#ifndef JSTR_ATTR_MALLOC_DEALLOC
#	define JSTR_ATTR_MALLOC_DEALLOC(deallocator)
#endif
#ifndef JSTR_ATTR_MALLOC_DEALLOC_PTR
#	define JSTR_ATTR_MALLOC_DEALLOC_PTR(deallocator, ptr_idx)
#endif
#ifndef JSTR_ATTR_RETURNS_NONNULL
#	define JSTR_ATTR_RETURNS_NONNULL
#endif
#ifndef JSTR_ATTR_WARN_UNUSED
#	define JSTR_ATTR_WARN_UNUSED
#endif
#ifndef JSTR_ATTR_DEPRECATED
#	define JSTR_ATTR_DEPRECATED(msg, replacement)
#endif
#ifndef JSTR_ATTR_FORMAT
#	define JSTR_ATTR_FORMAT(archetype, string_index, first_to_check)
#endif
#ifndef JSTR_ATTR_COLD
#	define JSTR_ATTR_COLD
#endif
#ifndef JSTR_ATTR_SENTINEL
#	define JSTR_ATTR_SENTINEL
#endif
#ifndef JSTR_ATTR_NONNULL_ALL
#	define JSTR_ATTR_NONNULL_ALL
#endif
#ifndef JSTR_NONNULL
#	define JSTR_NONNULL(args)
#endif
#ifndef JSTR_ATTR_RETURNS_NONNULL
#	define JSTR_ATTR_RETURNS_NONNULL
#endif
#ifndef JSTR_ATTR_WARN_UNUSED
#	define JSTR_ATTR_WARN_UNUSED
#endif
#ifndef JSTR_ATTR_DEPRECATED
#	define JSTR_ATTR_DEPRECATED(msg, replacement)
#endif
#ifndef JSTR_ATTR_MAY_ALIAS
#	define JSTR_ATTR_MAY_ALIAS
#	define JSTR_HAVE_ATTR_MAY_ALIAS 0
#endif
#ifndef JSTR_MAYBE_UNUSED
#	define JSTR_MAYBE_UNUSED
#endif
#ifndef JSTR_ATTR_BUILTIN_CONSTANT_P
#	define JSTR_ATTR_BUILTIN_CONSTANT_P(p) 0
#endif
#ifndef JSTR_ATTR_ACCESS
#	define JSTR_ATTR_ACCESS(x)
#endif
#define JSTR_FUNC_VOID_MAY_NULL JSTR_ATTR_NOTHROW JSTR_MAYBE_UNUSED
#define JSTR_FUNC_VOID          JSTR_FUNC_VOID_MAY_NULL JSTR_ATTR_NONNULL_ALL
#define JSTR_FUNC_MAY_NULL      JSTR_FUNC_VOID_MAY_NULL JSTR_ATTR_WARN_UNUSED
#define JSTR_FUNC               JSTR_FUNC_MAY_NULL JSTR_ATTR_NONNULL_ALL
#define JSTR_FUNC_CONST         JSTR_FUNC JSTR_ATTR_CONST
#define JSTR_FUNC_PURE          JSTR_FUNC JSTR_ATTR_PURE
#define JSTR_FUNC_PURE_MAY_NULL JSTR_FUNC_MAY_NULL JSTR_ATTR_PURE
#define JSTR_FUNC_RET_NONNULL   JSTR_FUNC JSTR_ATTR_RETURNS_NONNULL

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

#if defined __linux__ || defined linux || defined __linux
#	define JSTR_OS_LINUX 1
#	if defined __gnu_linux__
#		define JSTR_OS_GNULINUX 1
#	elif defined __ANDROID__
#		define JSTR_OS_ANDROID 1
#	endif
#elif defined __FreeBSD__
#	define JSTR_OS_FREEBSD 1
#elif defined __NetBSD__
#	define JSTR_OS_NETBSD 1
#elif defined __OpenBSD__
#	define JSTR_OS_OPENBSD 1
#elif defined __bsdi__
#	define JSTR_OS_BSDI 1
#elif defined __DragonFly__
#	define JSTR_OS_DRAGONFLYBSD 1
#elif defined _AIX || defined __TOS_AIX__
#	define JSTR_OS_AIX 1
#elif defined UTS
#	define JSTR_OS_UTS 1
#elif defined AMIGA || defined __amigaos__
#	define JSTR_OS_AMIGA 1
#elif defined aegis
#	define JSTR_OS_APOLLOAEGIS 1
#elif defined apollo
#	define JSTR_OS_APOLLODOMAIN 1
#elif defined __BEOS__
#	define JSTR_OS_BE 1
#elif defined __bg__ || defined __THW_BLUEGEN__
#	define JSTR_OS_BLUEGENE 1
#elif defined __convex__
#	define JSTR_OS_CONVEX 1
#elif defined DGUX || defined __DGUX__ || defined __dgux__
#	define JSTR_OS_DGUX 1
#elif defined _SEQUENT_ || defined sequent
#	define JSTR_OS_DYNIXPTX 1
#elif defined __EMX__
#	define JSTR_OS_EMX 1
#elif defined __GNU__ || defined __gnu_hurd__
#	define JSTR_OS_GNUHURD 1
#elif defined __FreeBSD_kernel__ && defined __GLIBC__
#	define JSTR_OS_GNUKFREEBSD 1
#elif defined __hiuxmpp
#	define JSTR_OS_HIUXMPP 1
#elif defined _hpux || defined hpux || defined __hpux
#	define JSTR_OS_HPUX 1
#elif defined __OS400__
#	define JSTR_OS_IBMOS400 1
#elif defined sgi || defined __sgi
#	define JSTR_OS_IRIX 1
#elif defined __Lynx__
#	define JSTR_OS_LYNX 1
#elif defined macintosh || defined Macintosh || (defined __APPLE__ && defined __MACH__)
#	define JSTR_OS_MAC 1
#elif defined __minix
#	define JSTR_OS_MINIX 1
#elif defined __MORPHOS__
#	define JSTR_OS_MORPH 1
#elif defined mpeix || defined __mpexl
#	define JSTR_OS_MPEIX 1
#elif defined MSDOS || defined __MSDOS__ || defined _MSDOS || defined __DOS__
#	define JSTR_OS_MSDOS 1
#elif defined __TANDEM
#	define JSTR_OS_NONSTOP 1
#elif defined OS2 || defined _OS2 || defined __OS2__ || defined __TOS_OS2__
#	define JSTR_OS_OS2 1
#elif defined __palmos__
#	define JSTR_OS_PALM 1
#elif defined EPLAN9
#	define JSTR_OS_PLAN9 1
#elif defined pyr
#	define JSTR_OS_PYRAMIDDCOSX 1
#elif defined __QNX__ || defined __QNXNTO__
#	define JSTR_OS_QNSX 1
#elif defined sinux
#	define JSTR_OS_RELIANTUNIX 1
#elif defined M_I386 || defined M_XENIX || defined _SCO_DS
#	define JSTR_OS_SCOOPENSERVER 1
#elif defined sun || defined __sun
#	if defined __SVR4 || defined __svr4__
#		define JSTR_OS_SOLARIS 1
#	else
#		define JSTR_OS_SUNOS 1
#	endif
#elif defined __VOS__
#	define JSTR_OS_STRATUSVOX 1
#elif defined __SYLLABLE__
#	define JSTR_OS_SYLLABLE 1
#elif defined __SYMBIAN32__
#	define JSTR_OS_SYMBIAN 1
#elif defined __osf__ || defined __osf
#	define JSTR_OS_TRU64OSF1 1
#elif defined ultrix || defined __ultrix || defined __ultrix__ || (defined unix && defined vax)
#	define JSTR_OS_ULTRIX 1
#elif defined _UNICOS
#	define JSTR_OS_UNIC 1
#elif defined sco || defined _UNIXWARE7
#	define JSTR_OS_UNIXWARE 1
#elif defined VMS || defined __VMS
#	define JSTR_OS_VMS 1
#elif defined __VXWORKS__ || defined __vxworks
#	define JSTR_OS_VXWORKS 1
#elif defined _WIN16 || defined _WIN32 || defined _WIN64 || defined __WIN32__ || defined __TOS_WIN__ || defined __WINDOWS__
#	define JSTR_OS_WINDOWS 1
#elif defined _WIN32_WCE
#	define JSTR_OS_WINDOWSCE 1
#elif defined _WINDU_SOURCE
#	define JSTR_OS_WINDU 1
#elif defined __MVS__ || defined __HOS_MVS__ || defined __TOS_MVS__
#	define JSTR_OS_Z 1
#else
#	define JSTR_OS_GENERIC 1
#endif /* os */

#if defined __unix__ || defined __unix
#	define JSTR_ENV_UNIX 1
#endif
#if defined __FreeBSD__ || defined __NetBSD__ || defined __OpenBSD__ || defined __bsdi__ || defined __DragonFly__ || defined _SYSTYPE_BSD
#	define JSTR_ENV_BSD 1
#endif
#ifdef __CYGWIN__
#	define JSTR_ENV_CYGWIN 1
#endif
#ifdef __INTERIX
#	define JSTR_ENV_INTERIX 1
#endif
#if defined __sysv__ || defined __SVR4 || defined __svr4__ || defined _SYSTYPE_SRV4
#	define JSTR_ENV_SVR4 1
#endif
#if defined _UWIN
#	define JSTR_ENV_UWIN 1
#endif /* env */

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
#elif defined __loongarch64
#	define JSTR_ARCH_LOONGARCH64 1
#elif defined __loongarch__
#	define JSTR_ARCH_LOONGARCH 1
#elif defined __CSKY__
#	define JSTR_ARCH_CSKY 1
#else
#	define JSTR_ARCH_GENERIC 1
#endif /* arch */

PJSTR_BEGIN_DECLS
#if JSTR_OS_SOLARIS
#	include <sys/int_types.h>
#else
#	include <stdint.h>
#endif
PJSTR_END_DECLS

#if JSTR_ENV_BSD
PJSTR_BEGIN_DECLS
#	include <sys/endian.h>
PJSTR_END_DECLS
#elif (JSTR_GLIBC_PREREQ(2, 19) && defined _BSD_SOURCE) \
|| defined _DEFAULT_SOURCE
PJSTR_BEGIN_DECLS
#	include <endian.h>
PJSTR_END_DECLS
#endif

#ifdef __BYTE_ORDER
#	define JSTR_BYTE_ORDER __BYTE_ORDER
#elif defined _BYTE_ORDER
#	define JSTR_BYTE_ORDER _BYTE_ORDER
#endif

#ifdef __BIG_ENDIAN
#	define JSTR_BIG_ENDIAN __BIG_ENDIAN
#elif defined _BIG_ENDIAN
#	define JSTR_BIG_ENDIAN _BIG_ENDIAN
#endif

#ifdef __LITTLE_ENDIAN
#	define JSTR_LITTLE_ENDIAN __LITTLE_ENDIAN
#elif defined _LITTLE_ENDIAN
#	define JSTR_LITTLE_ENDIAN _LITTLE_ENDIAN
#endif

#ifdef JSTR_BYTE_ORDER
#	if JSTR_BYTE_ORDER == JSTR_BIG_ENDIAN \
	|| defined __BIG_ENDIAN__              \
	|| defined __ARMEB__                   \
	|| defined __THUMBEB__                 \
	|| defined __AARCH64EB__               \
	|| defined _MIBSEB                     \
	|| defined __MIBSEB                    \
	|| defined __MIBSEB__
#		undef JSTR_ENDIAN_BIG
#		undef JSTR_ENDIAN_LITTLE
#		define JSTR_ENDIAN_BIG    1
#		define JSTR_ENDIAN_LITTLE 0
#	elif JSTR_BYTE_ORDER == JSTR_LITTLE_ENDIAN \
	|| defined __LITTLE_ENDIAN__                \
	|| defined __ARMEL__                        \
	|| defined __THUMBEL__                      \
	|| defined __AARCH64EL__                    \
	|| defined _MIPSEL                          \
	|| defined __MIPSEL                         \
	|| defined __MIPSEL__
#		undef JSTR_ENDIAN_LITTLE
#		undef JSTR_ENDIAN_BIG
#		define JSTR_ENDIAN_LITTLE 1
#		define JSTR_ENDIAN_BIG    0
#	endif
#endif /* byte_order */

#if !JSTR_ENDIAN_BIG && !JSTR_ENDIAN_LITTLE
#	error "Can't detect endianness."
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
#endif /* have_unlocked_io */

#ifdef __GLIBC__
#	ifdef _DIRENT_HAVE_D_TYPE
#		define JSTR_HAVE_DIRENT_D_TYPE 1
#	else
#		define JSTR_HAVE_DIRENT_D_TYPE 0
#	endif
#	ifdef _DIRENT_HAVE_D_RECLEN
#		define JSTR_HAVE_DIRENT_D_RECLEN 1
#	else
#		define JSTR_HAVE_DIRENT_D_RECLEN 0
#	endif
#	ifdef _DIRENT_HAVE_D_NAMLEN
#		define JSTR_HAVE_DIRENT_D_NAMLEN 1
#	else
#		define JSTR_HAVE_DIRENT_D_NAMLEN 0
#	endif
#endif /* dirent_have */

/* Check optimizations. */
#ifdef __GLIBC__
#	if JSTR_HAVE_MEMMEM && JSTR_ARCH_S390
#		define JSTR_HAVE_MEMMEM_OPTIMIZED 1
#	endif
#	if JSTR_ARCH_X86_64 || JSTR_ARCH_POWERPC7 || JSTR_ARCH_POWERPC64 || JSTR_ARCH_S390
#		define JSTR_HAVE_STRSTR_OPTIMIZED 1
#	endif
#	if JSTR_HAVE_STRCASESTR && (JSTR_ARCH_POWERPC64 || JSTR_ARHC_POWERPC8)
#		define JSTR_HAVE_STRCASESTR_OPTIMIZED 1
#	endif
#	if JSTR_ARCH_X86_64 || JSTR_ARCH_S390 || JSTR_ARCH_I386 || JSTR_ARCH_SPARC || JSTR_ARCH_POWERPC64 || JSTR_ARCH_POWERPC8
#		define JSTR_HAVE_STRCSPN_OPTIMIZED 1
#	endif
#	if JSTR_ARCH_X86_64 || JSTR_ARCH_S390 || JSTR_ARCH_I386 || JSTR_ARCH_SPARC
#		define JSTR_HAVE_STRPBRK_OPTIMIZED 1
#	endif
#	if JSTR_ARCH_ARM64 || JSTR_ARCH_ALPHA || JSTR_ARCH_I386 || JSTR_ARCH_IA64 || JSTR_ARCH_M68K || JSTR_ARCH_POWERPC64 || JSTR_ARCH_POWERPC32 || JSTR_ARCH_POWERPC7 || JSTR_ARCH_POWERPC8 || JSTR_ARCH_POWERPC4 || JSTR_ARCH_S390 || JSTR_ARCH_X86_64 || JSTR_ARCH_X86_32 || JSTR_ARCH_LOONGARCH64 || JSTR_ARCH_SPARC
#		define JSTR_HAVE_STRCHR_OPTIMIZED 1
#	endif
#	if JSTR_ARCH_ARM64 || JSTR_ARCH_ARM6 || JSTR_ARCH_ALPHA || JSTR_ARCH_I386 || JSTR_ARCH_POWERPC64 || JSTR_ARCH_POWERPC32 || JSTR_ARCH_POWERPC7 || JSTR_ARCH_POWERPC8 || JSTR_ARCH_POWERPC9 || JSTR_ARCH_IA64 || JSTR_ARCH_I386 || JSTR_ARCH_S390 || JSTR_ARCH_SPARC || JSTR_ARCH_X86_64 || JSTR_ARCH_X86_32 || JSTR_ARCH_LOONGARCH64 || JSTR_ARCH_CSKY
#		define JSTR_HAVE_STRCPY_OPTIMIZED 1
#	endif
#	if JSTR_ARCH_ARM6T2 || JSTR_ARCH_ARM6 || JSTR_ARCH_ARM || JSTR_ARCH_POWERPC64 || JSTR_ARCH_POWERPC32 || JSTR_ARCH_POWERPC7 || JSTR_ARCH_POWERPC8 || JSTR_ARCH_POWERPC9 || JSTR_ARCH_POWERPC4 || JSTR_ARCH_LOONGARCH64 || JSTR_ARCH_ALPHA || JSTR_ARCH_I386 || JSTR_ARCH_IA64 || JSTR_ARCH_X86_64 || JSTR_ARCH_X86_32 || JSTR_ARCH_SH || JSTR_ARCH_SPARC || JSTR_ARCH_CSKY
#		define JSTR_HAVE_STRLEN_OPTIMIZED 1
#	endif
#endif /* have_optimized */

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
#else /* jstr_arch_generic */
#	if (JSTR_HAS_BUILTIN(__builtin_clzl) || defined __builtin_clzl)  \
	&& (JSTR_HAS_BUILTIN(__builtin_clzll) || defined __builtin_clzll) \
	&& (JSTR_HAS_BUILTIN(__builtin_ctzl) || defined __builtin_ctzl)   \
	&& (JSTR_HAS_BUILTIN(__builtin_ctzll) || defined __builtin_ctzll)
#		define JSTR_HAVE_WORD_AT_A_TIME 1
#	endif
#endif /* have_word_at_a_time */

#if !JSTR_USE_LGPL
#	undef JSTR_HAVE_WORD_AT_A_TIME
#endif

#endif /* jstr_macros_h */
