/* memrchr -- find the last occurrence of a byte in a memory block
   Copyright (C) 1991-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#ifndef JSTR_LGPL_MEMRCHR_H
#define JSTR_LGPL_MEMRCHR_H 1

#include "jstr-macros.h"
#include "jstr-word-at-a-time.h"

#ifndef JSTR_LGPL_IMPL
PJSTR_BEGIN_DECLS
#	include <stddef.h>
PJSTR_END_DECLS

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static void *
jstr_memrchr(const void *s,
             int c,
             size_t n)
JSTR_NOEXCEPT
{
#endif
	typedef jstr_word_ty op_t;
	typedef jstr_word_ty find_t;
	if (jstr_unlikely(n == 0))
		return NULL;
	const op_t *word_ptr = (const op_t *)JSTR_PTR_ALIGN_UP((char *)s + n, sizeof(op_t));
	uintptr_t s_int = (uintptr_t)s + n;
	op_t word = *--word_ptr;
	const op_t repeated_c = jstr_word_repeat_bytes(c);
	/* Compute the address of the word containing the initial byte. */
	const op_t *const sword = (const op_t *)JSTR_PTR_ALIGN_DOWN(s, sizeof(op_t));
	/* If the end of buffer is not op_t aligned, mask off the undesirable bits
	   before find the last byte position.  */
	const find_t mask = jstr_word_shift_find_last(jstr_word_find_eq_all(word, repeated_c), s_int);
	if (mask != 0) {
		char *ret = (char *)word_ptr + jstr_word_index_last(mask);
		return ret >= (char *)s ? ret : NULL;
	}
	if (word_ptr == sword)
		return NULL;
	word = *--word_ptr;
	while (word_ptr != sword) {
		if (jstr_word_has_eq(word, repeated_c))
			return (char *)word_ptr + jstr_word_index_last_eq(word, repeated_c);
		word = *--word_ptr;
	}
	if (jstr_word_has_eq(word, repeated_c)) {
		/* We found a match, but it might be in a byte past the end of the
		   array.  */
		char *ret = (char *)word_ptr + jstr_word_index_last_eq(word, repeated_c);
		if (ret >= (char *)s)
			return ret;
	}
	return NULL;
#ifndef JSTR_LGPL_IMPL
}
#endif

#endif /* JSTR_LGPL_MEMRCHR_H */
