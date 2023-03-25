#ifndef JSTR_MACROS_H_DEF
#define JSTR_MACROS_H_DEF

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
#	define PURE
#	define CONST
#	define FLATTEN
#endif // __GNUC__ || __clang__ || _MSC_VER

#if defined(__GNUC__) || defined(__clang__)
#	include <stdint.h>
#	if __has_builtin(__builtin_clzll)
		CONST ALWAYS_INLINE static uint64_t private_jstr_next_pow2_64(uint64_t x)
		{
			return 1ull << (64 - __builtin_clzll(x - 1));
		}
#	endif // __has_builtin(__builtin_clzll)
#	if __has_builtin(__builtin_clz)
		CONST ALWAYS_INLINE static uint32_t private_jstr_next_pow2_32(uint32_t x)
		{
			return 1 << (32 - __builtin_clz(x - 1));
		}
#	endif // __has_builtin(__builtin_clz)
#elif defined(_MSC_VER)
#	include <stdint.h>
#	include <intrin.h>
#	pragma intrinsic(_BitScanReverse64)
	CONST ALWAYS_INLINE static uint32_t private_jstr_next_pow2_32(uint32_t x)
	{
		unsigned long index;
		_BitScanReverse(&index, x - 1);
		return 1 << (index + 1);
	}

	CONST ALWAYS_INLINE static uint64_t private_jstr_next_pow2_64(uint64_t x)
	{
		unsigned long index;
		_BitScanReverse64(&index, x - 1);
		return 1ull << (index + 1);
	}
#else
	CONST ALWAYS_INLINE static size_t private_jstr_next_pow2_32(size_t x)
	{
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return x + 1;
	}

	CONST ALWAYS_INLINE static size_t private_jstr_next_pow2_64(size_t x)
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

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

#endif // JSTR_MACROS_H_DEF
