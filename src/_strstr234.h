/* SPDX-License-Identifier: BSD-3-Clause */
/* The following functions are based on newlib's strstr.
 * Copyright (c) 2018 Arm Ltd.  All rights reserved.
 *
 * BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the company may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ARM LTD ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ARM LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#ifndef PJSTR_STRSTR234_FUNC_NAME
#	define PJSTR_STRSTR234_FUNC_NAME pjstr_strstr
#endif
#ifndef PJSTR_STRSTR234_CANON
#	define PJSTR_STRSTR234_CANON(x) (x)
#endif
#define L PJSTR_STRSTR234_CANON
#ifdef PJSTR_STRSTR234_STRNSTR
#	define N_PARAM , size_t l
#	define N       l-- &&
#else
#	define N_PARAM
#	define N
#endif

#include "jstr-macros.h"

#ifdef PJSTR_STRSTR234_MEMMEM

/* May not work. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, _lt8)(const unsigned char *hs,
                                             size_t hs_len,
                                             const unsigned char *ne,
                                             unsigned int ne_len)
{
	if (ne_len <= 4) {
		uint32_t h = 0;
		uint32_t n = 0;
		const unsigned int shift = (sizeof(uint32_t) - ne_len) << 3;
		for (unsigned int i = ne_len; i--; h = (h << 8) | L(*hs++), n = (n << 8) | L(*ne++)) {}
		h <<= shift;
		n <<= shift;
		for (hs_len -= ne_len; hs_len-- && h != n; h = (h << 8) | (*hs++ << shift)) {}
		return (h == n) ? (char *)(hs - ne_len) : NULL;
	} else {
		uint64_t h = 0;
		uint64_t n = 0;
		const unsigned int shift = (sizeof(uint64_t) - ne_len) << 3;
		for (unsigned int i = ne_len; i--; h = (h << 8) | L(*hs++), n = (n << 8) | L(*ne++)) {}
		h <<= shift;
		n <<= shift;
		for (hs_len -= ne_len; hs_len-- && h != n; h = (h << 8) | (L(*hs++) << shift)) {}
		return (h == n) ? (char *)(hs - ne_len) : NULL;
	}
}

#else

/* May not work. */
JSTR_ATTR_ACCESS((__read_only__, 2, 3))
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, _lt8)(const unsigned char *hs,
                                             const unsigned char *ne,
                                             unsigned int ne_len
                                             N_PARAM)
{
	if (ne_len <= 4) {
		uint32_t h = 0;
		uint32_t n = 0;
		const unsigned int shift = (sizeof(uint32_t) - ne_len) << 3;
		for (unsigned int i = ne_len; N i-- && *hs; h = (h << 8) | L(*hs++), n = (n << 8) | L(*ne++)) {}
		h <<= shift;
		n <<= shift;
		for (; N h != n; h = (h << 8) | (L(*hs++) << shift)) {}
		return (h == n) ? (char *)(hs - ne_len) : NULL;
	} else {
		uint64_t h = 0;
		uint64_t n = 0;
		const unsigned int shift = (sizeof(uint64_t) - ne_len) << 3;
		for (unsigned int i = ne_len; N i-- && *hs; h = (h << 8) | L(*hs++), n = (n << 8) | L(*ne++)) {}
		h <<= shift;
		n <<= shift;
		for (; N h != n; h = (h << 8) | (L(*hs++) << shift)) {}
		return (h == n) ? (char *)(hs - ne_len) : NULL;
	}
}

#endif

#ifdef PJSTR_STRSTR234_MEMMEM

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 2)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          size_t l)
JSTR_NOEXCEPT
{
#	if JSTR_LP64
	typedef uint32_t size_ty;
	enum { sh = 16 };
#	else
	typedef uint16_t size_ty;
	enum { sh = 8 };
#	endif
	const size_ty nw = (size_ty)L(ne[0]) << sh | (size_ty)L(ne[1]);
	size_ty hw = (size_ty)L(hs[0]) << sh | (size_ty)L(hs[1]);
	for (hs += 2, l -= 2; l-- && hw != nw; hw = hw << sh | L(*hs++)) {}
	return (hw == nw) ? (char *)(hs - 2) : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 3)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          size_t l)
JSTR_NOEXCEPT
{
	typedef uint32_t U32;
	const U32 nw = (U32)L(ne[0]) << 24 | (U32)L(ne[1]) << 16 | (U32)L(ne[2]) << 8;
	U32 hw = (U32)L(hs[0]) << 24 | (U32)L(hs[1]) << 16 | (U32)L(hs[2]) << 8;
	for (hs += 3, l -= 3; l-- && hw != nw; hw = (hw | L(*hs++)) << 8) {}
	return (hw == nw) ? (char *)(hs - 3) : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 4)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          size_t l)
