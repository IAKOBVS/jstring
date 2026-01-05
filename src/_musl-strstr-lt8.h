/* SPDX-License-Identifier: MIT */
/* Taken from or based on musl's string functions with modifications.
 * Copyright © 2005-2020 Rich Felker, et al.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#ifndef JSTR__STRSTR234_FUNC_NAME
#	define JSTR__STRSTR234_FUNC_NAME jstr__strstr
#endif
#ifndef JSTR__STRSTR234_CANON
#	define JSTR__STRSTR234_CANON(x) (x)
#endif
#define L (unsigned) JSTR__STRSTR234_CANON
#ifdef JSTR__STRSTR234_STRNSTR
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

#include "jstr-macros.h"

#ifdef JSTR__STRSTR234_MEMMEM

JSTR_FUNC_PURE
static char *
JSTR_CONCAT(JSTR__STRSTR234_FUNC_NAME, _lt8)(const unsigned char *hs, size_t hs_len, const unsigned char *ne, unsigned int ne_len)
JSTR_NOEXCEPT
{
	uint64_t hw = 0, nw = 0;
	const uint64_t mask = (uint64_t)-1 >> ((sizeof(uint64_t) - ne_len) << 3);
	for (unsigned int n = ne_len; n--; hw = (uint64_t)hw << 8 | L(*hs++), nw = (uint64_t)nw << 8 | L(*ne++))
		N_EXIT;
	for (hs_len -= ne_len, hw &= mask, nw &= mask; N hw != nw && hs_len--; hw = (uint64_t)(hw << 8 | L(*hs++)) & mask) {}
	return hw == nw ? (char *)(hs - ne_len) : NULL;
}

JSTR_FUNC_PURE
static char *
JSTR_CONCAT(JSTR__STRSTR234_FUNC_NAME, 2)(const unsigned char *h, size_t k, const unsigned char *n)
JSTR_NOEXCEPT
{
	const uint16_t nw = L(n[0]) << 8 | L(n[1]);
	uint16_t hw = L(h[0]) << 8 | L(h[1]);
	for (h += 2, k -= 2; N k-- && hw != nw; hw = hw << 8 | L(*h++))
		if (hw == nw)
			return (char *)h - 2;
	return hw == nw ? (char *)h - 2 : 0;
}

#elif JSTR__STRSTR234_MEMRMEM

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(JSTR__STRSTR234_FUNC_NAME, _lt8)(const unsigned char *hs, size_t hs_len, const unsigned char *ne, unsigned int ne_len)
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

JSTR_FUNC_PURE
static void *
JSTR_CONCAT(JSTR__STRSTR234_FUNC_NAME, _8more)(const unsigned char *hs, unsigned int hs_len, const unsigned char *ne, size_t ne_len)
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
JSTR_FUNC_PURE
static char *
JSTR_CONCAT(JSTR__STRSTR234_FUNC_NAME, _lt8)(const unsigned char *hs, const unsigned char *ne, unsigned int ne_len)
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

JSTR_FUNC_PURE
static char *
JSTR_CONCAT(JSTR__STRSTR234_FUNC_NAME, 2)(const unsigned char *h, const unsigned char *n)
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
#undef JSTR__STRSTR234_FUNC_NAME
#undef JSTR__STRSTR234_CANON
#undef JSTR__STRSTR234_MEMMEM
#undef JSTR__STRSTR234_STRNSTR
