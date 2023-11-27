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
   SOFTWARE.

   MIT License (Expat) */

#include "jstr-ctype.h"
#include "jstr-macros.h"
#include "jstr-stdstring.h"

PJSTR_BEGIN_DECLS
#include <stdint.h>
#include <string.h>
PJSTR_END_DECLS

#ifndef PJSTR_RAREBYTE_RETTYPE
#	define PJSTR_RAREBYTE_RETTYPE void *
#endif
#ifndef PJSTR_RAREBYTE_FUNC
#	define PJSTR_RAREBYTE_FUNC pjstr_memmem_rarebyte
#endif

#if !defined PJSTR_RAREBYTE_CMP_FUNC && JSTR_HAVE_UNALIGNED_ACCESS
#	define PJSTR_RAREBYTE_CMP_FUNC memcmp
#	define USE_UNALIGNED           1
#	if JSTR_ENDIAN_LITTLE
#		define SH <<
#	elif JSTR_ENDIAN_BIG
#		define SH >>
#	else
#		error "Can't detect endianness."
#	endif
#	define I(i)        (i * 8)
#	define TOWORD32(x) ((uint32_t)(x)[0] SH I(0) | (uint32_t)(x)[1] SH I(1) | (uint32_t)(x)[2] SH I(2) | (uint32_t)(x)[3] SH I(3))
#	define TOWORD64(x) ((uint64_t)TOWORD32((x)) | (uint64_t)(x)[4] SH I(4) | (uint64_t)(x)[5] SH I(5) | (uint64_t)(x)[6] SH I(6) | (uint64_t)(x)[7] SH I(7))
#	if JSTR_HAVE_ATTR_MAY_ALIAS
#		define EQ32(hs, ne_align) (*(u32 *)(hs) == (uint32_t)ne_align)
#		define EQ64(hs, ne_align) (*(u64 *)(hs) == ne_align)
#	else
#		define EQ64(hs, ne_align) !memcmp(hs, &(ne_align), 8)
#		define EQ32(hs, ne_align) !memcmp(hs, &(ne_align), 4)
#	endif
#endif
#define CMP_FUNC PJSTR_RAREBYTE_CMP_FUNC

#ifndef USE_UNALIGNED
#	define USE_UNALIGNED 0
#endif

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
	typedef unsigned char u;
	const int c = *(u *)rarebyte;
	const size_t shift = JSTR_PTR_DIFF(rarebyte, ne);
	const u *const end = hs + hs_len - (ne_len - shift) + 1;
	hs += shift;
#if USE_UNALIGNED
	typedef uint32_t u32 JSTR_ATTR_MAY_ALIAS;
	typedef uint64_t u64 JSTR_ATTR_MAY_ALIAS;
	uint64_t ne_align;
	const int short_ne = ne_len < 8;
	if (short_ne) {
		if (JSTR_HAVE_ATTR_MAY_ALIAS)
			ne_align = (uint64_t) * (u32 *)ne;
		else
			ne_align = (uint64_t)TOWORD32(ne);
		ne += 4;
		ne_len -= 4;
	} else {
		if (JSTR_HAVE_ATTR_MAY_ALIAS)
			ne_align = *(u64 *)ne;
		else
			ne_align = TOWORD64(ne);
		ne += 8;
		ne_len -= 8;
	}
#endif
	for (; (hs = (const u *)memchr(hs, c, end - hs)); ++hs) {
#if USE_UNALIGNED
		/* If CMP_FUNC is memcmp(), quickly compare first 4/8 bytes before calling memcmp(). */
		if (short_ne) {
			if (EQ32(hs - shift, ne_align) && !jstr_memcmpeq_loop(hs - shift + 4, ne, ne_len))
				return (ret_ty)(hs - shift);
		} else {
			if (EQ64(hs - shift, ne_align) && !memcmp(hs - shift + 8, ne, ne_len))
				return (ret_ty)(hs - shift);
		}
#else
		if (!CMP_FUNC((char *)hs - shift, (char *)ne, ne_len))
			return (ret_ty)(hs - shift);
#endif
	}
	return NULL;
}

#undef USE_UNALIGNED
#undef EQ32
#undef EQ64
#undef TOWORD64
#undef TOWORD32
#undef I
#undef SH
#undef PJSTR_RAREBYTE_FUNC
#undef PJSTR_RAREBYTE_CMP_FUNC
#undef CMP_FUNC
#undef PJSTR_RAREBYTE_RETTYPE