JSTR_NOEXCEPT
{
	typedef uint32_t U32;
	const U32 nw = (U32)L(ne[0]) << 24 | (U32)L(ne[1]) << 16 | (U32)L(ne[2]) << 8 | (U32)L(ne[3]);
	U32 hw = (U32)L(hs[0]) << 24 | (U32)L(hs[1]) << 16 | (U32)L(hs[2]) << 8 | (U32)L(hs[3]);
	for (hs += 4, l -= 4; l-- && hw != nw; hw = hw << 8 | L(*hs++)) {}
	return (hw == nw) ? (char *)(hs - 4) : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 5)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          size_t l)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	const U64 nw = (U64)L(ne[0]) << 56 | (U64)L(ne[1]) << 48 | (U64)L(ne[2]) << 40 | (U64)L(ne[3]) << 32 | (U64)L(ne[4]) << 24;
	U64 hw = (U64)L(hs[0]) << 56 | (U64)L(hs[1]) << 48 | (U64)L(hs[2]) << 40 | (U64)L(hs[3]) << 32 | (U64)L(hs[4]) << 24;
	for (hs += 5, l -= 5; l-- && hw != nw; hw = hw << 8 | L(*hs++) << 24) {}
	return (hw == nw) ? (char *)(hs - 5) : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 6)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          size_t l)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	const U64 nw = (U64)L(ne[0]) << 56 | (U64)L(ne[1]) << 48 | (U64)L(ne[2]) << 40 | (U64)L(ne[3]) << 32 | (U64)L(ne[4]) << 24 | (U64)L(ne[5]) << 16;
	U64 hw = (U64)L(hs[0]) << 56 | (U64)L(hs[1]) << 48 | (U64)L(hs[2]) << 40 | (U64)L(hs[3]) << 32 | (U64)L(hs[4]) << 24 | (U64)L(hs[5]) << 16;
	for (hs += 6, l -= 6; l-- && hw != nw; hw = hw << 8 | L(*hs++) << 16) {}
	return (hw == nw) ? (char *)(hs - 6) : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 7)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          size_t l)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	const U64 nw = (U64)L(ne[0]) << 56 | (U64)L(ne[1]) << 48 | (U64)L(ne[2]) << 40 | (U64)L(ne[3]) << 32 | (U64)L(ne[4]) << 24 | (U64)L(ne[5]) << 16 | (U64)L(ne[6]) << 8;
	U64 hw = (U64)L(hs[0]) << 56 | (U64)L(hs[1]) << 48 | (U64)L(hs[2]) << 40 | (U64)L(hs[3]) << 32 | (U64)L(hs[4]) << 24 | (U64)L(hs[5]) << 16 | (U64)L(hs[6]) << 8;
	for (hs += 7, l -= 7; l-- && hw != nw; hw = (hw | L(*hs++)) << 8) {}
	return (hw == nw) ? (char *)(hs - 7) : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 8)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          size_t l)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	const U64 nw = (U64)L(ne[0]) << 56 | (U64)L(ne[1]) << 48 | (U64)L(ne[2]) << 40 | (U64)L(ne[3]) << 32 | (U64)L(ne[4]) << 24 | (U64)L(ne[5]) << 16 | (U64)L(ne[6]) << 8 | (U64)L(ne[7]);
	U64 hw = (U64)L(hs[0]) << 56 | (U64)L(hs[1]) << 48 | (U64)L(hs[2]) << 40 | (U64)L(hs[3]) << 32 | (U64)L(hs[4]) << 24 | (U64)L(hs[5]) << 16 | (U64)L(hs[6]) << 8 | (U64)L(hs[7]);
	for (hs += 8, l -= 8; l-- && hw != nw; hw = hw << 8 | L(*hs++)) {}
	return (hw == nw) ? (char *)(hs - 8) : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 8more)(const unsigned char *hs,
                                              const unsigned char *const ne,
                                              size_t l,
                                              size_t ne_len)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	const U64 nw = (U64)L(ne[0]) << 56 | (U64)L(ne[1]) << 48 | (U64)L(ne[2]) << 40 | (U64)L(ne[3]) << 32 | (U64)L(ne[4]) << 24 | (U64)L(ne[5]) << 16 | (U64)L(ne[6]) << 8 | (U64)L(ne[7]);
	U64 hw = (U64)L(hs[0]) << 56 | (U64)L(hs[1]) << 48 | (U64)L(hs[2]) << 40 | (U64)L(hs[3]) << 32 | (U64)L(hs[4]) << 24 | (U64)L(hs[5]) << 16 | (U64)L(hs[6]) << 8 | (U64)L(hs[7]);
	const unsigned char *const ne_rest = ne + 8;
	const size_t ne_rest_len = ne_len - 8;
	for (hs += 8, l -= ne_len; l--; hw = hw << 8 | L(*hs++))
		if (hw == nw && !memcmp(hs, ne_rest, ne_rest_len))
			return (char *)(hs - 8);
	return (hw == nw && !memcmp(hs, ne_rest, ne_rest_len)) ? (char *)(hs - 8) : NULL;
}

