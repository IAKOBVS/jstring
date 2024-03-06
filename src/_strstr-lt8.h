/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2024 James Tirta Halim <tirtajames45 at gmail dot com>
 * This file is part of the jstring library.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#ifndef PJSTR_STRSTR234_FUNC_NAME
#	define PJSTR_STRSTR234_FUNC_NAME pjstr_strstr
#endif
#ifndef PJSTR_STRSTR234_CANON
#	define PJSTR_STRSTR234_CANON(x) (x)
#endif
#define L PJSTR_STRSTR234_CANON
#ifdef PJSTR_STRSTR234_STRNSTR
#	define N_PARAM , unsigned int l
#	define N       l-- &&
#	define N_EXIT                       \
		if (jstr_unlikely(l-- == 0)) \
		return NULL
#else
#	define N_PARAM
#	define N
#	define N_EXIT
#endif

#include "jstr-macros.h"

#ifdef __clang__
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wunknown-warning-option"
#endif
#ifdef __GNUC__
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

#ifdef PJSTR_STRSTR234_MEMMEM

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, _lt8)(const unsigned char *hs, unsigned int hs_len, const unsigned char *ne, unsigned int ne_len)
{
	uint64_t hw;
	uint64_t nw;
	const uint64_t mask = (uint64_t)-1 >> ((sizeof(uint64_t) - ne_len) << 3);
	for (unsigned int n = ne_len; n--; hw = (uint64_t)hw << 8 | L(*hs++), nw = (uint64_t)nw << 8 | L(*ne++))
		N_EXIT;
	for (hs_len -= ne_len, hw &= mask, nw &= mask; N hw != nw && hs_len--; hw = (uint64_t)(hw << 8 | L(*hs++)) & mask) {}
	return hw == nw ? (char *)(hs - ne_len) : NULL;
}

/* TODO: simplify strrstr/memrmem to accept ne_len <= 8 */

#elif PJSTR_STRSTR234_MEMRMEM

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 2)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          unsigned int l)
JSTR_NOEXCEPT
{
	typedef uint32_t U32;
	hs += l - 2;
	const U32 nw = (U32)L(ne[1]) << 16 | (U32)L(ne[0]);
	U32 hw = (U32)L(hs[1]) << 16 | (U32)L(hs[0]);
	for (l -= 2; l-- && hw != nw; hw = hw << 16 | L(*--hs)) {}
	return (hw == nw) ? (void *)hs : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 3)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          unsigned int l)
JSTR_NOEXCEPT
{
	typedef uint32_t U32;
	hs += l - 3;
	const U32 nw = (U32)L(ne[2]) << 24 | (U32)L(ne[1]) << 16 | (U32)L(ne[0]) << 8;
	U32 hw = (U32)L(hs[2]) << 24 | (U32)L(hs[1]) << 16 | (U32)L(hs[0]) << 8;
	for (l -= 3; l-- && hw != nw; hw = (hw | L(*--hs)) << 8) {}
	return (hw == nw) ? (void *)hs : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 4)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          unsigned int l)
