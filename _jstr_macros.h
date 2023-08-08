#ifndef JSTR_MACROS_H_DEF
#define JSTR_MACROS_H_DEF

/* #if defined(__GNUC__) || defined(__clang__) */
/* #	ifdef __cplusplus */
/* #		include <cstdint> */
/* #	else */
/* #		include <stdint.h> */
/* #	endif /1* __cplusplus *1/ */
/* #	if __has_builtin(__builtin_clzll) */
/* #		ifdef __cplusplus */
/* extern "C" { */
/* #		endif */

/* #		ifdef JSTR_64_BIT */
/* JSTR_INLINE */
/* JSTR_CONST */
/* JSTR_WARN_UNUSED */
/* uint64_t private_jstr_next_pow2_64(uint64_t x) JSTR_NOEXCEPT */
/* { */
/* 	return 1ull << (64 - __builtin_clzll(x - 1)); */
/* } */

/* #		endif /1* JSTR_64_BIT *1/ */

/* #	endif /1* __has_builtin(__builtin_clzll) *1/ */
/* #	if __has_builtin(__builtin_clz) */

/* #		ifdef JSTR_32_BIT */
/* JSTR_INLINE */
/* JSTR_CONST */
/* JSTR_WARN_UNUSED */
/* uint32_t private_jstr_next_pow2_32(uint32_t x) JSTR_NOEXCEPT */
/* { */
/* 	return 1 << (32 - __builtin_clz(x - 1)); */
/* } */

/* #		endif /1* JSTR_32_BIT *1/ */

/* #		ifdef __cplusplus */
/* } */
/* #		endif */
/* #	endif /1* __has_builtin(__builtin_clz) *1/ */
/* #elif defined(_MSC_VER) */
/* #	include <stdint.h> */
/* #	include <intrin.h> */
/* #	pragma intrinsic(_BitScanReverse64) */
/* #	ifdef __cplusplus */
/* extern "C" { */
/* #	endif */

/* #	ifdef JSTR_32_BIT */
/* JSTR_INLINE */
/* JSTR_CONST */
/* JSTR_WARN_UNUSED */
/* uint32_t private_jstr_next_pow2_32(uint32_t x) JSTR_NOEXCEPT */
/* { */
/* 	unsigned long index; */
/* 	_BitScanReverse(&index, x - 1); */
/* 	return 1 << (index + 1); */
/* } */

/* #	endif /1* JSTR_32_BIT *1/ */

/* #	ifdef JSTR_64_BIT */
/* JSTR_INLINE */
/* JSTR_CONST */
/* JSTR_WARN_UNUSED */
/* uint64_t private_jstr_next_pow2_64(uint64_t x) JSTR_NOEXCEPT */
/* { */
/* 	unsigned long index; */
/* 	_BitScanReverse64(&index, x - 1); */
/* 	return 1ull << (index + 1); */
/* } */

/* #	endif /1* JSTR_64_BIT *1/ */

/* #else */

/* #	ifdef JSTR_32_BIT */

/* JSTR_INLINE */
/* JSTR_CONST */
/* JSTR_WARN_UNUSED */
/* size_t private_jstr_next_pow2_32(size_t x) JSTR_NOEXCEPT */
/* { */
/* 	--x; */
/* 	x |= x >> 1; */
/* 	x |= x >> 2; */
/* 	x |= x >> 4; */
/* 	x |= x >> 8; */
/* 	x |= x >> 16; */
/* 	return x + 1; */
/* } */

/* #	endif /1* JSTR_32_BIT *1/ */

/* #	ifdef JSTR_64_BIT */

/* JSTR_INLINE */
/* JSTR_CONST */
/* JSTR_WARN_UNUSED */
/* size_t private_jstr_next_pow2_64(size_t x) JSTR_NOEXCEPT */
/* { */
/* 	--x; */
/* 	x |= x >> 1; */
/* 	x |= x >> 2; */
/* 	x |= x >> 4; */
/* 	x |= x >> 8; */
/* 	x |= x >> 16; */
/* 	x |= x >> 32; */
/* 	return x + 1; */
/* } */

/* #	endif /1* JSTR_64_BIT *1/ */

/* #	ifdef __cplusplus */
/* } */
/* #	endif */
/* #endif /1* __GNUC__ || __clang__ *1/ */

/* #ifdef __cplusplus */

/* #	include <type_traits> */

