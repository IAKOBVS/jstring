/* Zero byte detection, boolean.  HPPA version.
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

#ifndef PJSTR_STRING_FZB_H
#define PJSTR_STRING_FZB_H 1

#include "jstr_macros.h"

#include <sys/cdefs.h>
#include "string-optype.h"

_Static_assert (sizeof (jstr_op_ty) == 4, "64-bit not supported");

/* Determine if any byte within X is zero.  This is a pure boolean test.  */
static JSTR_INLINE int
pjstr_has_zero (jstr_op_ty x)
{
  /* It's more useful to expose a control transfer to the compiler
     than to expose a proper boolean result.  */
  asm goto ("uxor,sbz %%r0,%0,%%r0\n\t"
	    "b,n %l1" : : "r"(x) : : nbz);
  return 1;
 nbz:
  return 0;
}

/* Likewise, but for byte equality between X1 and X2.  */
static JSTR_INLINE int
pjstr_has_eq (jstr_op_ty x1, jstr_op_ty x2)
{
  asm goto ("uxor,sbz %0,%1,%%r0\n\t"
	    "b,n %l2" : : "r"(x1), "r"(x2) : : nbz);
  return 1;
 nbz:
  return 0;
}

/* Likewise, but for zeros in X1 and equal bytes between X1 and X2.  */
static JSTR_INLINE int
pjstr_has_zero_eq (jstr_op_ty x1, jstr_op_ty x2)
{
  asm goto ("uxor,sbz %%r0,%0,%%r0\n\t"
	    "uxor,nbz %0,%1,%%r0\n\t"
	    "b,n %l2" : : "r"(x1), "r"(x2) : : sbz);
  return 0;
 sbz:
  return 1;
}

#endif /* PJSTR_STRING_FZB_H */
