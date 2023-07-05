#ifndef JSTD_MACROS_H_DEF
#define JSTD_MACROS_H_DEF

#ifdef __cplusplus
#	define JSTD_NOEXCEPT noexcept
#else
#	define JSTD_NOEXCEPT
#endif

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) || defined(__GNUC__) && (__GNUC__ >= 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
#	define JSTD_WARN_UNUSED __attribute__((warn_unused_result))
#elif defined(__attribute_warn_unused_result__)
#	define JSTD_WARN_UNUSED __attribute_warn_unused_result__
#else
#	define JSTD_WARN_UNUSED
#endif // __attribute__unused

#if (defined(__GNUC__) && (__GNUC__ >= 4)) || (defined(__clang__) && (__clang_major__ >= 3))
#	define JSTD_HAS_TYPEOF
#endif // JSTD_HAS_TYPEOF

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#	define JSTD_HAS_GENERIC
#endif // JSTD_HAS_GENERIC

#if defined(static_assert)
#	define JSTD_HAS_STATIC_ASSERT
#	define JSTD_ASSERT(expr, msg) static_assert(expr, msg)
#	define JSTD_ASSERT_SEMICOLON(expr, msg) static_assert(expr, msg);
#elif __STDC_VERSION__ >= 201112L
#	define JSTD_HAS_STATIC_ASSERT
#	define JSTD_ASSERT(expr, msg) _Static_assert(expr, msg)
#else
#	define JSTD_ASSERT(expr, msg)
#	define JSTD_ASSERT_SEMICOLON(expr, msg)
#endif // static_assert

#ifdef JSTD_ALIGN_POWER_OF_TWO
#	ifdef JSTD_64_BIT
#		ifdef __cplusplus
#			define JSTD_NEXT_POW2(x) JSTD_NEXT_POW2_64(x)
#		else
#			define JSTD_NEXT_POW2(x) private_jstr_next_pow2_64(x)
#		endif // __cplusplus
#	elif JSTD_32_BIT
#		ifdef __cplusplus
#			define JSTD_NEXT_POW2(x) JSTD_NEXT_POW2_32(x)
#		else
#			define JSTD_NEXT_POW2(x) private_jstr_next_pow2_32(x)
#		endif // __cplusplus
#	else
#		define JSTD_NEXT_POW2(x) (x)
#	endif // JSTD_64_BIT
#endif // JSTD_ALIGN_POWER_OF_TWO

#ifdef __cplusplus
#	define JSTD_RESTRICT
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	define JSTD_RESTRICT restrict
#elif defined(__GNUC__) || defined(__clang__)
#	define JSTD_RESTRICT __restrict__
#elif defined(_MSC_VER)
#	define JSTD_RESTRICT __restrict
#else
#	define JSTD_RESTRICT
#endif // restrict

#if defined(__PRETTY_FUNCTION__)
#	define CURR_FUNC __PRETTY_FUNCTION__
#elif defined(__FUNCTION__)
#	define CURR_FUNC __FUNCTION__
#elif defined(__func__)
#	define CURR_FUNC __func__
#else
#	define CURR_FUNC ""
#endif // __func__

#if (defined(__GNUC__) && (__GNUC__ >= 3)) || (defined(__clang__) && __has_builtin(__builtin_expect))
#	define likely(x) __builtin_expect(!!(x), 1)
#	define unlikely(x) __builtin_expect(!!(x), 0)
#else
#	define likely(x) (x)
#	define unlikely(x) (x)
#endif // __has_builtin(__builtin_expect)

#if defined(__GNUC__) || defined(__clang__)
#	define JSTD_INLINE __attribute__((always_inline)) inline
#	if __has_attribute(pure)
#		define PURE __attribute__((pure))
#	else
#		define PURE
#	endif // PURE
#	if __has_attribute(const)
#		define JSTD_CONST __attribute__((const))
#	else
#		define JSTD_CONST
#	endif // JSTD_CONST
#	if __has_attribute(flatten)
#		define FLATTEN __attribute__((flatten))
#	else
#		define FLATTEN
#	endif // FLATTEN
#elif defined(_MSC_VER)
#	define JSTD_INLINE __forceinline inline
#	define PURE __declspec(noalias)
#	define JSTD_CONST __declspec(restrict)
#	define FLATTEN
#else
#	define JSTD_INLINE inline
#	define PURE
#	define JSTD_CONST
#	define FLATTEN
#endif // __GNUC__ || __clang__ || _MSC_VER

#if defined(__GNUC__) || defined(__clang__)
#	ifdef __cplusplus
#		include <cstdint>
#	else
#		include <stdint.h>
#	endif // __cplusplus
#	if __has_builtin(__builtin_clzll)
#	ifdef __cplusplus
extern "C" {
#	endif

#	ifdef JSTD_64_BIT
		JSTD_INLINE
		JSTD_CONST
		JSTD_WARN_UNUSED
		uint64_t private_jstr_next_pow2_64(uint64_t x) JSTD_NOEXCEPT
		{
			return 1ull << (64 - __builtin_clzll(x - 1));
		}

#	endif // JSTD_64_BIT

#	endif // __has_builtin(__builtin_clzll)
#	if __has_builtin(__builtin_clz)

#	ifdef JSTD_32_BIT
		JSTD_INLINE
		JSTD_CONST
		JSTD_WARN_UNUSED
		uint32_t private_jstr_next_pow2_32(uint32_t x) JSTD_NOEXCEPT
		{
			return 1 << (32 - __builtin_clz(x - 1));
		}

#	endif // JSTD_32_BIT

#	ifdef __cplusplus
}
#	endif
#	endif // __has_builtin(__builtin_clz)
#elif defined(_MSC_VER)
#	include <stdint.h>
#	include <intrin.h>
#	pragma intrinsic(_BitScanReverse64)
#	ifdef __cplusplus
		extern "C" {
#	endif

#	ifdef JSTD_32_BIT
	JSTD_INLINE
	JSTD_CONST
	JSTD_WARN_UNUSED
	uint32_t private_jstr_next_pow2_32(uint32_t x) JSTD_NOEXCEPT
	{
		unsigned long index;
		_BitScanReverse(&index, x - 1);
		return 1 << (index + 1);
	}

#	endif // JSTD_32_BIT

#	ifdef JSTD_64_BIT
	JSTD_INLINE
	JSTD_CONST
	JSTD_WARN_UNUSED
	uint64_t private_jstr_next_pow2_64(uint64_t x) JSTD_NOEXCEPT
	{
		unsigned long index;
		_BitScanReverse64(&index, x - 1);
		return 1ull << (index + 1);
	}

#	endif // JSTD_64_BIT

#else

#	ifdef JSTD_32_BIT

	JSTD_INLINE
	JSTD_CONST
	JSTD_WARN_UNUSED
	size_t private_jstr_next_pow2_32(size_t x) JSTD_NOEXCEPT
	{
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return x + 1;
	}

#endif // JSTD_32_BIT

#	ifdef JSTD_64_BIT

	JSTD_INLINE
	JSTD_CONST
	JSTD_WARN_UNUSED
	size_t private_jstr_next_pow2_64(size_t x) JSTD_NOEXCEPT
	{
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		x |= x >> 32;
		return x + 1;
	}

#endif // JSTD_64_BIT

#	ifdef __cplusplus
		}
#	endif
#endif // __GNUC__ || __clang__

#ifdef __cplusplus

#include <type_traits>

	JSTD_INLINE
	JSTD_CONST
	JSTD_WARN_UNUSED
	static constexpr std::size_t private_jstr_next_pow2_32_constexpr(std::size_t x) JSTD_NOEXCEPT
	{
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return x + 1;
	}

	JSTD_INLINE
	JSTD_CONST
	JSTD_WARN_UNUSED
	static constexpr std::size_t private_jstr_next_pow2_64_constexpr(std::size_t x) JSTD_NOEXCEPT
	{
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		x |= x >> 32;
		return x + 1;
	}

	template <typename T>
	JSTD_INLINE
	JSTD_WARN_UNUSED
	JSTD_CONST
	static std::size_t JSTD_NEXT_POW2_32(T x)
	{
		if 
#if __cplusplus > 201703L
		constexpr
#endif // cpp 17
			(std::is_integral<T>::value)
			return private_jstr_next_pow2_32_constexpr(x);
		return private_jstr_next_pow2_32(x);
	}

	template <typename T>
	JSTD_INLINE
	JSTD_WARN_UNUSED
	JSTD_CONST
	static std::size_t JSTD_NEXT_POW2_64(T x)
	{
		if 
#if __cplusplus > 201703L
		constexpr
#endif // cpp 17
			(std::is_integral<T>::value)
			return private_jstr_next_pow2_64_constexpr(x);
		return private_jstr_next_pow2_64(x);
	}

#endif // __cplusplus

#if defined(JSTD_HAS_TYPEOF) && defined(JSTD_HAS_GENERIC)
#	define JSTD_SAME_TYPE(x, y) _Generic((x), \
		typeof(y): 1,                     \
		default: 0                        \
	)
