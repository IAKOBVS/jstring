#ifndef JSTR_MACROS_H_DEF__
#define JSTR_MACROS_H_DEF__

#ifdef __cplusplus
#	define JSTR_NOEXCEPT__ noexcept
#else
#	define JSTR_NOEXCEPT__
#endif

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) || defined(__GNUC__) && (__GNUC__ >= 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
#	define JSTR_WARN_UNUSED__ __attribute__((warn_unused_result))
#elif defined(__attribute_warn_unused_result__)
#	define JSTR_WARN_UNUSED__ __attribute_warn_unused_result__
#else
#	define JSTR_WARN_UNUSED__
#endif // __attribute__unused

#if (defined(__GNUC__) && (__GNUC__ >= 4)) || (defined(__clang__) && (__clang_major__ >= 3))
#	define JSTR_HAS_TYPEOF
#endif // JSTR_HAS_TYPEOF

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#	define JSTR_HAS_GENERIC
#endif // JSTR_HAS_GENERIC

#if defined(static_assert)
#	define JSTR_HAS_STATIC_ASSERT
#elif __STDC_VERSION__ >= 201112L
#	define JSTR_HAS__STATIC_ASSERT_
#endif // static_assert

#ifdef JSTR_HAS_STATIC_ASSERT
#	define JSTR_ASSERT(expr, msg) static_assert(expr, msg)
#elif defined(JSTR_HAS__STATIC_ASSERT_)
#	define JSTR_ASSERT(expr, msg) _Static_assert(expr, msg)
#endif // JSTR_HAS_STATIC_ASSERT

#ifdef JSTR_ALIGN_POWER_OF_TWO
#	ifdef JSTR_64_BIT
#		define JSTR_NEXT_POW2(x) private_jstr_next_pow2_64(x)
#	elif JSTR_32_BIT
#		define JSTR_NEXT_POW2(x) private_jstr_next_pow2_32(x)
#	else
#		define JSTR_NEXT_POW2(x) (x)
#	endif // JSTR_64_BIT
#endif // JSTR_ALIGN_POWER_OF_TWO

#ifdef __cplusplus
#	define JSTR_RESTRICT__
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	define JSTR_RESTRICT__ restrict
#elif defined(__GNUC__) || defined(__clang__)
#	define JSTR_RESTRICT__ __restrict__
#elif defined(_MSC_VER)
#	define JSTR_RESTRICT__ __restrict
#else
#	define JSTR_RESTRICT__
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
#	define JSTR_INLINE__ __attribute__((always_inline)) inline
#	if __has_attribute(pure)
#		define PURE __attribute__((pure))
#	else
#		define PURE
#	endif // PURE
#	if __has_attribute(const)
#		define JSTR_CONST__ __attribute__((const))
#	else
#		define JSTR_CONST__
#	endif // JSTR_CONST__
#	if __has_attribute(flatten)
#		define FLATTEN __attribute__((flatten))
#	else
#		define FLATTEN
#	endif // FLATTEN
#elif defined(_MSC_VER)
#	define JSTR_INLINE__ __forceinline inline
#	define PURE __declspec(noalias)
#	define JSTR_CONST__ __declspec(restrict)
#	define FLATTEN
#else
#	define JSTR_INLINE__ inline
#	define PURE
#	define JSTR_CONST__
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
		JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	uint64_t private_jstr_next_pow2_64(uint64_t x) JSTR_NOEXCEPT__
		{
			return 1ull << (64 - __builtin_clzll(x - 1));
		}
#	endif // __has_builtin(__builtin_clzll)
#	if __has_builtin(__builtin_clz)
		JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	uint32_t private_jstr_next_pow2_32(uint32_t x) JSTR_NOEXCEPT__
		{
			return 1 << (32 - __builtin_clz(x - 1));
		}
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
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	uint32_t private_jstr_next_pow2_32(uint32_t x) JSTR_NOEXCEPT__
	{
		unsigned long index;
		_BitScanReverse(&index, x - 1);
		return 1 << (index + 1);
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	uint64_t private_jstr_next_pow2_64(uint64_t x) JSTR_NOEXCEPT__
	{
		unsigned long index;
		_BitScanReverse64(&index, x - 1);
		return 1ull << (index + 1);
	}
#else
	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	size_t private_jstr_next_pow2_32(size_t x) JSTR_NOEXCEPT__
	{
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return x + 1;
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	size_t private_jstr_next_pow2_64(size_t x) JSTR_NOEXCEPT__
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
#	ifdef __cplusplus
		}
#	endif
#endif // __GNUC__ || __clang__

#ifdef __cplusplus

#include <type_traits>

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static constexpr std::size_t private_jstr_next_pow2_32_constexpr(std::size_t x) JSTR_NOEXCEPT__
	{
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return x + 1;
	}

	JSTR_INLINE__
	JSTR_CONST__
	JSTR_WARN_UNUSED__
	static constexpr std::size_t private_jstr_next_pow2_64_constexpr(std::size_t x) JSTR_NOEXCEPT__
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
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	JSTR_CONST__
	static std::size_t JSTR_NEXT_POW2_32(T x)
	{
		if constexpr (std::is_integral<T>::value)
			return private_jstr_next_pow2_32_constexpr(x);
		return private_jstr_next_pow2_32(x);
	}

	template <typename T>
	JSTR_INLINE__
	JSTR_WARN_UNUSED__
	JSTR_CONST__
	static std::size_t JSTR_NEXT_POW2_64(T x)
	{
		if constexpr (std::is_integral<T>::value)
			return private_jstr_next_pow2_64_constexpr(x);
		return private_jstr_next_pow2_64(x);
	}

#endif // __cplusplus

#if defined(JSTR_HAS_TYPEOF) && defined(JSTR_HAS_GENERIC)
#	define JSTR_SAME_TYPE(x, y) _Generic((x), \
		typeof(y): 1,                     \
		default: 0                        \
	)
#	define JSTR_IS_TYPE(T, x) _Generic((x), \
		T: 1,                           \
		default: 0                      \
	)
