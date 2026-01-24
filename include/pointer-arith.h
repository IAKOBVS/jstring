/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 * This file is part of the jstring library.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#ifndef JSTR_PTR_ARITH_H
#	define JSTR_PTR_ARITH_H 1

#	include "config.h"

#	define JSTR_ALIGN_DOWN(base, size)                          ((base) & -((size_t)(size)))
#	define JSTR_ALIGN_UP(base, size)                            JSTR_ALIGN_DOWN((base) + (size_t)(size) - 1, (size))
#	define JSTR_ALIGN_UP_STR(base)                              ((size_t)JSTR_ALIGN_UP(base, JSTR_MALLOC_ALIGNMENT))
#	define JSTR_ALIGN_DOWN_STR(base)                            ((size_t)JSTR_ALIGN_DOWN(base, JSTR_MALLOC_ALIGNMENT))
#	define JSTR_DIFF(p1, p2)                                    ((size_t)((uintptr_t)(p1) - (uintptr_t)(p2)))
#	define JSTR_PTR_TOPTR(p)                                    ((uintptr_t)p)
#	define JSTR_PTR_DIFF(p1, p2)                                ((ptrdiff_t)((uintptr_t)(p1) - (uintptr_t)(p2)))
#	define JSTR_PTR_IS_NOT_ALIGNED(base, size)                  (((uintptr_t)(base)) & ((size_t)(size) - 1))
#	define JSTR_PTR_IS_ALIGNED(base, size)                      (!(JSTR_PTR_IS_NOT_ALIGNED((uintptr_t)base, (size_t)(size))))
#	define JSTR_PTR_ALIGN_DOWN(base, size)                      (uintptr_t)JSTR_ALIGN_DOWN(((uintptr_t)base), (size_t)(size))
#	define JSTR_PTR_ALIGN_UP(base, size)                        (uintptr_t)JSTR_ALIGN_UP(((uintptr_t)base), (size_t)(size))
#	define JSTR_PTR_IS_ALIGNED_STR(base)                        JSTR_PTR_IS_ALIGNED((uintptr_t)base, JSTR_MALLOC_ALIGNMENT)
#	define JSTR_PTR_NOT_CROSSING_PAGE(ptr, obj_size, page_size) ((size_t)JSTR_DIFF(JSTR_PTR_ALIGN_UP(ptr, page_size), ptr) >= (size_t)obj_size)

#endif /* JSTR_POINTER_ARITH_H */
