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

PJSTR_BEGIN_DECLS
#include <stddef.h>
PJSTR_END_DECLS

#ifndef JSTR_LGPL_IMPL
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static void *
jstr_memrchr(const void *s,
             int c,
             size_t sz)
JSTR_NOEXCEPT
{
#endif
	if (jstr_unlikely(sz == 0))
		return NULL;
	char *ret;
	const jstr_word_ty *word_ptr = (jstr_word_ty *)JSTR_PTR_ALIGN_UP((const char *)s + sz, sizeof(jstr_word_ty));
	const uintptr_t s_int = (uintptr_t)s + sz;
	jstr_word_ty word = *--word_ptr;
	const jstr_word_ty repeated_c = jstr_word_repeat_bytes(c);
	const jstr_word_ty *const sword = (jstr_word_ty *)JSTR_PTR_ALIGN_DOWN(s, sizeof(jstr_word_ty));
	const jstr_word_ty mask = jstr_word_shift_find_last(jstr_word_find_eq_all(word, repeated_c), s_int);
	if (mask != 0) {
		ret = (char *)word_ptr + jstr_word_index_last(mask);
		return ret >= (char *)s ? ret : NULL;
	}
	if (word_ptr == sword)
		return NULL;
	word = *--word_ptr;
	for (; word_ptr != sword; word = *--word_ptr)
		if (jstr_word_has_eq(word, repeated_c))
			return (char *)word_ptr + jstr_word_index_last_eq(word, repeated_c);
	if (jstr_word_has_eq(word, repeated_c)) {
		ret = (char *)word_ptr + jstr_word_index_last_eq(word, repeated_c);
		if (ret >= (char *)s)
			return ret;
	}
	return NULL;
#ifndef JSTR_LGPL_IMPL
}
#endif

#endif /* JSTR_LGPL_MEMRCHR_H */