#elif PJSTR_STRSTR234_MEMRMEM

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 2)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          size_t l)
JSTR_NOEXCEPT
{
#	if JSTR_LP64
	typedef uint32_t size_ty;
	enum { SHIFT = 16 };
#	else
	typedef uint16_t size_ty;
	enum { SHIFT = 8 };
#	endif
	hs += l - 2;
	const size_ty nw = (size_ty)L(ne[1]) << SHIFT | (size_ty)L(ne[0]);
	size_ty hw = (size_ty)L(hs[1]) << SHIFT | (size_ty)L(hs[0]);
	for (l -= 2; l-- && hw != nw; hw = hw << SHIFT | L(*--hs)) {}
	return (hw == nw) ? (void *)hs : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 3)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          size_t l)
JSTR_NOEXCEPT
{
	typedef uint32_t U32;
	hs += l - 3;
	const uint32_t nw = (U32)L(ne[2]) << 24 | (U32)L(ne[1]) << 16 | (U32)L(ne[0]) << 8;
	uint32_t hw = (U32)L(hs[2]) << 24 | (U32)L(hs[1]) << 16 | (U32)L(hs[0]) << 8;
	for (l -= 3; l-- && hw != nw; hw = (hw | L(*--hs)) << 8) {}
	return (hw == nw) ? (void *)hs : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 4)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          size_t l)
JSTR_NOEXCEPT
{
	typedef uint32_t U32;
	hs += l - 4;
	const uint32_t nw = (U32)L(ne[3]) << 24 | (U32)L(ne[2]) << 16 | (U32)L(ne[1]) << 8 | (U32)L(ne[0]);
	uint32_t hw = (U32)L(hs[3]) << 24 | (U32)L(hs[2]) << 16 | (U32)L(hs[1]) << 8 | (U32)L(hs[0]);
	for (l -= 4; l-- && hw != nw; hw = hw << 8 | L(*--hs)) {}
	return (hw == nw) ? (void *)hs : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 5)(const unsigned char *hs,
                                          const unsigned char *const ne,
                                          size_t l)
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
                                          size_t l)
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
                                          size_t l)
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
                                          size_t l)
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
                                              size_t l,
                                              size_t ne_len)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	hs += l - ne_len;
	const U64 nw = (U64)L(ne[7]) << 56 | (U64)L(ne[6]) << 48 | (U64)L(ne[5]) << 40 | (U64)L(ne[4]) << 32 | (U64)L(ne[3]) << 24 | (U64)L(ne[2]) << 16 | (U64)L(ne[1]) << 8 | (U64)L(ne[0]);
	U64 hw = (U64)L(hs[7]) << 56 | (U64)L(hs[6]) << 48 | (U64)L(hs[5]) << 40 | (U64)L(hs[4]) << 32 | (U64)L(hs[3]) << 24 | (U64)L(hs[2]) << 16 | (U64)L(hs[1]) << 8 | (U64)L(hs[0]);
	const unsigned char *const ne_rest = ne + 8;
	const size_t ne_rest_len = ne_len - 8;
	for (l -= ne_len; l--; hw = hw << 8 | L(*--hs))
		if (hw == nw && !memcmp(hs + 8, ne_rest, ne_rest_len))
			return (void *)hs;
	return (hw == nw && !memcmp(hs + 8, ne_rest, ne_rest_len)) ? (void *)hs : NULL;
}

#else

JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 2)(const unsigned char *hs,
                                          const unsigned char *const ne
                                          N_PARAM)
{
#	if JSTR_LP64
	typedef uint32_t size_ty;
	enum { sh = 16 };
#	else
	typedef uint16_t size_ty;
	enum { sh = 8 };
#	endif
	const size_ty nw = (size_ty)(L(ne[0]) << sh) | (size_ty)L(ne[1]);
	size_ty hw = 0;
	unsigned int c;
	for (c = L(hs[0]); N nw != hw && c != 0; c = L(*++hs))
		hw = (hw << sh) | c;
	return nw == hw ? (char *)hs - 2 : NULL;
}

JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 3)(const unsigned char *hs,
                                          const unsigned char *const ne
                                          N_PARAM)
JSTR_NOEXCEPT
{
	typedef uint32_t U32;
	const U32 nw = (U32)(L(ne[0]) << 24) | (U32)L(ne[1]) << 16 | (U32)L(ne[2]) << 8;
	U32 hw = 0;
	unsigned int c;
	for (c = L(hs[0]); N nw != hw && c != 0; c = L(*++hs))
		hw = (hw | c) << 8;
	return nw == hw ? (char *)hs - 3 : NULL;
}

JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 4)(const unsigned char *hs,
                                          const unsigned char *const ne
                                          N_PARAM)
JSTR_NOEXCEPT
{
	typedef uint32_t U32;
	const U32 nw = (U32)(L(ne[0]) << 24) | (U32)L(ne[1]) << 16 | (U32)L(ne[2]) << 8 | (U32)L(ne[3]);
	U32 hw = 0;
	unsigned int c;
	for (c = L(hs[0]); N c != 0 && nw != hw; c = L(*++hs))
		hw = (hw << 8) | c;
	return nw == hw ? (char *)hs - 4 : NULL;
}

JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 5)(const unsigned char *hs,
                                          const unsigned char *const ne
                                          N_PARAM)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	const U64 nw = (U64)L(ne[0]) << 56 | (U64)L(ne[1]) << 48 | (U64)L(ne[2]) << 40 | (U64)L(ne[3]) << 32 | (U64)L(ne[4]) << 24;
	U64 hw = 0;
	unsigned int c;
	for (c = L(hs[0]); N c != 0 && nw != hw; c = L(*++hs))
		hw = (hw << 8) | c << 24;
	return nw == hw ? (char *)hs - 5 : NULL;
}

JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 6)(const unsigned char *hs,
                                          const unsigned char *const ne
                                          N_PARAM)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	const U64 nw = (U64)L(ne[0]) << 56 | (U64)L(ne[1]) << 48 | (U64)L(ne[2]) << 40 | (U64)L(ne[3]) << 32 | (U64)L(ne[4]) << 24 | (U64)L(ne[5]) << 16;
	U64 hw = 0;
	unsigned int c;
	for (c = L(hs[0]); N c != 0 && nw != hw; c = L(*++hs))
		hw = (hw << 8) | c << 16;
	return nw == hw ? (char *)hs - 6 : NULL;
}

JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 7)(const unsigned char *hs,
                                          const unsigned char *const ne
                                          N_PARAM)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	const U64 nw = (U64)L(ne[0]) << 56 | (U64)L(ne[1]) << 48 | (U64)L(ne[2]) << 40 | (U64)L(ne[3]) << 32 | (U64)L(ne[4]) << 24 | (U64)L(ne[5]) << 16 | (U64)L(ne[6]) << 8;
	U64 hw = 0;
	unsigned int c;
	for (c = L(hs[0]); N c != 0 && nw != hw; c = L(*++hs))
		hw = (hw | c) << 8;
	return nw == hw ? (char *)hs - 7 : NULL;
}

JSTR_FUNC_PURE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC_NAME, 8)(const unsigned char *hs,
                                          const unsigned char *const ne
                                          N_PARAM)
JSTR_NOEXCEPT
{
	typedef uint64_t U64;
	const U64 nw = (U64)L(ne[0]) << 56 | (U64)L(ne[1]) << 48 | (U64)L(ne[2]) << 40 | (U64)L(ne[3]) << 32 | (U64)L(ne[4]) << 24 | (U64)L(ne[5]) << 16 | (U64)L(ne[6]) << 8 | (U64)L(ne[7]);
	U64 hw = 0;
	unsigned int c;
	for (c = L(hs[0]); N c != 0 && nw != hw; c = L(*++hs))
		hw = (hw << 8) | c;
	return nw == hw ? (char *)hs - 8 : NULL;
}

#endif

#undef L
#undef N
#undef N_PARAM
#undef PJSTR_STRSTR234_FUNC_NAME
#undef PJSTR_STRSTR234_CANON
#undef PJSTR_STRSTR234_MEMMEM
#undef PJSTR_STRSTR234_STRNSTR
