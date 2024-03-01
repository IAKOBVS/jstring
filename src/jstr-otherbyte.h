/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2024 James Tirta Halim <tirtajames45 at gmail dot com>
 * This file is part of the jstring library.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#ifndef JSTR_OTHERBYTE_H
#define JSTR_OTHERBYTE_H

#include "jstr-ctype.h"
#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stddef.h>
PJSTR_END_DECLS

PJSTR_BEGIN_DECLS

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_otherbytefind_len(const void *s,
                       size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	const int c = *p;
	if (jstr_unlikely(c == 0))
		return (char *)s;
	for (; n && *p != c; ++p, --n) {}
	return (void *)p;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_otherbytefind(const char *s)
{
	const int c = *s;
	if (jstr_unlikely(c == 0))
		return (char *)s;
	const unsigned char *p = (const unsigned char *)s;
	for (; *p != c; ++p) {}
	return (char *)p;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_otherbytefindcase(const char *s)
{
	const int c = jstr_tolower(*s);
	if (jstr_unlikely(c == 0))
		return (char *)s;
	const unsigned char *p = (const unsigned char *)s;
	for (; jstr_tolower(*p) != c; ++p) {}
	return (char *)p;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_otherbytefindcase_len(const void *s,
                           size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	const int c = jstr_tolower(*p);
	if (jstr_unlikely(c == 0))
		return (char *)s;
	for (; n && jstr_tolower(*p) != c; ++p, --n) {}
	return (char *)p;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
jstr_otherbytefindnonalpha(const char *s)
{
	const unsigned char *p = (const unsigned char *)s;
	for (; jstr_isalpha(*p); ++p) {}
	return (char *)p;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static void *
jstr_otherbytefindnonalpha_len(const void *s,
                               size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	for (; n && jstr_isalpha(*p); ++p, --n) {}
	return (char *)p;
}

PJSTR_END_DECLS

#endif /* JSTR_OTHERBYTE_H */
