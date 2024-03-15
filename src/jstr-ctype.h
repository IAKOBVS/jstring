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

#ifndef JSTR_CTYPE_H
#define JSTR_CTYPE_H 1

#include "jstr-macros.h"

JSTR__BEGIN_DECLS
#include <errno.h>
#include <string.h>
JSTR__END_DECLS

#include "jstr-ctype-table.h"

#define R JSTR_RESTRICT

JSTR__BEGIN_DECLS

/* Will not handle EOF correctly.
 * tolower(EOF) != EOF */
#define jstr_toupper(c) ((int)jstr_ctype_table_toupper[(unsigned char)c])
#define jstr_tolower(c) ((int)jstr_ctype_table_tolower[(unsigned char)c])

#define jstr_isctype(c, ctype)                                                 \
	((int)(jstr_ctype_table[(unsigned char)c] & (ctype)))
#define jstr_isalpha(c)  jstr_isctype(c, JSTR_ISALPHA)
#define jstr_islower(c)  jstr_isctype(c, JSTR_ISLOWER)
#define jstr_isupper(c)  jstr_isctype(c, JSTR_ISUPPER)
#define jstr_isspace(c)  jstr_isctype(c, JSTR_ISSPACE)
#define jstr_isblank(c)  jstr_isctype(c, JSTR_ISBLANK)
#define jstr_isdigit(c)  jstr_isctype(c, JSTR_ISDIGIT)
#define jstr_isxdigit(c) jstr_isctype(c, JSTR_ISXDIGIT)
#define jstr_ispunct(c)  jstr_isctype(c, JSTR_ISPUNCT)
#define jstr_isgraph(c)  jstr_isctype(c, JSTR_ISGRAPH)
#define jstr_isprint(c)  jstr_isctype(c, JSTR_ISPRINT)
#define jstr_iscntrl(c)  jstr_isctype(c, JSTR_ISCNTRL)
#define jstr_isword(c)   jstr_isctype(c, JSTR_ISWORD)

/* ASCII. */
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static char *
jstr_skipctype(const char *R s, int ctype) JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s;
	if (jstr_unlikely(ctype & JSTR_ISCNTRL)) {
		for (; *p && jstr_isctype(*p, ctype); ++p) {}
		return (char *)p;
	}
	while (jstr_isctype(*p++, ctype)) {}
	return (char *)p - 1;
}

#define JSTR__DEFINE_REPEAT_CTYPE(FUNC)                                        \
	FUNC(alpha, JSTR_ISALPHA)                                              \
	FUNC(lower, JSTR_ISLOWER)                                              \
	FUNC(upper, JSTR_ISUPPER)                                              \
	FUNC(space, JSTR_ISSPACE)                                              \
	FUNC(blank, JSTR_ISBLANK)                                              \
	FUNC(digit, JSTR_ISDIGIT)                                              \
	FUNC(xdigit, JSTR_ISXDIGIT)                                            \
	FUNC(punct, JSTR_ISPUNCT)                                              \
	FUNC(graph, JSTR_ISGRAPH)                                              \
	FUNC(print, JSTR_ISPRINT)                                              \
	FUNC(cntrl, JSTR_ISCNTRL)                                              \
	FUNC(word, JSTR_ISWORD)

#define JSTR__DEFINE_SKIP_CTYPE(ctype, ctype_enum)                             \
	/* ASCII. */                                                           \
	JSTR_ATTR_INLINE                                                       \
	JSTR_FUNC_PURE                                                         \
	static char *jstr_skip##ctype(const char *R s) JSTR_NOEXCEPT           \
	{                                                                      \
		return jstr_skipctype(s, ctype_enum);                          \
	}

JSTR__DEFINE_REPEAT_CTYPE(JSTR__DEFINE_SKIP_CTYPE)

#undef JSTR__DEFINE_SKIP_CTYPE

/* ASCII. */
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isctypestr(const char *R s, int ctype) JSTR_NOEXCEPT
{
	return jstr_likely(*s) && *(jstr_skipctype(s, ctype) + 1) == '\0';
}

#define JSTR__DEFINE_ISCTYPE_STR(ctype, ctype_enum)                            \
	/* ASCII. */                                                           \
	JSTR_ATTR_INLINE                                                       \
	JSTR_FUNC_PURE                                                         \
	static int jstr_is##ctype##str(const char *R s) JSTR_NOEXCEPT          \
	{                                                                      \
		return jstr_isctypestr(s, ctype_enum);                         \
	}

JSTR__DEFINE_REPEAT_CTYPE(JSTR__DEFINE_ISCTYPE_STR)

#undef JSTR__DEFINE_ISCTYPE_STR

/* ASCII. */
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static char *
jstr_skipctype_rev(const char *s,
                   const jstr_ctype_ty ctype,
                   size_t n) JSTR_NOEXCEPT
{
	if (jstr_unlikely(n == 0))
		return (char *)s;
	const unsigned char *p = (const unsigned char *)s + n - 1;
	for (; n-- && jstr_isctype(*p, ctype); --p) {}
	return (char *)p;
}

#define JSTR__DEFINE_SKIP_CTYPE_REV(ctype, ctype_enum)                         \
	/* ASCII. */                                                           \
	JSTR_ATTR_INLINE                                                       \
	JSTR_FUNC_PURE                                                         \
	static char *jstr_skip##ctype##_rev(const char *s, size_t n)           \
	JSTR_NOEXCEPT                                                          \
	{                                                                      \
		return jstr_skipctype_rev(s, ctype_enum, n);                   \
	}

JSTR__DEFINE_REPEAT_CTYPE(JSTR__DEFINE_SKIP_CTYPE_REV)

#undef JSTR__DEFINE_SKIP_CTYPE_REV

/* ASCII. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstr_toupperstr_p(char *R s) JSTR_NOEXCEPT
{
	unsigned char *p = (unsigned char *)s;
	for (; (*p = jstr_toupper(*p)); ++p) {}
	return (char *)p;
}

/* ASCII. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstr_tolowerstr_p(char *R s) JSTR_NOEXCEPT
{
	unsigned char *p = (unsigned char *)s;
	for (; (*p = jstr_tolower(*p)); ++p) {}
	return (char *)p;
}

/* Copy SRC to DST touppered.
 * Return pointer to '\0' in DST. */
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_toupperstrcpy_p(char *R dst, const char *R src) JSTR_NOEXCEPT
{
	const unsigned char *s = (const unsigned char *)src;
	while ((*dst++ = (char)jstr_tolower(*s++))) {}
	return dst - 1;
}

/* Copy SRC to DST tolowered.
 * Return pointer to '\0' in DST. */
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_tolowerstrcpy_p(char *R dst, const char *R src) JSTR_NOEXCEPT
{
	const unsigned char *s = (const unsigned char *)src;
	while ((*dst++ = (char)jstr_tolower(*s++))) {}
	return dst - 1;
}

JSTR__END_DECLS

#undef JSTR__DEFINE_REPEAT_CTYPE
#undef R

#endif /* JSTR_CTYPE_H */
