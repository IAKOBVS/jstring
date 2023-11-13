/* Zero byte detection; jstr_word_indexes.  Generic C version.
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

#ifndef JSTRP_STRING_FZC_H
#define JSTRP_STRING_FZC_H 1

#include "jstr-macros.h"

#include <endian.h>
#include "_string-fza.h"
#include "_string-fzi.h"

/* Given a word X that is known to contain a zero byte, return the jstr_word_index of
   the first such within the word in memory order.  */
static JSTR_INLINE unsigned int
jstr_word_index_first_zero (jstr_word_ty x)
{
  if (JSTR_ENDIAN_LITTLE)
    x = jstr_word_find_zero_low (x);
  else
    x = jstr_word_find_zero_all (x);
  return jstr_word_index_first (x);
}

/* Similarly, but perform the search for byte equality between X1 and X2.  */
static JSTR_INLINE unsigned int
jstr_word_index_first_eq (jstr_word_ty x1, jstr_word_ty x2)
{
  if (JSTR_ENDIAN_LITTLE)
    x1 = jstr_word_find_eq_low (x1, x2);
  else
    x1 = jstr_word_find_eq_all (x1, x2);
  return jstr_word_index_first (x1);
}

/* Similarly, but perform the search for zero within X1 or equality between
   X1 and X2.  */
static JSTR_INLINE unsigned int
jstr_word_index_first_zero_eq (jstr_word_ty x1, jstr_word_ty x2)
{
  if (JSTR_ENDIAN_LITTLE)
    x1 = jstr_word_find_zero_eq_low (x1, x2);
  else
    x1 = jstr_word_find_zero_eq_all (x1, x2);
  return jstr_word_index_first (x1);
}

/* Similarly, but perform the search for zero within X1 or inequality between
   X1 and X2.  */
static JSTR_INLINE unsigned int
jstr_word_index_first_zero_ne (jstr_word_ty x1, jstr_word_ty x2)
{
  return jstr_word_index_first (jstr_word_find_zero_ne_all (x1, x2));
}

/* Similarly, but search for the last zero within X.  */
static JSTR_INLINE unsigned int
jstr_word_index_last_zero (jstr_word_ty x)
{
  if (JSTR_ENDIAN_LITTLE)
    x = jstr_word_find_zero_all (x);
  else
    x = jstr_word_find_zero_low (x);
  return jstr_word_index_last (x);
}

static JSTR_INLINE unsigned int
jstr_word_index_last_eq (jstr_word_ty x1, jstr_word_ty x2)
{
  return jstr_word_index_last_zero (x1 ^ x2);
}

#endif /* STRING_FZC_H */
