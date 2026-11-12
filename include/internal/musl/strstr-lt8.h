
#ifndef JSTR_INTERNAL_STRSTR234_FUNC_NAME
#	define JSTR_INTERNAL_STRSTR234_FUNC_NAME jstr_internal_strstr
#endif
#ifndef JSTR_INTERNAL_STRSTR234_CANON
#	define JSTR_INTERNAL_STRSTR234_CANON(x) (x)
#endif
#define L (unsigned)JSTR_INTERNAL_STRSTR234_CANON
#ifdef JSTR_INTERNAL_STRSTR234_STRNSTR
#	define N_PARAM , size_t l
#	define N       l-- &&
#	define N_EXIT                       \
		if (jstr_unlikely(l-- == 0)) \
		return NULL
#else
#	define N_PARAM
#	define N
#	define N_EXIT
#endif

#include "../../macros.h"

#ifdef JSTR_INTERNAL_STRSTR234_MEMMEM

/**
 * @brief Search for a needle of fewer than 8 bytes in a haystack using a packed 64-bit word comparison (internal helper).
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param hs Pointer parameter; must be non-NULL (caller guarantees) unless length is 0.
 * @param hs_len Byte/element count; may be 0.
 * @param ne Pointer parameter; must be non-NULL (caller guarantees) unless length is 0.
 * @param ne_len Byte/element count; may be 0.
 * @return Pointer to the first occurrence of @p ne in @p hs, or NULL if not found.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(JSTR_INTERNAL_STRSTR234_FUNC_NAME, _lt8)(const unsigned char *hs, size_t hs_len, const unsigned char *ne, unsigned int ne_len)
JSTR_NOEXCEPT
{
	uint64_t hw = 0, nw = 0;
	const uint64_t mask = (uint64_t)-1 >> ((sizeof(uint64_t) - ne_len) << 3);
	for (unsigned int n = ne_len; n--; hw = (uint64_t)hw << 8 | L(*hs++), nw = (uint64_t)nw << 8 | L(*ne++))
		N_EXIT;
	for (hs_len -= ne_len, hw &= mask, nw &= mask; N hw != nw && hs_len--; hw = (uint64_t)(hw << 8 | L(*hs++)) & mask) {}
	return hw == nw ? (char *)(hs - ne_len) : NULL;
}

/**
 * @brief Search for a 2-byte needle in a haystack using packed 16-bit word comparison (internal helper).
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param h Pointer parameter; must be non-NULL (caller guarantees) unless length is 0.
 * @param k Byte/element count; may be 0.
 * @param n Byte/element count; may be 0.
 * @return Pointer to the first occurrence of @p ne in @p hs, or NULL if not found.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(JSTR_INTERNAL_STRSTR234_FUNC_NAME, 2)(const unsigned char *h, size_t k, const unsigned char *n)
JSTR_NOEXCEPT
{
	const uint16_t nw = L(n[0]) << 8 | L(n[1]);
	uint16_t hw = L(h[0]) << 8 | L(h[1]);
	for (h += 2, k -= 2; N k-- && hw != nw; hw = hw << 8 | L(*h++))
		if (hw == nw)
			return (char *)h - 2;
	return hw == nw ? (char *)h - 2 : 0;
}

#elif JSTR_INTERNAL_STRSTR234_MEMRMEM

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
/**
 * @brief Search for a needle of fewer than 8 bytes in a haystack using a packed 64-bit word comparison (internal helper).
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param hs Pointer parameter; must be non-NULL (caller guarantees) unless length is 0.
 * @param hs_len Byte/element count; may be 0.
 * @param ne Pointer parameter; must be non-NULL (caller guarantees) unless length is 0.
 * @param ne_len Byte/element count; may be 0.
 * @return Pointer to the matching occurrence, or NULL if not found.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(JSTR_INTERNAL_STRSTR234_FUNC_NAME, _lt8)(const unsigned char *hs, size_t hs_len, const unsigned char *ne, unsigned int ne_len)
JSTR_NOEXCEPT
{
	uint64_t hw = 0, nw = 0;
	const uint64_t mask = (uint64_t)-1 >> ((sizeof(uint64_t) - ne_len) << 3);
	hs += hs_len - 1;
	ne += ne_len - 1;
	for (unsigned int n = ne_len; n--; hw = (uint64_t)hw << 8 | L(*hs--), nw = (uint64_t)nw << 8 | L(*ne--))
		N_EXIT;
	for (hs_len -= ne_len, hw &= mask, nw &= mask; N hw != nw && hs_len--; hw = (uint64_t)(hw << 8 | L(*hs--)) & mask) {}
	return hw == nw ? (char *)(hs + 1) : NULL;
}

/**
 * @brief Search for a needle of 8 or more bytes in a haystack, comparing the tail then the head (internal helper).
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param hs Pointer parameter; must be non-NULL (caller guarantees) unless length is 0.
 * @param hs_len Byte/element count; may be 0.
 * @param ne Pointer parameter; must be non-NULL (caller guarantees) unless length is 0.
 * @param ne_len Byte/element count; may be 0.
 * @return Pointer to the matching occurrence, or NULL if not found.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_FUNC_PURE
static void *
JSTR_CONCAT(JSTR_INTERNAL_STRSTR234_FUNC_NAME, _8more)(const unsigned char *hs, unsigned int hs_len, const unsigned char *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	hs += hs_len - ne_len;
	const U64 nw = (U64)L(ne[7]) << 56 | (U64)L(ne[6]) << 48
	               | (U64)L(ne[5]) << 40 | (U64)L(ne[4]) << 32
	               | (U64)L(ne[3]) << 24 | (U64)L(ne[2]) << 16
	               | (U64)L(ne[1]) << 8 | (U64)L(ne[0]);
	U64 hw = (U64)L(hs[7]) << 56 | (U64)L(hs[6]) << 48 | (U64)L(hs[5]) << 40
	         | (U64)L(hs[4]) << 32 | (U64)L(hs[3]) << 24
	         | (U64)L(hs[2]) << 16 | (U64)L(hs[1]) << 8 | (U64)L(hs[0]);
	for (hs_len -= ne_len; hs_len--; hw = hw << 8 | L(*--hs))
		if (hw == nw && !memcmp(hs + 8, ne + 8, ne_len - 8))
			return (void *)hs;
	return (hw == nw && !memcmp(hs + 8, ne + 8, ne_len - 8)) ? (void *)hs
	                                                         : NULL;
}

#else /* STRSTR */

