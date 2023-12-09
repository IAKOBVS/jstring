/* The following functions are based on newlib's strstr.
   Copyright (c) 2018 Arm Ltd.  All rights reserved.

   BSD-3-Clause

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
   3. The name of the company may not be used to endorse or promote
      products derived from this software without specific prior written
      permission.

   THIS SOFTWARE IS PROVIDED BY ARM LTD ``AS IS'' AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL ARM LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#ifndef PJSTR_STRSTR234_FUNC
#	define PJSTR_STRSTR234_FUNC pjstr_strstr
#endif
#ifndef PJSTR_STRSTR234_CANONIZE
#	define PJSTR_STRSTR234_CANONIZE(x) (x)
#endif
#define L PJSTR_STRSTR234_CANONIZE

#include "jstr-macros.h"

#ifdef PJSTR_STRSTR234_MEMMEM

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC, 2)(const unsigned char *hs,
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
	const size_ty nw = (size_ty)L(ne[0]) << SHIFT | L(ne[1]);
	size_ty hw = (size_ty)L(hs[0]) << SHIFT | L(hs[1]);
	for (++hs, --l; l-- && hw != nw; hw = hw << SHIFT | L(*++hs)) {}
	return (hw == nw) ? (void *)(hs - 1) : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC, 3)(const unsigned char *hs,
                                     const unsigned char *const ne,
                                     size_t l)
JSTR_NOEXCEPT
{
	const uint32_t nw = (uint32_t)L(ne[0]) << 24 | L(ne[1]) << 16 | L(ne[2]) << 8;
	uint32_t hw = (uint32_t)L(hs[0]) << 24 | L(hs[1]) << 16 | L(hs[2]) << 8;
	for (hs += 2, l -= 2; l-- && hw != nw; hw = (hw | L(*++hs)) << 8) {}
	return (hw == nw) ? (void *)(hs - 2) : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC, 4)(const unsigned char *hs,
                                     const unsigned char *const ne,
                                     size_t l)
JSTR_NOEXCEPT
{
	const uint32_t nw = (uint32_t)L(ne[0]) << 24 | L(ne[1]) << 16 | L(ne[2]) << 8 | L(ne[3]);
	uint32_t hw = (uint32_t)L(hs[0]) << 24 | L(hs[1]) << 16 | L(hs[2]) << 8 | L(hs[3]);
	for (hs += 3, l -= 3; l-- && hw != nw; hw = hw << 8 | L(*++hs)) {}
	return (hw == nw) ? (void *)(hs - 3) : NULL;
}

#else

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC, 2)(const unsigned char *hs,
                                     const unsigned char *const ne)
{
#	if JSTR_LP64
	typedef uint32_t size_ty;
	enum { SHIFT = 16 };
#	else
	typedef uint16_t size_ty;
	enum { SHIFT = 8 };
#	endif
	const size_ty h1 = (size_ty)(L(ne[0]) << SHIFT) | L(ne[1]);
	size_ty h2 = 0;
	unsigned int c;
	for (c = L(hs[0]); h1 != h2 && c != 0; c = L(*++hs))
		h2 = (h2 << SHIFT) | c;
	return h1 == h2 ? (char *)hs - 2 : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC, 3)(const unsigned char *hs,
                                     const unsigned char *const ne)
{
	const uint32_t h1 = (uint32_t)(L(ne[0]) << 24) | (L(ne[1]) << 16) | (L(ne[2]) << 8);
	uint32_t h2 = 0;
	unsigned int c;
	for (c = L(hs[0]); h1 != h2 && c != 0; c = L(*++hs))
		h2 = (h2 | c) << 8;
	return h1 == h2 ? (char *)hs - 3 : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
JSTR_CONCAT(PJSTR_STRSTR234_FUNC, 4)(const unsigned char *hs,
                                     const unsigned char *const ne)
{
	const uint32_t h1 = (uint32_t)(L(ne[0]) << 24) | (L(ne[1]) << 16) | (L(ne[2]) << 8) | L(ne[3]);
	uint32_t h2 = 0;
	unsigned int c;
	for (c = L(hs[0]); c != 0 && h1 != h2; c = L(*++hs))
		h2 = (h2 << 8) | c;
	return h1 == h2 ? (char *)hs - 4 : NULL;
}

#endif

#undef L
#undef PJSTR_STRSTR234_FUNC
#undef PJSTR_STRSTR234_CANONIZE
#undef PJSTR_STRSTR234_MEMMEM
