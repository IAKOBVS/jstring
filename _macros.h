#ifndef JSTR_MACROS_H_DEF
#define JSTR_MACROS_H_DEF

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L
#	define JSTR_HAVE_GENERIC 1
#endif /* HAVE_GENERIC */

#if defined JSTR_HAVE_TYPEOF && defined JSTR_HAVE_GENERIC
#	define JSTR_SAME_TYPE(x, y) _Generic((x),           \
					      typeof(y) : 1, \
					      default : 0)
#	define JSTR_IS_TYPE(T, x) _Generic((x),   \
					    T : 1, \
					    default : 0)
#endif /* HAVE_TYPEOF && JSTR_HAVE_GENERIC */

#if (defined __GNUC__ || defined __clang__) && JSTR_HAVE_GENERIC
#	define JSTR_GENERIC_CASE_SIZE(_expr)                           \
		int : _expr,                                            \
		      unsigned int : _expr,                             \
				     size_t : _expr,                    \
					      long : _expr,             \
						     long long : _expr, \
								 unsigned long long : _expr
#	define JSTR_GENERIC_CASE_STR(_bool) \
		char * : _bool,              \
			 const char * : _bool

#	define JSTR_GENERIC_CASE_STR_STACK(_bool, _s) \
		char(*)[sizeof(_s)] : 1,               \
		const char(*)[sizeof(_s)] : 1

#	define JSTR_GENERIC_CASE_CHAR(_bool) \
		char : _bool,                 \
		       const char : _bool

#	define JSTR_IS_SIZE(_expr) _Generic((_expr),                   \
					     JSTR_GENERIC_CASE_SIZE(1), \
					     default : 0)

#	define JSTR_IS_STR(_expr) _Generic((_expr),                  \
					    JSTR_GENERIC_CASE_STR(1), \
					    default : 0)

#	define JSTR_IS_STR_STACK(_expr) _Generic((_expr),                               \
						  JSTR_GENERIC_CASE_STR_STACK(1, _expr), \
						  default : 0)

#	define JSTR_IS_CHAR(_expr) _Generic((_expr),                   \
					     JSTR_GENERIC_CASE_CHAR(1), \
					     default : 0)

#	define JSTR_ASSERT_IS_SIZE(_expr) \
		JSTR_ASSERT(JSTR_IS_SIZE(_expr), "Passing non-number as number argument!");
#	define JSTR_ASSERT_IS_STR(_expr) \
		JSTR_ASSERT(JSTR_IS_STR(_expr), "Passing non-string as string argument!");
#	define JSTR_ASSERT_IS_CHAR(_expr) \
		JSTR_ASSERT(JSTR_IS_CHAR(_expr), "Passing non-char as char argument!");
#	define JSTR_ASSERT_TYPECHECK(_expr_ty, _expr) \
		JSTR_ASSERT(JSTR_SAME_TYPE(_expr_ty, _expr), "Passing the wrong data type!");
#else
#	define JSTR_GENERIC_CASE_SIZE(_expr)
#	define JSTR_GENERIC_CASE_STR(_bool)
#	define JSTR_GENERIC_CASE_CHAR(_bool)
#	define JSTR_IS_STR(_expr)
#	define JSTR_IS_CHAR(_expr)
#	define JSTR_ASSERT_IS_SIZE(_expr)
#	define JSTR_ASSERT_IS_STR(_expr)
#	define JSTR_ASSERT_IS_CHAR(_expr)
#	define JSTR_ASSERT_TYPECHECK(_expr_ty, _expr)
#endif /* __GNUC__ || __clang__ */

#if __cplusplus > 199711L
#	define JSTR_NOEXCEPT noexcept
#else
#	define JSTR_NOEXCEPT
#endif

#if (defined __GNUC__ && (__GNUC__ >= 4)) || (defined __clang__ && (__clang_major__ >= 3))
#	define JSTR_HAVE_TYPEOF 1
#endif /* HAVE_TYPEOF */