/* JSTR_INLINE */
/* JSTR_CONST */
/* JSTR_WARN_UNUSED */
/* static constexpr std::size_t private_jstr_next_pow2_32_constexpr(std::size_t x) JSTR_NOEXCEPT */
/* { */
/* 	--x; */
/* 	x |= x >> 1; */
/* 	x |= x >> 2; */
/* 	x |= x >> 4; */
/* 	x |= x >> 8; */
/* 	x |= x >> 16; */
/* 	return x + 1; */
/* } */

/* JSTR_INLINE */
/* JSTR_CONST */
/* JSTR_WARN_UNUSED */
/* static constexpr std::size_t private_jstr_next_pow2_64_constexpr(std::size_t x) JSTR_NOEXCEPT */
/* { */
/* 	--x; */
/* 	x |= x >> 1; */
/* 	x |= x >> 2; */
/* 	x |= x >> 4; */
/* 	x |= x >> 8; */
/* 	x |= x >> 16; */
/* 	x |= x >> 32; */
/* 	return x + 1; */
/* } */

/* template <typename T> */
/* JSTR_INLINE */
/* JSTR_WARN_UNUSED */
/* JSTR_CONST static std::size_t */
/* JSTR_NEXT_POW2_32(T x) */
/* { */
/* 	if */
/* #	if __cplusplus > 201703L */
/* 	constexpr */
/* #	endif /1* cpp 17 *1/ */
/* 	(std::is_integral<T>::value) */
/* 		return private_jstr_next_pow2_32_constexpr(x); */
/* 	return private_jstr_next_pow2_32(x); */
/* } */

/* template <typename T> */
/* JSTR_INLINE */
/* JSTR_WARN_UNUSED */
/* JSTR_CONST static std::size_t */
/* JSTR_NEXT_POW2_64(T x) */
/* { */
/* 	if */
/* #	if __cplusplus > 201703L */
/* 	constexpr */
/* #	endif /1* cpp 17 *1/ */
/* 	(std::is_integral<T>::value) */
/* 		return private_jstr_next_pow2_64_constexpr(x); */
/* 	return private_jstr_next_pow2_64(x); */
/* } */

/* #endif /1* __cplusplus *1/ */

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#	define JSTR_HAS_GENERIC
#endif /* JSTR_HAS_GENERIC */

#if defined(JSTR_HAS_TYPEOF) && defined(JSTR_HAS_GENERIC)
#	define JSTR_SAME_TYPE(x, y) _Generic((x),           \
					      typeof(y) : 1, \
					      default : 0)
#	define JSTR_IS_TYPE(T, x) _Generic((x),   \
					    T : 1, \
					    default : 0)
#endif /* JSTR_HAS_TYPEOF && JSTR_HAS_GENERIC */

#if defined(__GNUC__) || defined(__clang__)
#	ifdef JSTR_HAS_GENERIC
#		define JSTR_GENERIC_CASE_SIZE(expr)                           \
			int : expr,                                            \
			      unsigned int : expr,                             \
					     size_t : expr,                    \
						      long : expr,             \
							     long long : expr, \
									 unsigned long long : expr
#		define JSTR_GENERIC_CASE_STR(bool_) \
			char * : bool_,              \
				 const char * : bool_

#		define JSTR_GENERIC_CASE_STR_STACK(bool_, s) \
			char(*)[sizeof(s)] : 1,               \
			const char(*)[sizeof(s)] : 1

#		define JSTR_GENERIC_CASE_CHAR(bool_) \
			char : bool_,                 \
			       const char : bool_

#		define JSTR_IS_SIZE(expr) _Generic((expr),                    \
						    JSTR_GENERIC_CASE_SIZE(1), \
						    default : 0)

#		define JSTR_IS_STR(expr) _Generic((expr),                   \
						   JSTR_GENERIC_CASE_STR(1), \
						   default : 0)

#		define JSTR_IS_STR_STACK(expr) _Generic((expr),                               \
							 JSTR_GENERIC_CASE_STR_STACK(1, expr), \
							 default : 0)

#		define JSTR_IS_CHAR(expr) _Generic((expr),                    \
						    JSTR_GENERIC_CASE_CHAR(1), \
						    default : 0)

#		define JSTR_ASSERT_IS_SIZE(expr) \
			JSTR_ASSERT(JSTR_IS_SIZE(expr), "Passing non-number as number argument!");
#		define JSTR_ASSERT_IS_STR(expr) \
			JSTR_ASSERT(JSTR_IS_STR(expr), "Passing non-string as string argument!");
