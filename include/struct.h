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

#ifndef JSTR_STRUCT_H
#	define JSTR_STRUCT_H 1

#	include "macros.h"

JSTR_INTERNAL_BEGIN_DECLS
#	include <stddef.h>
JSTR_INTERNAL_END_DECLS

#if defined(JSTR_IMPLEMENTATION)
#	undef JSTR_IMPLEMENTATION
#	include "internal/musl/twoway.h"
#	define JSTR_IMPLEMENTATION 1
#else
#	include "internal/musl/twoway.h"
#endif

#	define jstr_struct(j)  &(j)->data, &(j)->size, &(j)->capacity
#	define jstr_literal(s) (s), (sizeof(s) - 1)
#	define jstr_literal_init(s)    \
		{                       \
			jstr_literal(s) \
		}

JSTR_INTERNAL_BEGIN_DECLS
typedef struct jstr_ty {
	char *data;
	size_t size;
	size_t capacity;
} jstr_ty;

typedef struct jstr_literal_ty {
	const char *data;
	const unsigned int size;
} jstr_literal_ty;

typedef struct jstr_twoway_ty {
	char _buf[4096];
} jstr_twoway_ty;

static
JSTR_ATTR_INLINE
void jstr_twoway_set_len(jstr_twoway_ty *t, size_t len)
{
	((jstr_internal_twoway_ty *)t)->needle_len = len;
}

static
JSTR_ATTR_INLINE
size_t jstr_twoway_get_len(const jstr_twoway_ty *t)
{
	return ((jstr_internal_twoway_ty *)t)->needle_len;
}

JSTR_INTERNAL_END_DECLS

#endif /* JSTR_STRUCT_H */