#ifdef static_assert
#	define JSTR_HAVE_STATIC_ASSERT		  1
#	define JSTR_ASSERT(_expr, msg)		  static_assert(_expr, msg)
#	define JSTR_ASSERT_SEMICOLON(_expr, msg) static_assert(_expr, msg);
#elif __STDC_VERSION__ >= 201112L
#	define JSTR_HAVE_STATIC_ASSERT 1
#	define JSTR_ASSERT(_expr, msg) _Static_assert(_expr, msg)
#else
#	define JSTR_ASSERT(_expr, msg)
#	define JSTR_ASSERT_SEMICOLON(_expr, msg)
#endif /* static_assert */

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#	define JSTR_RESTRICT restrict
#elif defined __GNUC__ || defined __clang__
#	define JSTR_RESTRICT __restrict__
#elif defined _MSC_VER
#	define JSTR_RESTRICT __restrict
#else
#	define JSTR_RESTRICT
#endif /* restrict */

#if (defined __GNUC__ && (__GNUC__ >= 3)) || (defined __clang__ && __has_builtin(__builtin_expect))
#	define jstr_likely(x)	 __builtin_expect(!!(x), 1)
#	define jstr_unlikely(x) __builtin_expect(!!(x), 0)
#else
#	define jstr_likely(x)	 (x)
#	define jstr_unlikely(x) (x)
#endif /* __has_builtin(__builtin_expect) */

#if defined __GNUC__ || defined __clang__

#	define JSTR_INLINE __attribute__((always_inline)) inline
#	if __has_attribute(pure)
#		define JSTR_PURE __attribute__((pure))
#	else
#		define JSTR_PURE
#	endif /* JSTR_PURE */
#	if __has_attribute(const)
#		define JSTR_CONST __attribute__((const))
#	else
#		define JSTR_CONST
#	endif /* JSTR_CONST */
#	if __has_attribute(flatten)
#		define JSTR_FLATTEN __attribute__((flatten))
#	else
#		define JSTR_FLATTEN
#	endif /* JSTR_FLATTEN */
#	if __has_attribute(cold)
#		define JSTR_COLD __attribute__((cold))
#	else
#		define JSTR_COLD
#	endif /* JSTR_COLD */
#	if __has_attribute(sentinel)
#		define JSTR_SENTINEL __attribute__((sentinel))
#	else
#		define JSTR_SENTINEL
#	endif /* JSTR_SENTINEL */
#	if __has_attribute(nonnull)
#		define JSTR_NONNULL_ALL   __attribute__((nonnull))
#		define JSTR_NONNULL(args) __attribute__((nonnull(args)))
#	else
#		define JSTR_NONNULL_ALL
#		define JSTR_NONNULL(args)
#	endif /* JSTR_NONNULL */
#	if __has_attribute(malloc)
#		define JSTR_MALLOC				      __attribute__((malloc))
#		define JSTR_MALLOC_DEALLOC(deallocator)	      __attribute__((malloc, deallocator))
#		define JSTR_MALLOC_DEALLOC_PTR(deallocator, ptr_idx) __attribute__((malloc, deallocator, ptr_idx))
#	endif /* JSTR_MALLOC */
#	if __has_attribute(returns_nonnull)
#		define JSTR_RETURNS_NONNULL __attribute__((returns_nonnull))
#	else
#		define JSTR_RETURNS_NONNULL
#	endif /* RETURNS_NONNULL */
#	if __has_attribute(warn_unused_result)
#		define JSTR_WARN_UNUSED __attribute__((warn_unused_result))
#	else
#		define JSTR_WARN_UNUSED
#	endif /* */
#	if __has_builtin(__builtin_constant_p)
#		define JSTR_CONSTANT_P(p) __builtin_constant_p(p)
#	else
#		define JSTR_CONSTANT_P(p) 0
#	endif /* __has_builtin(__builtin_constant_p) */
#	if __has_builtin(deprecated)
#		define JSTR_DEPRECATED(msg, replacement) __attribute__((deprecated(msg, replacement)))
#	else
#		define JSTR_DEPRECATED(msg, replacement)
#	endif /* JSTR_DEPRECATED */
#	if __has_attribute(nothrow)
#		define JSTR_NOTHROW __attribute__((nothrow))
#	else
#		define JSTR_NOTHROW
#	endif

#elif defined _MSC_VER