#		define JSTR_ASSERT_IS_CHAR(expr) \
			JSTR_ASSERT(JSTR_IS_CHAR(expr), "Passing non-char as char argument!");
#		define JSTR_ASSERT_TYPECHECK(Texpr, expr) JSTR_ASSERT(JSTR_SAME_TYPE(Texpr, expr), "Passing the wrong data type!");
#	else
#		define JSTR_ASSERT_IS_SIZE(expr)
#		define JSTR_ASSERT_IS_STR(expr)
#		define JSTR_ASSERT_IS_CHAR(expr)
#		define JSTR_ASSERT_TYPECHECK(Texpr, expr)
#	endif /* JSTR_HAS_GENERIC */
#else
#	define JSTR_IS_SIZE(val)
#endif /* __GNUC__ || __clang__ */

#ifdef __cplusplus
#	define JSTR_NOEXCEPT noexcept
#else
#	define JSTR_NOEXCEPT
#endif

#if (defined(__GNUC__) && (__GNUC__ >= 4)) || (defined(__clang__) && (__clang_major__ >= 3))
#	define JSTR_HAS_TYPEOF
#endif /* JSTR_HAS_TYPEOF */

#ifdef static_assert
#	define JSTR_HAS_STATIC_ASSERT
#	define JSTR_ASSERT(expr, msg)		 static_assert(expr, msg)
#	define JSTR_ASSERT_SEMICOLON(expr, msg) static_assert(expr, msg);
#elif __STDC_VERSION__ >= 201112L
#	define JSTR_HAS_STATIC_ASSERT
#	define JSTR_ASSERT(expr, msg) _Static_assert(expr, msg)
#else
#	define JSTR_ASSERT(expr, msg)
#	define JSTR_ASSERT_SEMICOLON(expr, msg)
#endif /* static_assert */

#ifdef JSTR_ALIGN_POWER_OF_TWO
#	ifdef JSTR_64_BIT
#		ifdef __cplusplus
#			define JSTR_NEXT_POW2(x) JSTR_NEXT_POW2_64(x)
#		else
#			define JSTR_NEXT_POW2(x) private_jstr_next_pow2_64(x)
#		endif /* __cplusplus */
#	elif JSTR_32_BIT
#		ifdef __cplusplus
#			define JSTR_NEXT_POW2(x) JSTR_NEXT_POW2_32(x)
#		else
#			define JSTR_NEXT_POW2(x) private_jstr_next_pow2_32(x)
#		endif /* __cplusplus */
#	else
#		define JSTR_NEXT_POW2(x) (x)
#	endif /* JSTR_64_BIT */
#endif /* JSTR_ALIGN_POWER_OF_TWO */

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	define JSTR_RESTRICT restrict
#elif defined(__GNUC__) || defined(__clang__)
#	define JSTR_RESTRICT __restrict__
#elif defined(_MSC_VER)
#	define JSTR_RESTRICT __restrict
#else
#	define JSTR_RESTRICT
#endif /* restrict */

#if (defined(__GNUC__) && (__GNUC__ >= 3)) || (defined(__clang__) && __has_builtin(__builtin_expect))
#	define likely(x)   __builtin_expect(!!(x), 1)
#	define unlikely(x) __builtin_expect(!!(x), 0)
#else
#	define likely(x)   (x)
#	define unlikely(x) (x)
#endif /* __has_builtin(__builtin_expect) */

#if defined(__GNUC__) || defined(__clang__)

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

#elif defined(_MSC_VER)

#	define JSTR_INLINE __forceinline inline
#	define JSTR_PURE   __declspec(noalias)
#	define JSTR_CONST  __declspec(restrict)
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

#else

#	define JSTR_INLINE inline
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

#endif /* __GNUC__ || __clang__ || _MSC_VER */

#if defined(__GNUC__) || defined(__clang__)
#	define JSTR_MAYBE_UNUSED __attribute__((unused))
#else
#	define JSTR_MAYBE_UNUSED
#endif /* MAYBE_UNUSED */

#if defined(__GNUC__) || defined(__clang__)
#	define JSTR_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#	define JSTR_NOINLINE __declspec(noinline)
#else
#	define JSTR_NOINLINE
#endif /* JSTR_NOINLINE */

#ifndef JSTR_MAX
#	define JSTR_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif /* JSTR_MAX */

