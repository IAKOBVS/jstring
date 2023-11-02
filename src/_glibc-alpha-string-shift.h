/* Shift unaligned word read.  Alpha version.
   Copyright (C) 2023 Free Software Foundation, Inc.
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
   <http://www.gnu.org/licenses/>.  */
#ifndef PJSTR_STRING_SHIFT_H
#define PJSTR_STRING_SHIFT_H 1
#include "jstr-macros.h"
#include "jstr-string-fza.h"
#include <limits.h>
#include <stdint.h>
/* Return the mask WORD shifted based on S_INT address value, to ignore
   values not presented in the aligned word read.  */
static JSTR_INLINE jstr_word_ty
jstr_word_shift_find(jstr_word_ty word, uintptr_t s)
{
	return word >> (s % sizeof(jstr_word_ty));
}
/* Mask off the bits defined the the S alignment value.  */
static JSTR_INLINE jstr_word_ty
jstr_word_shift_find_last(jstr_word_ty word, uintptr_t s)
{
	s = s % sizeof(jstr_word_ty);
	if (s == 0)
		return word;
	return word & ~((jstr_word_ty)-1 << s);
}
#endif /* _STRING_SHIFT_H */
