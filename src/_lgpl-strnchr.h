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

#ifndef JSTR_STRNCHR_H
#define JSTR_STRNCHR_H 1

#include "jstr-macros.h"
#include "jstr-word-at-a-time.h"

JSTR_ATTR_INLINE
JSTR_FUNC_CONST
static const char *
pjstr_sadd(const uintptr_t x,
           const uintptr_t y)
JSTR_NOEXCEPT
{
	return (const char *)(y > UINTPTR_MAX - x ? UINTPTR_MAX : x + y);
}

JSTR_FUNC_PURE
static char *
jstr_strnchr(const char *s,
             int c,
             size_t n)
JSTR_NOEXCEPT
{
	const jstr_word_ty *word_ptr;
	uintptr_t s_int;
	jstr_word_ty word;
	jstr_word_ty repeated_c;
	const char *lbyte;
	const jstr_word_ty *lword;
	char *ret;
	jstr_word_ty mask;
	if (jstr_unlikely(n == 0)
	    || jstr_unlikely(*s == '\0'))
		return NULL;
	word_ptr = (jstr_word_ty *)JSTR_PTR_ALIGN_DOWN(s, sizeof(jstr_word_ty));
	s_int = (uintptr_t)s;
	word = jstr_word_toword(word_ptr);
	repeated_c = jstr_word_repeat_bytes(c);
	lbyte = pjstr_sadd(s_int, n - 1);
	lword = (jstr_word_ty *)JSTR_PTR_ALIGN_DOWN(lbyte, sizeof(jstr_word_ty));
	mask = jstr_word_shift_find(jstr_word_find_zero_eq_all(word, repeated_c), s_int);
	if (mask != 0) {
		ret = (char *)s + jstr_word_index_first(mask);
		return (ret <= lbyte && *ret) ? ret : NULL;
	}
	if (word_ptr == lword)
		return NULL;
	word = jstr_word_toword(++word_ptr);
	for (; word_ptr != lword; word = jstr_word_toword(++word_ptr)) {
		if (jstr_word_has_zero_eq(word, repeated_c)) {
			ret = (char *)word_ptr + jstr_word_index_first_zero_eq(word, repeated_c);
			return *ret ? ret : NULL;
		}
	}
	if (jstr_word_has_zero_eq(word, repeated_c)) {
		ret = (char *)word_ptr + jstr_word_index_first_zero_eq(word, repeated_c);
		if (ret <= lbyte && *ret)
			return ret;
	}
	return NULL;
}

#endif /* JSTR_STRNCHR_H */
