/* Zero byte detection; indexes.  Generic C version.
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

#ifndef JSTR_STRING_FZI_H
#define JSTR_STRING_FZI_H 1

#include "jstr-macros.h"

#include <limits.h>
#include "_string-fza.h"

static JSTR_ATTR_MAYBE_UNUSED JSTR_ATTR_INLINE int
jstr_word_clz (jstr_word_ty c)
{
  if (sizeof (jstr_word_ty) == sizeof (unsigned long))
    return __builtin_clzl (c);
  else
    return __builtin_clzll (c);
}

static JSTR_ATTR_MAYBE_UNUSED JSTR_ATTR_INLINE int
jstr_word_ctz (jstr_word_ty c)
{
  if (sizeof (jstr_word_ty) == sizeof (unsigned long))
    return __builtin_ctzl (c);
  else
    return __builtin_ctzll (c);
}

/* A subroutine for the index_zero functions.  Given a test word C, return
   the (memory order) index of the first byte (in memory order) that is
   non-zero.  */
static JSTR_ATTR_MAYBE_UNUSED JSTR_ATTR_INLINE unsigned int
jstr_word_index_first (jstr_word_ty c)
{
  int r;
  if (JSTR_ENDIAN_LITTLE)
    r = jstr_word_ctz (c);
  else
    r = jstr_word_clz (c);
  return r / CHAR_BIT;
}

/* Similarly, but return the (memory order) index of the last byte that is
   non-zero.  */
static JSTR_ATTR_MAYBE_UNUSED JSTR_ATTR_INLINE unsigned int
jstr_word_index_last (jstr_word_ty c)
{
  int r;
  if (JSTR_ENDIAN_LITTLE)
    r = jstr_word_clz (c);
  else
    r = jstr_word_ctz (c);
  return sizeof (jstr_word_ty) - 1 - (r / CHAR_BIT);
}

#endif /* STRING_FZI_H */
