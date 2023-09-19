/* string-fzi.h -- zero byte detection; indices.  Alpha version.
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
#ifndef P_JSTR_STRING_FZI_H
#define P_JSTR_STRING_FZI_H
#include "jstr-macros.h"
#include <stdint.h>
#include "string-optype.h"
#include "string-fza.h"
/* Note that since CMPBGE creates a bit mask rather than a byte mask,
   we cannot simply provide a target-specific string-fza.h.  */
/* A subroutine for the jstr_word_index_zero functions.  Given a bitmask C,
   return the index of the first bit set in memory order.  */
static JSTR_INLINE unsigned int
jstr_word_index_first (jstr_word_ty c)
{
#ifdef __alpha_cix__
  return __builtin_ctzl (c);
#else
  c = c & -c;
  return (c & 0xf0 ? 4 : 0) + (c & 0xcc ? 2 : 0) + (c & 0xaa ? 1 : 0);
#endif
}
/* Similarly, but return the (memory order) index of the last bit
   that is non-zero.  Note that only the least 8 bits may be nonzero.  */
static JSTR_INLINE unsigned int
jstr_word_index_last (jstr_word_ty x)
{
#ifdef __alpha_cix__
  return __builtin_clzl (x) ^ 63;
#else
  unsigned r = 0;
  if (x & 0xf0)
    r += 4;
  if (x & (0xc << r))
    r += 2;
  if (x & (0x2 << r))
    r += 1;
  return r;
#endif
}
#endif /* _STRING_FZI_H */
