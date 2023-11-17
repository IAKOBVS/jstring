/* Helper macros for pointer arithmetic.
   Copyright (C) 2012-2023 Free Software Foundation, Inc.
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
   <https://www.gnu.org/licenses/>.  */
#ifndef PJSTR_LIBC_POINTER_ARITH_H
#define PJSTR_LIBC_POINTER_ARITH_H 1

#include <stdint.h>

/* Cast an integer VAL to void * ptr.  */
#define JSTR_CAST_TO_PTR(val) \
	((void *)(uintptr_t)(val))

/* Return the uintptr_t difference between P1 and P2 where P1 >= P2.  */
#define JSTR_PTR_DIFF(p1, p2) \
	((uintptr_t)((uintptr_t)(p1) - (uintptr_t)(p2)))

/* Check if BASE is aligned on SIZE  */
#define JSTR_PTR_IS_ALIGNED(base, size) \
	((((uintptr_t)(base)) & ((size)-1)) == 0)

/* Align a value by rounding down to closest size.
   e.g. Using size of 4096, we get this behavior:
        {4095, 4096, 4097} = {0, 4096, 4096}.  */
#define JSTR_ALIGN_DOWN(base, size) \
	((base) & -((uintptr_t)(size)))

/* Align a value by rounding up to closest size.
   e.g. Using size of 4096, we get this behavior:
        {4095, 4096, 4097} = {4096, 4096, 8192}.
  Note: The size argument has side effects (expanded multiple times).  */
#define JSTR_ALIGN_UP(base, size) \
	JSTR_ALIGN_DOWN((base) + (size)-1, (size))

/* Same as ALIGN_DOWN(), but automatically casts when base is a ptr.  */
#define JSTR_PTR_ALIGN_DOWN(base, size) \
	(JSTR_ALIGN_DOWN((uintptr_t)(base), (size)))

/* Same as ALIGN_UP(), but automatically casts when base is a ptr.  */
#define JSTR_PTR_ALIGN_UP(base, size) \
	(JSTR_ALIGN_UP((uintptr_t)(base), (size)))

#endif /* PJSTR_LIBC_POINTER_ARITH_H */
