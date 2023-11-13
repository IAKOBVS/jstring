/* Zero byte detection, boolean.  Generic C version.
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

#ifndef JSTRP_STRING_FZB_H
#define JSTRP_STRING_FZB_H 1

#include "jstr-macros.h"

#include <endian.h>
#include "_string-fza.h"

/* Determine if any byte within X is zero.  This is a pure boolean test.  */

static JSTR_INLINE int
jstr_word_has_zero (jstr_word_ty x)
{
  return find_zero_low (x) != 0;
}

/* Likewise, but for byte equality between X1 and X2.  */

static JSTR_INLINE int
jstr_word_has_eq (jstr_word_ty x1, jstr_word_ty x2)
{
  return find_eq_low (x1, x2) != 0;
}

/* Likewise, but for zeros in X1 and equal bytes between X1 and X2.  */

static JSTR_INLINE int
jstr_word_has_zero_eq (jstr_word_ty x1, jstr_word_ty x2)
{
  return find_zero_eq_low (x1, x2);
}

#endif /* _STRING_FZB_H */
