/* Miscellaneous functions used in string implementations.  Generic C version.
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
#ifndef PJSTR_STRING_MISC_H
#define PJSTR_STRING_MISC_H 1
#include "jstr-macros.h"
#include "string-optype.h"
#include <endian.h>
#include <limits.h>
/* Extract the byte at index IDX from word X, with index 0 being the
   least significant byte.  */
static JSTR_INLINE unsigned char
jstr_word_extractbyte(jstr_word_ty x, unsigned int idx)
{
	if (JSTR_ENDIAN_LITTLE)
		return x >> (idx * CHAR_BIT);
	else
		return x >> (sizeof(x) - 1 - idx) * CHAR_BIT;
}
/* Setup an word with each byte being c_in.  For instance, on a 64 bits
   machine with input as 0xce the functions returns 0xcececececececece.  */
static JSTR_INLINE jstr_word_ty
jstr_word_repeat_bytes(unsigned char c_in)
{
	return ((jstr_word_ty)-1 / 0xff) * c_in;
}
#endif /* _STRING_MISC_H */
