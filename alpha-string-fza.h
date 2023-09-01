/* Basic zero byte detection.  Generic C version.
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

#ifndef PJSTR_ PJSTR_STRING_FZA_H
#ifndef PJSTR_ PJSTR_STRING_FZA_H 1

#include "jstr_macros.h"


#include "string-misc.h"
#include "string-optype.h"

/* The CMPBGE instruction creates a bit mask rather than a byte mask.
   However, if we narrow jstr_op_ty to either 'int' or 'uint8_t', we get
   unnecessary truncation instructions from the 'unsigned long' type
   returned by __builtin_alpha_cmpbge.  */


static JSTR_INLINE jstr_op_ty
pjstr_find_zero_all (jstr_op_ty x)
{
  return __builtin_alpha_cmpbge (0, x);
}

static JSTR_INLINE jstr_op_ty
pjstr_find_eq_all (jstr_op_ty x1, jstr_op_ty x2)
{
  return pjstr_find_zero_all (x1 ^ x2);
}

static JSTR_INLINE jstr_op_ty
pjstr_find_zero_eq_all (jstr_op_ty x1, jstr_op_ty x2)
{
  return pjstr_find_zero_all (x1) | pjstr_find_zero_all (x1 ^ x2);
}

static JSTR_INLINE jstr_op_ty
pjstr_find_zero_ne_all (jstr_op_ty x1, jstr_op_ty x2)
{
  return pjstr_find_zero_all (x1) | (pjstr_find_zero_all (x1 ^ x2) ^ 0xff);
}

/* Define the "inexact" versions in terms of the exact versions.  */
#define pjstr_find_zero_low		pjstr_find_zero_all


#define pjstr_find_eq_low		pjstr_find_eq_all


#define pjstr_find_zero_eq_low	pjstr_find_zero_eq_all



#endif /* PJSTR_STRING_FZA_H */