#ifndef JSTR_MIN
#	define JSTR_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif /* JSTR_MIN */

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
#define JSTR_CASE_VOWEL \
JSTR_CASE_VOWEL_UPPER:  \
JSTR_CASE_VOWEL_LOWER:

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
#define JSTR_CASE_CONSONANT \
JSTR_CASE_CONSONANT_UPPER:  \
JSTR_CASE_CONSONANT_LOWER:

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

#define JSTR_CASE_ALPHA	   JSTR_CASE_LOWER JSTR_CASE_UPPER
#define JSTR_CASE_ALPHANUM JSTR_CASE_DIGIT JSTR_CASE_ALPHA

#define JSTR_CASE_WHITESPACE \
case '\n':                   \
case '\t':                   \
case '\r':                   \
case ' ':

#if ((__GLIBC__ < 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ <= 19)) && defined(_BSD_SOURCE)) \
|| defined(_DEFAULT_SOURCE)
#	define JSTR_HAS_STRCASECMP
#	define JSTR_HAS_STRNCASECMP
#endif

#ifdef _XOPEN_SOURCE
#	define JSTR_HAS_MEMCCPY
#endif /* Misc || Xopen */

#ifdef _GNU_SOURCE
#	define JSTR_HAS_MEMMEM
#	define JSTR_HAS_MEMRCHR
#	define JSTR_HAS_STRCHRNUL
#	define JSTR_HAS_FGETS_UNLOCKED
#	define JSTR_HAS_FPUTS_UNLOCKED
#	define JSTR_HAS_GETWC_UNLOCKED
#	define JSTR_HAS_GETWCCHAR_UNLOCKED
#	define JSTR_HAS_FGETWC_UNLOCKED
#	define JSTR_HAS_FPUTWC_UNLOCKED
#	define JSTR_HAS_PUTWCHAR_UNLOCKED
#	define JSTR_HAS_FGETWS_UNLOCKED
#	define JSTR_HAS_FPUTWS_UNLOCKED
#	define JSTR_HAS_WMEMPCPY
#	define JSTR_HAS_MEMPCPY
#	define JSTR_HAS_STRCASESTR
#endif /* Gnu */

#if ((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 24)) && _POSIX_C_SOURCE >= 199309L) \
|| (((__GLIBC__ == 2) && (__GLIBC_MINOR__ <= 19)) && defined(_SVID_SOURCE)                           \
    || defined(_BSD_SOURCE))                                                                         \
|| ((__GLIBC__ == 2) && (__GLIBC_MINOR__ <= 23) && defined(_POSIX_C_SOURCE))
#	define JSTR_HAS_GETC_UNLOCKED
#	define JSTR_HAS_GETCHAR_UNLOCKED
#	define JSTR_HAS_PUTC_UNLOCKED
#	define JSTR_HAS_PUTCHAR_UNLOCKED
#endif /* Posix || Bsd  */

#if (__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 19))
#	ifdef _DEFAULT_SOURCE
#		define JSTR_HAS_FREAD_UNLOCKED
#		define JSTR_HAS_FWRITE_UNLOCKED
#		define JSTR_HAS_FPUTC_UNLOCKED
#		define JSTR_HAS_FGETC_UNLOCKED
#		define JSTR_HAS_CLEARERR_UNLOCKED
#		define JSTR_HAS_FEOF_UNLOCKED
#		define JSTR_HAS_FERROR_UNLOCKED
#		define JSTR_HAS_FILENO_UNLOCKED
#		define JSTR_HAS_FFLUSH_UNLOCKED
#	elif defined(_SVID_SOURCE) || defined(_BSD_SOURCE)
#		define JSTR_HAS_FREAD_UNLOCKED
#		define JSTR_HAS_FWRITE_UNLOCKED
#		define JSTR_HAS_FPUTC_UNLOCKED
#		define JSTR_HAS_FGETC_UNLOCKED
#		define JSTR_HAS_CLEARERR_UNLOCKED
#		define JSTR_HAS_FEOF_UNLOCKED
#		define JSTR_HAS_FERROR_UNLOCKED
#		define JSTR_HAS_FILENO_UNLOCKED
#		define JSTR_HAS_FFLUSH_UNLOCKED
#	endif
#endif /* Default || Svid || Bsd */

#if ((__GLIBC__ > 2) || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 10)) && (_POSIX_C_SOURCE >= 200809L) \
|| defined(_GNU_SOURCE)
#	define JSTR_HAS_STPCPY
#	define JSTR_HAS_STRNLEN
#endif /* Posix || Gnu */

#define JSTR_RST JSTR_RESTRICT

#endif /* JSTR_MACROS_H_DEF */
