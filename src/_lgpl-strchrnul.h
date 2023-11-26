/* Copyright (C) 1991-2023 Free Software Foundation, Inc.
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

#ifndef JSTR_LGPL_STRCHRNUL_H
#define JSTR_LGPL_STRCHRNUL_H 1

#include "jstr-macros.h"
#include "jstr-word-at-a-time.h"

#ifndef JSTR_LGPL_IMPL
JSTR_FUNC_PURE
static char *
jstr_strchrnul(const char *s,
               int c)
JSTR_NOEXCEPT
{
#endif
	const uintptr_t s_int = (uintptr_t)s;
	const jstr_word_ty *word_ptr = (const jstr_word_ty *)JSTR_PTR_ALIGN_DOWN(s, sizeof(jstr_word_ty));
	const jstr_word_ty repeated_c = jstr_word_repeat_bytes(c);
	jstr_word_ty word = *word_ptr;
	const jstr_word_ty mask = jstr_word_shift_find(jstr_word_find_zero_eq_all(word, repeated_c), s_int);
	if (mask != 0)
		return (char *)s + jstr_word_index_first(mask);
	do
		word = *++word_ptr;
	while (!jstr_word_has_zero_eq(word, repeated_c));
	return (char *)word_ptr + jstr_word_index_first_zero_eq(word, repeated_c);
#ifndef JSTR_LGPL_IMPL
}
#endif

#endif /* JSTR_LGPL_STRCHRNUL_H */