JSTR_ATTR_ACCESS((__read_only__, 2, 3))
/**
 * @brief Search for a needle of fewer than 8 bytes in a haystack using a packed 64-bit word comparison (internal helper).
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param hs Pointer parameter; must be non-NULL (caller guarantees) unless length is 0.
 * @param ne Pointer parameter; must be non-NULL (caller guarantees) unless length is 0.
 * @param ne_len Byte/element count; may be 0.
 * @return Pointer to the first occurrence of @p ne in @p hs, or NULL if not found.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(JSTR_INTERNAL_STRSTR234_FUNC_NAME, _lt8)(const unsigned char *hs, const unsigned char *ne, unsigned int ne_len)
JSTR_NOEXCEPT
{
	uint64_t hw = 0, nw = 0;
	const uint64_t mask = (uint64_t)-1 >> ((sizeof(uint64_t) - ne_len) << 3);
	for (; *ne; hw = (uint64_t)hw << 8 | L(*hs++), nw = (uint64_t)nw << 8 | L(*ne++)) {
		N_EXIT;
		if (jstr_unlikely(*hs == '\0'))
			return NULL;
	}
	for (hw &= mask, nw &= mask; N hw != nw && *hs; hw = (uint64_t)(hw << 8 | L(*hs++)) & mask) {}
	return hw == nw ? (char *)(hs - ne_len) : NULL;
}

/**
 * @brief Search for a 2-byte needle in a haystack using packed 16-bit word comparison (internal helper).
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param h Pointer parameter; must be non-NULL (caller guarantees) unless length is 0.
 * @param n Byte/element count; may be 0.
 * @return Pointer to the first occurrence of @p ne in @p hs, or NULL if not found.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(JSTR_INTERNAL_STRSTR234_FUNC_NAME, 2)(const unsigned char *h, const unsigned char *n)
JSTR_NOEXCEPT
{
	const uint16_t nw = L(n[0]) << 8 | L(n[1]);
	uint16_t hw = L(h[0]) << 8 | L(h[1]);
	for (h++; N * h && hw != nw; hw = hw << 8 | L(*++h)) {}
	return *h ? (char *)h - 1 : 0;
}

#endif

#undef L
#undef N
#undef N_EXIT
#undef N_PARAM
#undef JSTR_INTERNAL_STRSTR234_FUNC_NAME
#undef JSTR_INTERNAL_STRSTR234_CANON
#undef JSTR_INTERNAL_STRSTR234_MEMMEM
#undef JSTR_INTERNAL_STRSTR234_STRNSTR
