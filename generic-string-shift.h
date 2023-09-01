/* Shift unaligned word read  Generic C version.
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

#ifndef PJSTR_ PJSTR_STRING_SHIFT_H
#ifndef PJSTR_ PJSTR_STRING_SHIFT_H 1

#include "jstr_macros.h"


#include <endian.h>
#include <limits.h>
#include <stdint.h>
#include "string-fza.h"

/* Return the mask WORD shifted based on S_INT address value, to ignore
   values not presented in the aligned word read.  */
static JSTR_INLINE jstr_op_ty
pjstr_shift_find (jstr_op_ty word, uintptr_t s)
{
  if (__BYTE_ORDER == __LITTLE_ENDIAN)
    return word >> (CHAR_BIT * (s % sizeof (jstr_op_ty)));
  else
    return word << (CHAR_BIT * (s % sizeof (jstr_op_ty)));
}

/* Mask off the bits defined for the S alignment value, or return WORD if
   S is 0.  */
static JSTR_INLINE jstr_op_ty
pjstr_shift_find_last (jstr_op_ty word, uintptr_t s)
{
  s = s % sizeof (jstr_op_ty);
  if (s == 0)
    return word;
  if (__BYTE_ORDER == __LITTLE_ENDIAN)
    return word & ~(((jstr_op_ty)-1) << (s * CHAR_BIT));
  else
    return word & ~(((jstr_op_ty)-1) >> (s * CHAR_BIT));
}

#endif /* PJSTR_STRING_SHIFT_H */