#endif // JSTR_HAS_TYPEOF && JSTR_HAS_GENERIC

#if defined(__GNUC__) || defined(__clang__)
#	ifdef JSTR_HAS_GENERIC
#		define JSTR_GENERIC_CASE_SIZE(expr) \
			int: expr,                  \
			unsigned int: expr,         \
			size_t: expr,               \
			long: expr,                 \
			long long: expr,            \
			unsigned long long: expr    \

#		define JSTR_GENERIC_CASE_STR(expr) \
			char *: expr,              \
			const char *: expr         \

#		define JSTR_GENERIC_CASE_CHAR(expr) \
			char: expr,                 \
			const char: expr            \

#		define JSTR_IS_SIZE(expr) _Generic((expr), \
			JSTR_GENERIC_CASE_SIZE(1),         \
			default: 0)

#		define JSTR_IS_STR(expr) _Generic((expr), \
			JSTR_GENERIC_CASE_STR(1),         \
					default: 0)

#		define JSTR_IS_CHAR(expr) _Generic((expr), \
			JSTR_GENERIC_CASE_CHAR(1),         \
					default: 0)

#		define JSTR_ASSERT_SIZE(expr)                                                              \
			JSTR_ASSERT(JSTR_IS_SIZE(expr), "Not using a number where a number is required!");
#		define JSTR_ASSERT_STR(expr)                                                              \
			JSTR_ASSERT(JSTR_IS_STR(expr), "Not using a char * where a char * is required!");
#		define JSTR_ASSERT_CHAR(expr)                                                          \
			JSTR_ASSERT(JSTR_IS_CHAR(expr), "Not using a char where a char is required!");
#		define JSTR_ASSERT_TYPECHECK(Texpr, expr) JSTR_ASSERT(JSTR_SAME_TYPE(Texpr, expr), "Passing the wrong data type!");
#	else
#		define JSTR_IS_SIZE(expr)
#		define JSTR_ASSERT_SIZE(expr)
#	endif // JSTR_HAS_GENERIC
#	define JSTR_MACRO_START ({
#	define JSTR_MACRO_END ;})
#else
#	define JSTR_IS_SIZE(val)
#	define JSTR_MACRO_START (
#	define JSTR_MACRO_END )
#endif // __GNUC__ || __clang__

#ifndef MAX
#	define MAX(a,b) ((a)>(b)?(a):(b))
#endif // MAX

#ifndef MIN
#	define MIN(a,b) ((a)<(b)?(a):(b))
#endif // MIN

#define JSTR_CASE_VOWEL_LOWER case 'a': case 'i': case 'u': case 'e': case 'o':
#define JSTR_CASE_VOWEL_UPPER case 'A': case 'I': case 'U': case 'E': case 'O':
#define JSTR_CASE_VOWEL JSTR_CASE_VOWEL_UPPER: JSTR_CASE_VOWEL_LOWER

#define JSTR_CASE_CONSONANT_LOWER case 'b': case 'c': case 'd': case 'f': case 'g': case 'h': case 'j': case 'k': case 'l': case 'm': case 'n': case 'p': case 'q': case 'r': case 's': case 't': case 'v': case 'w': case 'x': case 'y': case 'z':
#define JSTR_CASE_CONSONANT_UPPER case 'B': case 'C': case 'D': case 'F': case 'G': case 'H': case 'J': case 'K': case 'L': case 'M': case 'N': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'V': case 'W': case 'X': case 'Y': case 'Z':
#define JSTR_CASE_CONSONANT JSTR_CASE_CONSONANT_UPPER: JSTR_CASE_CONSONANT_LOWER

#define JSTR_CASE_DIGIT case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
#define JSTR_CASE_LOWER case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
#define JSTR_CASE_UPPER case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':

#define JSTR_CASE_ALPHA JSTR_CASE_LOWER JSTR_CASE_UPPER
#define JSTR_CASE_ALPHANUM JSTR_CASE_DIGIT JSTR_CASE_ALPHA

#define JSTR_CASE_WHITESPACE case '\n': case '\t': case '\r': case ' ':
	 
#ifdef __cplusplus
#	define JSTR_PRIVATE__ private:
#	define JSTR_PUBLIC__ public:
#	define JSTR_CAST__(T) (T)
#	define JSTR_CPP_CONST__ const
#else
#	define JSTR_CAST__(T)
#	define JSTR_PRIVATE__
#	define JSTR_PUBLIC__
#	define JSTR_CPP_CONST__
#endif // __cplusplus

#if defined(__USE_GNU) || (_XOPEN_SOURCE >= 700) || (_POSIX_C_SOURCE >= 200890L) || defined(__USE_BSD)
#	define JSTR_HAS_MEMMEM__
#endif // JSTR_HAS_MEMMEM__

#if defined(__USE_GNU) || (_XOPEN_SOURCE >= 700) || (_POSIX_C_SOURCE >= 200890L) || defined(__USE_BSD) || __GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 26)
#	define JSTR_HAS_MEMRCHR__
#endif // JSTR_HAS_MEMRCHR__

#endif // JSTR_MACROS_H_DEF__
