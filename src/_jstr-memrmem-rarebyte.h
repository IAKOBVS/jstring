/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023 James Tirta Halim <tirtajames45 at gmail dot com>
   This file is part of the jstring library.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE. */

#include "jstr-macros.h"
#include "jstr-ctype.h"
#include "jstr-stdstring.h"

PJSTR_BEGIN_DECLS
#include <stdint.h>
#include <string.h>
PJSTR_END_DECLS

#ifndef PJSTR_RAREBYTE_RETTYPE
#	define PJSTR_RAREBYTE_RETTYPE void *
#endif
#ifndef PJSTR_RAREBYTE_FUNC
#	define PJSTR_RAREBYTE_FUNC pjstr_memrmem_rarebyte
#endif
#if !defined PJSTR_RAREBYTE_CMP_FUNC && JSTR_HAVE_UNALIGNED_ACCESS && (JSTR_HAVE_ATTR_MAY_ALIAS || JSTR_HAVE_BUILTIN_MEMCMP)
#	define USE_UNALIGNED 1
#endif
#ifndef PJSTR_RAREBYTE_CMP_FUNC
#	define PJSTR_RAREBYTE_CMP_FUNC memcmp
#endif
#define CMP_FUNC PJSTR_RAREBYTE_CMP_FUNC
#if JSTR_HAVE_ATTR_MAY_ALIAS
#	define EQ32(hs, ne_align) (JSTR_BYTE_TOWORDU32(hs) == (uint32_t)ne_align)
#	define EQ64(hs, ne_align) (JSTR_BYTE_TOWORDU64(hs) == ne_align)
#else
#	define EQ64(hs, ne_align) !memcmp(hs, &(ne_align), 8)
#	define EQ32(hs, ne_align) !memcmp(hs, &(ne_align), 4)
#endif
#ifndef USE_UNALIGNED
#	define USE_UNALIGNED 0
#endif
#ifndef PJSTR_RAREBYTE_CANONIZE
#	define PJSTR_RAREBYTE_CANONIZE(x) (x)
#endif
#define CANONIZE PJSTR_RAREBYTE_CANONIZE

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static PJSTR_RAREBYTE_RETTYPE
PJSTR_RAREBYTE_FUNC(const unsigned char *hs,
                    size_t hs_len,
                    const unsigned char *ne,
                    size_t ne_len,
                    const unsigned char *const rarebyte)
{
	typedef PJSTR_RAREBYTE_RETTYPE ret_ty;
	typedef const unsigned char cu;
	const int c = *rarebyte;
	const size_t shift = JSTR_PTR_DIFF(rarebyte, ne);
	const unsigned char *p = hs + hs_len - (ne_len - shift) + 1;
	hs += shift;
#if !USE_UNALIGNED
	const int c0 = CANONIZE(*ne);
	for (; (p = (cu *)memrchr(hs, c, JSTR_PTR_DIFF(p, hs)));)
		if (CANONIZE(*(p - shift)) == c0 && !CMP_FUNC((char *)p - shift, (char *)ne, ne_len))
			return (ret_ty)(p - shift);
#else
	const int short_ne = ne_len < 8;
	uint64_t ne_align;
	if (short_ne) {
		ne_align = (uint64_t)JSTR_BYTE_TOWORDU32(ne);
		ne += 4;
		ne_len -= 4;
	} else {
		ne_align = JSTR_BYTE_TOWORDU64(ne);
		ne += 8;
		ne_len -= 8;
	}
	for (; (p = (cu *)memrchr(hs, c, JSTR_PTR_DIFF(p, hs)));)
		/* If CMP_FUNC is not defined, use memcmp() and quickly compare first 4/8 bytes before calling memcmp(). */
		if (short_ne) {
			if (EQ32(p - shift, ne_align)
			    && !jstr_memcmpeq_loop(p - shift + 4, ne, ne_len))
				return (ret_ty)(p - shift);
		} else {
			if (EQ64(p - shift, ne_align)
			    && !memcmp(p - shift + 8, ne, ne_len))
				return (ret_ty)(p - shift);
		}
#endif
	return NULL;
}

#undef USE_UNALIGNED
#undef EQ32
#undef EQ64
#undef TOWORD64
#undef TOWORD32
#undef PJSTR_RAREBYTE_FUNC
#undef PJSTR_RAREBYTE_CMP_FUNC
#undef CMP_FUNC
#undef PJSTR_RAREBYTE_RETTYPE
#undef PJSTR_RAREBYTE_CANONIZE
#undef CANONIZE
