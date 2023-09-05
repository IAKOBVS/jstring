/* string-fzi.h -- zero byte indexes.  HPPA version.
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
#ifndef PJSTR_STRING_FZI_H
#define PJSTR_STRING_FZI_H 1

#include "jstr_macros.h"

#include "string-optype.h"
#include "string-fza.h"

_Static_assert (sizeof (pjstr_op_ty) == 4, "64-bit not supported");

static JSTR_INLINE unsigned int
pjstr_index_first (pjstr_op_ty c)
{
  unsigned int ret;
  /* Since we have no pjstr_clz insn, direct tests of the bytes is faster
     than loading up the constants to do the masking.  */
  asm ("extrw,u,= %1,23,8,%%r0\n\t"
       "ldi 2,%0\n\t"
       "extrw,u,= %1,15,8,%%r0\n\t"
       "ldi 1,%0\n\t"
       "extrw,u,= %1,7,8,%%r0\n\t"
       "ldi 0,%0"
       : "=r"(ret) : "r"(c), "0"(3));
  return ret;
}

static JSTR_INLINE unsigned int
pjstr_index_last (pjstr_op_ty c)
{
  unsigned int ret;
  /* Since we have no pjstr_ctz insn, direct tests of the bytes is faster
     than loading up the constants to do the masking.  */
  asm ("extrw,u,= %1,15,8,%%r0\n\t"
       "ldi 1,%0\n\t"
       "extrw,u,= %1,23,8,%%r0\n\t"
       "ldi 2,%0\n\t"
       "extrw,u,= %1,31,8,%%r0\n\t"
       "ldi 3,%0"
       : "=r"(ret) : "r"(c), "0"(0));
  return ret;
}

#endif /* _STRING_FZI_H */
