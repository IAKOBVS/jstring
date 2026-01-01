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

#include "_musl-ctype.h"

#ifndef JSTR_CTYPE_H
#	define JSTR_CTYPE_H 1

#	include "jstr-macros.h"

#	define R JSTR_RESTRICT

JSTR__BEGIN_DECLS

/* ASCII. */
JSTR_FUNC_VOID
static char *
jstr_toupperstr_p(char *s)
JSTR_NOEXCEPT
{
	unsigned char *p = (unsigned char *)s;
	for (; (*p = jstr_toupper(*p)); ++p) {}
	return (char *)p;
}

/* ASCII. */
JSTR_FUNC_VOID
static char *
jstr_tolowerstr_p(char *s)
JSTR_NOEXCEPT
{
	unsigned char *p = (unsigned char *)s;
	for (; (*p = jstr_tolower(*p)); ++p) {}
	return (char *)p;
}

/* Auto-vectorized at -O2 by GCC >= 15.1 and Clang >= 3.6. */
/* ASCII. */
JSTR_FUNC_VOID
static void
jstr_tolowerstr_len(char *s, size_t n)
JSTR_NOEXCEPT
{
	unsigned char *p = (unsigned char *)s;
	for (; n--; ++p)
		*p = jstr_tolower(*p);
}

/* Auto-vectorized at -O2 by GCC >= 15.1 and Clang >= 3.6. */
/* ASCII. */
JSTR_FUNC_VOID
static void
jstr_tolowerstrcpy_len(char *R dst, char *R src, size_t n)
JSTR_NOEXCEPT
{
	if (n) {
		unsigned char *d = (unsigned char *)dst;
		const unsigned char *s = (unsigned char *)src;
		while (n--)
			*d++ = jstr_tolower(*s++);
		*d = '\0';
	}
}

/* Auto-vectorized at -O2 by GCC >= 15.1 and Clang >= 3.6. */
/* ASCII. */
JSTR_FUNC_VOID
static void
jstr_toupperstr_len(char *s, size_t n)
JSTR_NOEXCEPT
{
	unsigned char *p = (unsigned char *)s;
	for (; n--; ++p)
		*p = jstr_toupper(*p);
}

/* Auto-vectorized at -O2 by GCC >= 15.1 and Clang >= 3.6. */
/* ASCII. */
JSTR_FUNC_VOID
static void
jstr_toupperstrcpy_len(char *R dst, char *R src, size_t n)
JSTR_NOEXCEPT
{
	if (n) {
		unsigned char *d = (unsigned char *)dst;
		const unsigned char *s = (unsigned char *)src;
		while (n--)
			*d++ = jstr_toupper(*s++);
		*d = '\0';
	}
}

/* Copy SRC to DST touppered.
 * Return ptr to '\0' in DST. */
JSTR_FUNC
static char *
jstr_toupperstrcpy_p(char *R dst, const char *R src)
JSTR_NOEXCEPT
{
	unsigned char *d = (unsigned char *)dst;
	const unsigned char *s = (const unsigned char *)src;
	while ((*d++ = jstr_tolower(*s++))) {}
	return (char *)d - 1;
}

/* Copy SRC to DST tolowered.
 * Return ptr to '\0' in DST. */
JSTR_FUNC
static char *
jstr_tolowerstrcpy_p(char *R dst, const char *R src)
JSTR_NOEXCEPT
{
	unsigned char *d = (unsigned char *)dst;
	const unsigned char *s = (const unsigned char *)src;
	while ((*d++ = jstr_tolower(*s++))) {}
	return (char *)d - 1;
}

JSTR__END_DECLS

#	undef JSTR__DEFINE_REPEAT_CTYPE
#	undef R

#endif /* JSTR_CTYPE_H */
