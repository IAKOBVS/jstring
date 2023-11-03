#include "_jstr-macros.h"
#include "jstr-std-string.h"
#include <string.h>

#ifndef PJSTR_MEMMEM_RETTYPE
#	define PJSTR_MEMMEM_RETTYPE unsigned char *
#endif
#ifndef PJSTR_MEMMEM_FN
#	define PJSTR_MEMMEM_FN pjstr_memmem_impl
#endif
#ifndef PJSTR_MEMMEM_HASH2
#	define PJSTR_MEMMEM_HASH2(p) (((size_t)(((p)[0])) - ((size_t)((p)[-1]) << 3)) % 256)
#endif
#ifndef PJSTR_MEMMEM_CHECK_EOL
#	define PJSTR_MEMMEM_CHECK_EOL (0)
#endif
#ifndef PJSTR_MEMMEM_CMP_FN
#	define PJSTR_MEMMEM_CMP_FN memcmp
#endif

#if PJSTR_MEMMEM_SHORT_NEEDLE
#	undef PJSTR_MEMMEM_FN_IMPL
#	define PJSTR_MEMMEM_FN_IMPL JSTR_CONCAT(PJSTR_, JSTR_CONCAT(PJSTR_MEMMEM_FN, _short_ne_impl))
#else
#	undef PJSTR_MEMMEM_FN_IMPL
#	define PJSTR_MEMMEM_FN_IMPL JSTR_CONCAT(PJSTR_, JSTR_CONCAT(PJSTR_MEMMEM_FN, _long_ne_impl))
#endif

JSTR_FUNC_PURE
JSTR_INLINE
static PJSTR_MEMMEM_RETTYPE
PJSTR_MEMMEM_FN_IMPL(const unsigned char *JSTR_RESTRICT hs,
		     const size_t hl,
		     const unsigned char *JSTR_RESTRICT ne,
		     const size_t nl)
JSTR_NOEXCEPT
{
#if PJSTR_MEMMEM_SHORT_NEEDLE
	typedef int idx_ty;
	typedef uint8_t arr_ty;
#else
	typedef size_t idx_ty;
	typedef size_t arr_ty;
#endif
	/* Based on glibc memmem and strstr released under the terms of the GNU Lesser General Public License.
	   Copyright (C) 1991-2023 Free Software Foundation, Inc. */
	const unsigned char *end = hs + hl - nl;
	size_t tmp;
	const size_t m1 = nl - 1;
	size_t off = 0;
	arr_ty shift[256];
	JSTR_BZERO_ARRAY(shift);
	for (idx_ty i = 1; i < (idx_ty)m1; ++i)
		shift[PJSTR_MEMMEM_HASH2(ne + i)] = i;
	const size_t shift1 = m1 - shift[PJSTR_MEMMEM_HASH2(ne + m1)];
	shift[PJSTR_MEMMEM_HASH2(ne + m1)] = m1;
	goto start;
	do {
#if PJSTR_MEMMEM_CHECK_EOL /* As in strstr() where we don't know where HAYSTACKLEN needs to be updated. */
		if (jstr_unlikely(hs > end)) {
			end += jstr_strnlen((char *)(end + m1), 2048);
			if (hs > end)
				return NULL;
		}
#endif
start:;
		do {
			hs += m1;
			tmp = shift[PJSTR_MEMMEM_HASH2(hs)];
		} while (!tmp & (hs <= end));
		hs -= tmp;
		if (tmp < m1)
			continue;
		if (m1 < 15 || !PJSTR_MEMMEM_CMP_FN((char *)(hs + off), (char *)(ne + off), 8)) {
			if (!PJSTR_MEMMEM_CMP_FN((char *)hs, (char *)ne, m1))
				return (PJSTR_MEMMEM_RETTYPE)hs;
			off = (off >= 8 ? off : m1) - 8;
		}
		hs += shift1;
	} while (hs <= end);
	return NULL;
}