#	define JSTR_INLINE   __forceinline inline
#	define JSTR_NOINLINE __declspec(noinline)
#	define JSTR_PURE     __declspec(noalias)
#	define JSTR_CONST    __declspec(restrict)
#	define JSTR_FLATTEN
#	define JSTR_COLD
#	define JSTR_SENTINEL
#	define JSTR_NONNULL_ALL
#	define JSTR_NONNULL(args)
#	define JSTR_MALLOC
#	define JSTR_MALLOC_DEALLOC(deallocator)
#	define JSTR_MALLOC_DEALLOC_PTR(deallocator, ptr_idx)
#	define JSTR_RETURNS_NONNULL
#	define JSTR_CONSTANT_P(p) 0
#	define JSTR_WARN_UNUSED
#	define JSTR_DEPRECATED(msg, replacement)
#	define JSTR_NOTHROW __declspec(nothrow)

#else

#	define JSTR_INLINE inline
#	define JSTR_NOINLINE
#	define JSTR_PURE
#	define JSTR_CONST
#	define JSTR_FLATTEN
#	define JSTR_COLD
#	define JSTR_SENTINEL
#	define JSTR_NONNULL_ALL
#	define JSTR_NONNULL(args)
#	define JSTR_MALLOC
#	define JSTR_MALLOC_DEALLOC(deallocator)
#	define JSTR_MALLOC_DEALLOC_PTR(deallocator, ptr_idx)
#	define JSTR_CONSTANT_P(p) 0
#	define JSTR_WARN_UNUSED
#	define JSTR_DEPRECATED(msg, replacement)
#	define JSTR_NOTHROW

#endif /* Gnuc || clang || msvc */

#if defined __GNUC__ || defined __clang__
#	define JSTR_MAYBE_UNUSED __attribute__((unused))
#else
#	define JSTR_MAYBE_UNUSED
#endif /* maybe_unused */

#if defined __GNUC__ || defined __clang__
#	define JSTR_NOINLINE __attribute__((noinline))
#elif defined _MSC_VER
#	define JSTR_NOINLINE __declspec(noinline)
#else
#	define JSTR_NOINLINE
#endif /* noinline */

#ifndef JSTR_MAX
#	define JSTR_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif /* Max */

#ifndef JSTR_MIN
#	define JSTR_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif /* Min */

#define JSTR_CASE_VOWEL_LOWER \
case 'a':                     \
case 'i':                     \
case 'u':                     \
case 'e':                     \
case 'o':

#define JSTR_CASE_VOWEL_UPPER \
case 'A':                     \
case 'I':                     \
case 'U':                     \
case 'E':                     \
case 'O':

#define JSTR_CASE_CONSONANT_LOWER \
case 'b':                         \
case 'c':                         \
case 'd':                         \
case 'f':                         \
case 'g':                         \
case 'h':                         \
case 'j':                         \
case 'k':                         \
case 'l':                         \
case 'm':                         \
case 'n':                         \
case 'p':                         \
case 'q':                         \
case 'r':                         \
case 's':                         \
case 't':                         \
case 'v':                         \
case 'w':                         \
case 'x':                         \
case 'y':                         \
case 'z':
#define JSTR_CASE_CONSONANT_UPPER \
case 'B':                         \
case 'C':                         \
case 'D':                         \
case 'F':                         \
case 'G':                         \
case 'H':                         \
case 'J':                         \
case 'K':                         \
case 'L':                         \
case 'M':                         \
case 'N':                         \
case 'P':                         \
case 'Q':                         \
case 'R':                         \
case 'S':                         \
case 'T':                         \
case 'V':                         \
case 'W':                         \
case 'X':                         \
case 'Y':                         \
case 'Z':

#define JSTR_CASE_DIGIT \
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

#define JSTR_CASE_LOWER \
case 'a':               \
case 'b':               \
case 'c':               \
case 'd':               \
case 'e':               \
case 'f':               \
case 'g':               \
case 'h':               \
case 'i':               \
case 'j':               \
case 'k':               \
case 'l':               \
case 'm':               \
case 'n':               \
case 'o':               \
case 'p':               \
case 'q':               \
case 'r':               \
case 's':               \
case 't':               \
case 'u':               \
case 'v':               \
case 'w':               \
case 'x':               \
case 'y':               \
case 'z':

