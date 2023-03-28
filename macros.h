#ifndef JSTR_MACROS_H_DEF
#define JSTR_MACROS_H_DEF

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

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	define RESTRICT restrict
#elif defined(__GNUC__) || defined(__clang__)
#	define RESTRICT __restrict__
#elif defined(_MSC_VER)
#	define RESTRICT __restrict
#else
#	define RESTRICT
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
#	define ALWAYS_INLINE __attribute__((always_inline)) inline
#	if __has_attribute(pure)
#		define PURE __attribute__((pure))
#	else
#		define PURE
#	endif // PURE
#	if __has_attribute(const)
#		define CONST __attribute__((const))
#	else
#		define CONST
#	endif // CONST
#	if __has_attribute(flatten)
#		define FLATTEN __attribute__((flatten))
#	else
#		define FLATTEN
#	endif // FLATTEN
#elif defined(_MSC_VER)
#	define ALWAYS_INLINE __forceinline inline
#	define PURE __declspec(noalias)
#	define CONST __declspec(restrict)
#	define FLATTEN
#else
#	define ALWAYS_INLINE inline
#	define PURE
#	define CONST
#	define FLATTEN
#endif // __GNUC__ || __clang__ || _MSC_VER

#if defined(__GNUC__) || defined(__clang__)
#	include <stdint.h>
#	if __has_builtin(__builtin_clzll)
		CONST ALWAYS_INLINE uint64_t private_jstr_next_pow2_64(uint64_t x)
		{
			return 1ull << (64 - __builtin_clzll(x - 1));
		}
#	endif // __has_builtin(__builtin_clzll)
#	if __has_builtin(__builtin_clz)
		CONST ALWAYS_INLINE uint32_t private_jstr_next_pow2_32(uint32_t x)
		{
			return 1 << (32 - __builtin_clz(x - 1));
		}
#	endif // __has_builtin(__builtin_clz)
#elif defined(_MSC_VER)
#	include <stdint.h>
#	include <intrin.h>
#	pragma intrinsic(_BitScanReverse64)
	CONST ALWAYS_INLINE uint32_t private_jstr_next_pow2_32(uint32_t x)
	{
		unsigned long index;
		_BitScanReverse(&index, x - 1);
		return 1 << (index + 1);
	}

	CONST ALWAYS_INLINE uint64_t private_jstr_next_pow2_64(uint64_t x)
	{
		unsigned long index;
		_BitScanReverse64(&index, x - 1);
		return 1ull << (index + 1);
	}
#else
	CONST ALWAYS_INLINE size_t private_jstr_next_pow2_32(size_t x)
	{
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return x + 1;
	}

	CONST ALWAYS_INLINE size_t private_jstr_next_pow2_64(size_t x)
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
#endif // __GNUC__ || __clang__

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
#		define JSTR_GENERIC_CASE_SIZE(expr)        \
			int: expr,                         \
			unsigned int: expr,                \
			size_t: expr,                      \
			long: expr,                        \
			long long: expr,                   \
			unsigned long long: expr,          \
						           \
			const int: expr,                   \
			const unsigned int: expr,          \
			const size_t: expr,                \
			const long: expr,                  \
			const long long: expr,             \
			const unsigned long long: expr

#		define JSTR_GENERIC_CASE_STR(expr) \
			char *: expr,              \
			const char *: expr

#		define JSTR_GENERIC_CASE_CHAR(expr) \
			char: expr,                 \
			const char: expr

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

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

#endif // JSTR_MACROS_H_DEF