#	define JSTD_IS_TYPE(T, x) _Generic((x), \
		T: 1,                           \
		default: 0                      \
	)
#endif // JSTD_HAS_TYPEOF && JSTD_HAS_GENERIC

#if defined(__GNUC__) || defined(__clang__)
#	ifdef JSTD_HAS_GENERIC
#		define JSTD_GENERIC_CASE_SIZE(expr) \
			int: expr,                  \
			unsigned int: expr,         \
			size_t: expr,               \
			long: expr,                 \
			long long: expr,            \
			unsigned long long: expr    \

#		define JSTD_GENERIC_CASE_STR(expr) \
			char *: expr,              \
			const char *: expr         \

#		define JSTD_GENERIC_CASE_CHAR(expr) \
			char: expr,                 \
			const char: expr            \

#		define JSTD_IS_SIZE(expr) _Generic((expr), \
			JSTD_GENERIC_CASE_SIZE(1),         \
			default: 0)

#		define JSTD_IS_STR(expr) _Generic((expr), \
			JSTD_GENERIC_CASE_STR(1),         \
					default: 0)

#		define JSTD_IS_CHAR(expr) _Generic((expr), \
			JSTD_GENERIC_CASE_CHAR(1),         \
					default: 0)

#		define JSTD_ASSERT_IS_SIZE(expr)                                                   \
			JSTD_ASSERT(JSTD_IS_SIZE(expr), "Passing non-number as number argument!");
