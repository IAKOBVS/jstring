/* The following is based on newlib's memmem() with modifications.
   Copyright (c) 2018 Arm Ltd.  All rights reserved.

   SPDX-License-Identifier: BSD-3-Clause

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

#include "jstr-macros.h"
#include "jstr-stdstring.h"
#include <string.h>

#ifndef PJSTR_MEMMEM_RETTYPE
#	define PJSTR_MEMMEM_RETTYPE unsigned char *
#endif
#ifndef PJSTR_MEMMEM_FUNC
#	define PJSTR_MEMMEM_FUNC pjstr_memmem_lgpl
#endif
#define PJSTR_MEMMEM_HASH2(p) (((size_t)((p)[0]) - ((size_t)((p)[-1]) << 3)) % 256)
#ifndef PJSTR_MEMMEM_CHECK_EOL
#	define PJSTR_MEMMEM_CHECK_EOL 0
#endif
#ifndef PJSTR_MEMMEM_CMP_FUNC
#	define PJSTR_MEMMEM_CMP_FUNC memcmp
#endif
#if PJSTR_MEMMEM_HASH2_ICASE
#	define PJSTR_MEMMEM_HASH2_SETUP(p, fn1, fn2) (((size_t)fn1((p)[0]) - ((size_t)fn2((p)[-1]) << 3)) % 256)
#endif
#if PJSTR_MEMMEM_SHORT_NEEDLE
#	undef PJSTR_MEMMEM_FUNC_IMPL
#	define PJSTR_MEMMEM_FUNC_IMPL JSTR_CONCAT(PJSTR_MEMMEM_FUNC, _short_ne_impl)
#else
#	undef PJSTR_MEMMEM_FUNC_IMPL
#	define PJSTR_MEMMEM_FUNC_IMPL JSTR_CONCAT(PJSTR_MEMMEM_FUNC, _long_ne_impl)
#endif

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static PJSTR_MEMMEM_RETTYPE
PJSTR_MEMMEM_FUNC_IMPL(const unsigned char *hs,
                       size_t hs_len,
                       const unsigned char *ne,
                       size_t ne_len)
JSTR_NOEXCEPT
{
#if PJSTR_MEMMEM_SHORT_NEEDLE
	typedef int idx_ty;
	typedef uint8_t arr_ty;
#else
	typedef size_t idx_ty;
	typedef size_t arr_ty;
#endif
	const unsigned char *
#if !PJSTR_MEMMEM_CHECK_EOL
	const
#endif
	end
	= hs + hs_len - ne_len;
	size_t tmp;
	const size_t m1 = ne_len - 1;
	size_t off = 0;
	arr_ty shift[256];
	JSTR_BZERO_ARRAY(shift);
	for (idx_ty i = 1; i < (idx_ty)m1; ++i) {
		shift[PJSTR_MEMMEM_HASH2(ne + i)] = i;
#if PJSTR_MEMMEM_HASH2_ICASE
		shift[PJSTR_MEMMEM_HASH2_SETUP(ne + i, jstr_tolower, )] = i;
		shift[PJSTR_MEMMEM_HASH2_SETUP(ne + i, , jstr_tolower)] = i;
		shift[PJSTR_MEMMEM_HASH2_SETUP(ne + i, jstr_tolower, jstr_tolower)] = i;
#endif
	}
	const size_t shift1 = m1 - shift[PJSTR_MEMMEM_HASH2(ne + m1)];
	shift[PJSTR_MEMMEM_HASH2(ne + m1)] = m1;
#if PJSTR_MEMMEM_HASH2_ICASE
	shift[PJSTR_MEMMEM_HASH2_SETUP(ne + m1, jstr_tolower, )] = m1;
	shift[PJSTR_MEMMEM_HASH2_SETUP(ne + m1, , jstr_tolower)] = m1;
	shift[PJSTR_MEMMEM_HASH2_SETUP(ne + m1, jstr_tolower, jstr_tolower)] = m1;
#endif
	goto start;
	do {
#if PJSTR_MEMMEM_CHECK_EOL /* As in strstr() where HAYSTACKLEN needs to be updated. */
		if (jstr_unlikely(hs > end)) {
			end += jstr_strnlen((const char *)(end + m1), 2048);
			if (hs > end)
				return NULL;
		}
#endif
start:;
		do {
			hs += m1;
			tmp = shift[PJSTR_MEMMEM_HASH2(hs)];
		} while (!tmp && hs <= end);
		hs -= tmp;
		if (tmp < m1)
			continue;
		if (m1 < 15 || !PJSTR_MEMMEM_CMP_FUNC((const char *)(hs + off), (const char *)(ne + off), 8)) {
			if (!PJSTR_MEMMEM_CMP_FUNC((const char *)hs, (const char *)ne, m1))
				return (PJSTR_MEMMEM_RETTYPE)hs;
			off = (off >= 8 ? off : m1) - 8;
		}
		hs += shift1;
	}
#if PJSTR_MEMMEM_CHECK_EOL
	while (1);
#else
	while (hs <= end);
#endif
	return NULL;
}