#define JSTR_CASE_UPPER \
case 'A':               \
case 'B':               \
case 'C':               \
case 'D':               \
case 'E':               \
case 'F':               \
case 'G':               \
case 'H':               \
case 'I':               \
case 'J':               \
case 'K':               \
case 'L':               \
case 'M':               \
case 'N':               \
case 'O':               \
case 'P':               \
case 'Q':               \
case 'R':               \
case 'S':               \
case 'T':               \
case 'U':               \
case 'V':               \
case 'W':               \
case 'X':               \
case 'Y':               \
case 'Z':

#define JSTR_CASE_VOWEL       \
	JSTR_CASE_VOWEL_UPPER \
	JSTR_CASE_VOWEL_LOWER

#define JSTR_CASE_CONSONANT       \
	JSTR_CASE_CONSONANT_UPPER \
	JSTR_CASE_CONSONANT_LOWER

#define JSTR_CASE_ALPHA \
	JSTR_CASE_LOWER \
	JSTR_CASE_UPPER

#define JSTR_CASE_ALPHANUM \
	JSTR_CASE_DIGIT    \
	JSTR_CASE_ALPHA

#define JSTR_CASE_WHITESPACE \
case '\t':                   \
case '\n':                   \
case '\v':                   \
case '\f':                   \
case '\r':                   \
case ' ':

#if (__GLIBC__ < 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ <= 19) && defined _BSD_SOURCE) \
|| defined _DEFAULT_SOURCE
#	define JSTR_HAVE_STRCASECMP  1
#	define JSTR_HAVE_STRNCASECMP 1
#endif

#ifdef _XOPEN_SOURCE
#	define JSTR_HAVE_MEMCCPY 1
#endif /* Misc || Xopen */

#ifdef _GNU_SOURCE
#	define JSTR_HAVE_MEMMEM	     1
#	define JSTR_HAVE_MEMRCHR	     1
#	define JSTR_HAVE_STRCHRNUL	     1
#	define JSTR_HAVE_FGETS_UNLOCKED     1
#	define JSTR_HAVE_FPUTS_UNLOCKED     1
#	define JSTR_HAVE_GETWC_UNLOCKED     1
#	define JSTR_HAVE_GETWCCHAR_UNLOCKED 1
#	define JSTR_HAVE_FGETWC_UNLOCKED    1
#	define JSTR_HAVE_FPUTWC_UNLOCKED    1
#	define JSTR_HAVE_PUTWCHAR_UNLOCKED  1
#	define JSTR_HAVE_FGETWS_UNLOCKED    1
#	define JSTR_HAVE_FPUTWS_UNLOCKED    1
#	define JSTR_HAVE_WMEMPCPY	     1
#	define JSTR_HAVE_MEMPCPY	     1
#	define JSTR_HAVE_STRCASESTR	     1
#endif /* Gnu */

#if (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 24) && _POSIX_C_SOURCE >= 199309L) \
|| ((__GLIBC__ == 2 && __GLIBC_MINOR__ <= 19) && defined _SVID_SOURCE || defined _BSD_SOURCE)  \
|| (__GLIBC__ == 2 && __GLIBC_MINOR__ <= 23 && defined _POSIX_C_SOURCE)
#	define JSTR_HAVE_GETC_UNLOCKED	   1
#	define JSTR_HAVE_GETCHAR_UNLOCKED 1
#	define JSTR_HAVE_PUTC_UNLOCKED	   1
#	define JSTR_HAVE_PUTCHAR_UNLOCKED 1
#endif /* Posix || Bsd  */

#if (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 19) && defined _DEFAULT_SOURCE) \
|| defined _SVID_SOURCE || defined _BSD_SOURCE
#	define JSTR_HAVE_FREAD_UNLOCKED    1
#	define JSTR_HAVE_FWRITE_UNLOCKED   1
#	define JSTR_HAVE_FPUTC_UNLOCKED    1
#	define JSTR_HAVE_FGETC_UNLOCKED    1
#	define JSTR_HAVE_CLEARERR_UNLOCKED 1
#	define JSTR_HAVE_FEOF_UNLOCKED	    1
#	define JSTR_HAVE_FERROR_UNLOCKED   1
#	define JSTR_HAVE_FILENO_UNLOCKED   1
#	define JSTR_HAVE_FFLUSH_UNLOCKED   1
#endif /* Default || Svid || Bsd */