#		define JSTD_ASSERT_IS_STR(expr)                                                   \
			JSTD_ASSERT(JSTD_IS_STR(expr), "Passing non-string as string argument!");
#		define JSTD_ASSERT_IS_CHAR(expr)                                               \
			JSTD_ASSERT(JSTD_IS_CHAR(expr), "Passing non-char as char argument!");
#		define JSTD_ASSERT_TYPECHECK(Texpr, expr) JSTD_ASSERT(JSTD_SAME_TYPE(Texpr, expr), "Passing the wrong data type!");
#	else
#		define JSTD_ASSERT_IS_SIZE(expr)
#		define JSTD_ASSERT_IS_STR(expr)
#		define JSTD_ASSERT_IS_CHAR(expr)
#		define JSTD_ASSERT_TYPECHECK(Texpr, expr)
#	endif // JSTD_HAS_GENERIC
#	define JSTD_MACRO_START ({
#	define JSTD_MACRO_END ;})
#else
#	define JSTD_IS_SIZE(val)
#	define JSTD_MACRO_START (
#	define JSTD_MACRO_END )
#endif // __GNUC__ || __clang__

 #ifndef MAX
#	define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif // MAX

#ifndef MIN
#	define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif // MIN


#define JSTD_CASE_VOWEL_LOWER case 'a': case 'i': case 'u': case 'e': case 'o':
#define JSTD_CASE_VOWEL_UPPER case 'A': case 'I': case 'U': case 'E': case 'O':
#define JSTD_CASE_VOWEL JSTD_CASE_VOWEL_UPPER: JSTD_CASE_VOWEL_LOWER

#define JSTD_CASE_CONSONANT_LOWER case 'b': case 'c': case 'd': case 'f': case 'g': case 'h': case 'j': case 'k': case 'l': case 'm': case 'n': case 'p': case 'q': case 'r': case 's': case 't': case 'v': case 'w': case 'x': case 'y': case 'z':
#define JSTD_CASE_CONSONANT_UPPER case 'B': case 'C': case 'D': case 'F': case 'G': case 'H': case 'J': case 'K': case 'L': case 'M': case 'N': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'V': case 'W': case 'X': case 'Y': case 'Z':
#define JSTD_CASE_CONSONANT JSTD_CASE_CONSONANT_UPPER: JSTD_CASE_CONSONANT_LOWER

#define JSTD_CASE_DIGIT case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
#define JSTD_CASE_LOWER case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
#define JSTD_CASE_UPPER case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':

#define JSTD_CASE_ALPHA JSTD_CASE_LOWER JSTD_CASE_UPPER
#define JSTD_CASE_ALPHANUM JSTD_CASE_DIGIT JSTD_CASE_ALPHA

#define JSTD_CASE_WHITESPACE case '\n': case '\t': case '\r': case ' ':
	 
#ifdef __cplusplus
#	define JSTD_PRIVATE private:
#	define JSTD_PUBLIC public:
#	define JSTD_CAST(T) (T)
#	define JSTD_CPP_CONST const
#else
#	define JSTD_CAST(T)
#	define JSTD_PRIVATE
#	define JSTD_PUBLIC
#	define JSTD_CPP_CONST
#endif // __cplusplus

#if defined(_GNU_SOURCE)
#	define JSTD_HAS_MEMMEM
#	define JSTD_HAS_MEMRCHR
#endif // JSTD_HAS_MEMRCHR__

#if __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 19
#	if _DEFAULT_SOURCE
#		define JSTD_HAS_FREAD_UNLOCKED
#		define JSTD_HAS_FWRITE_UNLOCKED
#		define JSTD_HAS_FPUTC_UNLOCKED
#		define JSTD_HAS_FGETC_UNLOCKED
#	elif defined(_SVID_SOURCE) || defined(_BSD_SOURCE)
#		define JSTD_HAS_FREAD_UNLOCKED
#		define JSTD_HAS_FWRITE_UNLOCKED
#		define JSTD_HAS_FPUTC_UNLOCKED
#		define JSTD_HAS_FGETC_UNLOCKED
#	endif /* _DEFAULT_SOURCE */
#endif /* */
	 
#ifdef __GLIBC__
#	if (((__GLIBC__ > 2) || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 1)) && (_POSIX_C_SOURCE >= 200809L) || defined(_GNU_SOURCE))
#		define JSTD_HAS_STPCPY
#	endif
#endif // __GLIBC__

#endif // JSTD_MACROS_H_DEF