JSTR_NOEXCEPT
{
	typedef uint32_t U32;
	hs += l - 4;
	const U32 nw = (U32)L(ne[3]) << 24 | (U32)L(ne[2]) << 16 | (U32)L(ne[1]) << 8 | (U32)L(ne[0]);
	U32 hw = (U32)L(hs[3]) << 24 | (U32)L(hs[2]) << 16 | (U32)L(hs[1]) << 8 | (U32)L(hs[0]);
	for (l -= 4; l-- && hw != nw; hw = hw << 8 | L(*--hs)) {}
	return (hw == nw) ? (void *)hs : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 5)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          unsigned int l)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	hs += l - 5;
	const U64 nw = (U64)L(ne[4]) << 56 | (U64)L(ne[3]) << 48 | (U64)L(ne[2]) << 40 | (U64)L(ne[1]) << 32 | (U64)L(ne[0]) << 24;
	U64 hw = (U64)L(hs[4]) << 56 | (U64)L(hs[3]) << 48 | (U64)L(hs[2]) << 40 | (U64)L(hs[1]) << 32 | (U64)L(hs[0]) << 24;
	for (l -= 5; l-- && hw != nw; hw = hw << 8 | L(*--hs) << 24) {}
	return (hw == nw) ? (void *)hs : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 6)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          unsigned int l)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	hs += l - 6;
	const U64 nw = (U64)L(ne[5]) << 56 | (U64)L(ne[4]) << 48 | (U64)L(ne[3]) << 40 | (U64)L(ne[2]) << 32 | (U64)L(ne[1]) << 24 | (U64)L(ne[0]) << 16;
	U64 hw = (U64)L(hs[5]) << 56 | (U64)L(hs[4]) << 48 | (U64)L(hs[3]) << 40 | (U64)L(hs[2]) << 32 | (U64)L(hs[1]) << 24 | (U64)L(hs[0]) << 16;
	for (l -= 6; l-- && hw != nw; hw = hw << 8 | L(*--hs) << 16) {}
	return (hw == nw) ? (void *)hs : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 7)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          unsigned int l)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	hs += l - 7;
	const U64 nw = (U64)L(ne[6]) << 56 | (U64)L(ne[5]) << 48 | (U64)L(ne[4]) << 40 | (U64)L(ne[3]) << 32 | (U64)L(ne[2]) << 24 | (U64)L(ne[1]) << 16 | (U64)L(ne[0]) << 8;
	U64 hw = (U64)L(hs[6]) << 56 | (U64)L(hs[5]) << 48 | (U64)L(hs[4]) << 40 | (U64)L(hs[3]) << 32 | (U64)L(hs[2]) << 24 | (U64)L(hs[1]) << 16 | (U64)L(hs[0]) << 8;
	for (l -= 7; l-- && hw != nw; hw = (hw | L(*--hs)) << 8) {}
	return (hw == nw) ? (void *)hs : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 8)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          unsigned int l)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	hs += l - 8;
	const U64 nw = (U64)L(ne[7]) << 56 | (U64)L(ne[6]) << 48 | (U64)L(ne[5]) << 40 | (U64)L(ne[4]) << 32 | (U64)L(ne[3]) << 24 | (U64)L(ne[2]) << 16 | (U64)L(ne[1]) << 8 | (U64)L(ne[0]);
	U64 hw = (U64)L(hs[7]) << 56 | (U64)L(hs[6]) << 48 | (U64)L(hs[5]) << 40 | (U64)L(hs[4]) << 32 | (U64)L(hs[3]) << 24 | (U64)L(hs[2]) << 16 | (U64)L(hs[1]) << 8 | (U64)L(hs[0]);
	for (l -= 8; l-- && hw != nw; hw = hw << 8 | L(*--hs)) {}
	return (hw == nw) ? (void *)hs : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 8more)(const unsigned char *hs,
                                              const unsigned char *const ne,
                                              unsigned int l,
                                              unsigned int ne_len)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	hs += l - ne_len;
	const U64 nw = (U64)L(ne[7]) << 56 | (U64)L(ne[6]) << 48 | (U64)L(ne[5]) << 40 | (U64)L(ne[4]) << 32 | (U64)L(ne[3]) << 24 | (U64)L(ne[2]) << 16 | (U64)L(ne[1]) << 8 | (U64)L(ne[0]);
	U64 hw = (U64)L(hs[7]) << 56 | (U64)L(hs[6]) << 48 | (U64)L(hs[5]) << 40 | (U64)L(hs[4]) << 32 | (U64)L(hs[3]) << 24 | (U64)L(hs[2]) << 16 | (U64)L(hs[1]) << 8 | (U64)L(hs[0]);
	for (l -= ne_len; l--; hw = hw << 8 | L(*--hs))
		if (hw == nw && !memcmp(hs + 8, ne + 8, ne_len - 8))
			return (void *)hs;
	return (hw == nw && !memcmp(hs + 8, ne + 8, ne_len - 8)) ? (void *)hs : NULL;
}

#else

JSTR_ATTR_ACCESS((__read_only__, 2, 3))
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, _lt8)(const unsigned char *hs, const unsigned char *ne, unsigned int ne_len)
{
	uint64_t hw;
	uint64_t nw;
	const uint64_t mask = (uint64_t)-1 >> ((sizeof(uint64_t) - ne_len) << 3);
	for (; *ne; hw = (uint64_t)hw << 8 | L(*hs++), nw = (uint64_t)nw << 8 | L(*ne++)) {
		N_EXIT;
		if (jstr_unlikely(*hs == '\0'))
			return NULL;
	}
	for (hw &= mask, nw &= mask; N hw != nw && *hs; hw = (uint64_t)(hw << 8 | L(*hs++)) & mask) {}
	return hw == nw ? (char *)(hs - ne_len) : NULL;
}

#endif

#ifdef __clang__
#	pragma clang diagnostic pop
#endif
#ifdef __GNUC__
#	pragma GCC diagnostic pop
#endif

#undef L
#undef N
#undef N_EXIT
#undef N_PARAM
#undef PJSTR_STRSTR234_FUNC_NAME
#undef PJSTR_STRSTR234_CANON
#undef PJSTR_STRSTR234_MEMMEM
#undef PJSTR_STRSTR234_STRNSTR