#if (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 10)) && _POSIX_C_SOURCE >= 200809L \
|| defined _GNU_SOURCE
#	define JSTR_HAVE_STPCPY  1
#	define JSTR_HAVE_STRNLEN 1
#	define JSTR_HAVE_STRNDUP 1
#endif /* Posix || Gnu */

#if _XOPEN_SOURCE >= 500                                                                      \
|| (__GLIBC__ == 2 && __GLIBC_MINOR__ <= 19 && (defined _BSD_SOURCE || defined _SVID_SOURCE)) \
|| (__GLIBC__ > 2 || __GLIBC__ == 2 && __GLIBC_MINOR__ >= 12 && __POSIX_C_SOURCE >= 200809L)
#	define JSTR_HAVE_STRDUP 1
#endif /* Xopen || Bsd || Svid || Posix */

#ifdef _GNU_SOURCE
#	define JSTR_HAVE_STRDUPA  1
#	define JSTR_HAVE_STRNDUPA 1
#endif /* Gnu */

#if defined __linux__ && defined __GLIBC__
#	define JSTR_HAVE_REALLOC_MREMAP 1
#endif /* Gnu */

#define JSTR_RST JSTR_RESTRICT

/* Helper macros for pointer arithmetic.
   Copyright (C) 2012-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library.  */

#include <stdint.h>

#if 0

/* 1 if 'type' is a pointer type, 0 otherwise.  */
#	define __pointer_type(type) (__builtin_classify_type((type)0) == 5)

/* intptr_t if P is true, or T if P is false.  */
#	define __integer_if_pointer_type_sub(T, P)                      \
		__typeof__(*(0 ? (__typeof__(0 ? (T *)0 : (void *)(P)))0 \
			       : (__typeof__(0 ? (intptr_t *)0 : (void *)(!(P))))0))

/* intptr_t if EXPR has a pointer type, or the type of EXPR otherwise.  */
#	define __integer_if_pointer_type(expr)                                \
		__integer_if_pointer_type_sub(__typeof__((__typeof__(expr))0), \
					      __pointer_type(__typeof__(expr)))

/* Cast an integer or a pointer VAL to integer with proper type.  */
#	define cast_to_integer(val) ((__integer_if_pointer_type(val))(val))

/* Cast an integer VAL to void * pointer.  */
#	define cast_to_pointer(val) ((void *)(uintptr_t)(val))

#endif

/* Align a value by rounding down to closest size.
   e.g. Using size of 4096, we get this behavior:
	{4095, 4096, 4097} = {0, 4096, 4096}.  */
#define PJSTR_ALIGN_DOWN(base, size) ((base) & -((uintptr_t)(size)))

/* Align a value by rounding up to closest size.
   e.g. Using size of 4096, we get this behavior:
	{4095, 4096, 4097} = {4096, 4096, 8192}.

  Note: The size argument has side effects (expanded multiple times).  */
#define PJSTR_ALIGN_UP(base, size) PJSTR_ALIGN_DOWN((base) + (size)-1, (size))

/* Same as ALIGN_DOWN(), but automatically casts when base is a pointer.  */
#define PJSTR_PTR_ALIGN_DOWN(base, size) \
	(PJSTR_ALIGN_DOWN((uintptr_t)(base), (size)))

/* Same as ALIGN_UP(), but automatically casts when base is a pointer.  */
#define PJSTR_PTR_ALIGN_UP(base, size) \
	(PJSTR_ALIGN_UP((uintptr_t)(base), (size)))

/* Check if BASE is aligned on SIZE  */
#define PJSTR_PTR_IS_ALIGNED(base, size) \
	((((uintptr_t)(base)) & ((size)-1)) == 0)

/* Returns the ptrdiff_t diference between P1 and P2.  */
#define PJSTR_PTR_DIFF(p1, p2) \
	((ptrdiff_t)((uintptr_t)(p1) - (uintptr_t)(p2)))

#endif /* JSTR_MACROS_H_DEF */
