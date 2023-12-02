/* Copyright (c) 2023 James Tirta Halim <tirtajames45 at gmail dot com>
   This file is part of the jstring library.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

   MIT License (Expat) */

#ifndef JSTR_CTYPE_H
#define JSTR_CTYPE_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <errno.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr-ctype-table.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

/*
   ASCII.
   Will NOT handle EOF correctly.
   toupper(EOF) != EOF;
*/
JSTR_ATTR_INLINE
JSTR_FUNC_CONST
static int
jstr_toupper(int c)
JSTR_NOEXCEPT
{
	return jstr_ctype_table_toupper[(unsigned char)c];
}

/*
   ASCII.
   Will NOT handle EOF correctly.
   tolower(EOF) != EOF;
*/
JSTR_ATTR_INLINE
JSTR_FUNC_CONST
static int
jstr_tolower(int c)
JSTR_NOEXCEPT
{
	return jstr_ctype_table_tolower[(unsigned char)c];
}

#define PJSTR_DEFINE_REPEAT_CTYPE(FUNC) \
	FUNC(alpha, JSTR_ISALPHA)       \
	FUNC(lower, JSTR_ISLOWER)       \
	FUNC(upper, JSTR_ISUPPER)       \
	FUNC(space, JSTR_ISSPACE)       \
	FUNC(blank, JSTR_ISBLANK)       \
	FUNC(digit, JSTR_ISDIGIT)       \
	FUNC(xdigit, JSTR_ISXDIGIT)     \
	FUNC(punct, JSTR_ISPUNCT)       \
	FUNC(graph, JSTR_ISGRAPH)       \
	FUNC(print, JSTR_ISPRINT)       \
	FUNC(cntrl, JSTR_ISCNTRL)       \
	FUNC(word, JSTR_ISWORD)

/*
   ASCII.
   For example: jstr_isctype(C, JSTR_ISLOWER | JSTR_ISDIGIT) checks if C is either lowercase or a digit.
*/
JSTR_ATTR_INLINE
JSTR_FUNC_CONST
static int
jstr_isctype(int c,
             int type)
JSTR_NOEXCEPT
{
	return jstr_ctype_table[(unsigned char)c] & type;
}

#define PJSTR_DEFINE_ISCTYPE(ctype, ctype_enum)                         \
	/* ASCII. */                                                    \
	JSTR_ATTR_INLINE                                                \
	JSTR_FUNC_CONST                                                 \
	static int                                                      \
	jstr_is##ctype(int c)                                           \
	JSTR_NOEXCEPT                                                   \
	{                                                               \
		return jstr_ctype_table[(unsigned char)c] & ctype_enum; \
	}

PJSTR_DEFINE_REPEAT_CTYPE(PJSTR_DEFINE_ISCTYPE)

#undef PJSTR_DEFINE_ISCTYPE

/* ASCII. */
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static char *
jstr_skipctype(const char *R s,
               int ctype)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s;
	if (jstr_unlikely(ctype & JSTR_ISCNTRL)) {
		for (; *p && jstr_ctype_table[*p] & ctype; ++p)
			;
		return (char *)p;
	}
	while (jstr_ctype_table[*p++] & ctype)
		;
	return (char *)p - 1;
}

#define PJSTR_DEFINE_SKIP_CTYPE(ctype, ctype_enum)    \
	/* ASCII. */                                  \
	JSTR_ATTR_INLINE                              \
	JSTR_FUNC_PURE                                \
	static char *                                 \
	jstr_skip##ctype(const char *R s)             \
	JSTR_NOEXCEPT                                 \
	{                                             \
		return jstr_skipctype(s, ctype_enum); \
	}

PJSTR_DEFINE_REPEAT_CTYPE(PJSTR_DEFINE_SKIP_CTYPE)

#undef PJSTR_DEFINE_SKIP_CTYPE

/* ASCII. */
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static int
jstr_isctypestr(const char *R s,
                int ctype)
JSTR_NOEXCEPT
{
	return jstr_likely(*s) && *(jstr_skipctype(s, ctype) + 1) == '\0';
}

#define PJSTR_DEFINE_ISCTYPE_STR(ctype, ctype_enum)    \
	/* ASCII. */                                   \
	JSTR_ATTR_INLINE                               \
	JSTR_FUNC_PURE                                 \
	static int                                     \
	jstr_is##ctype##str(const char *R s)           \
	JSTR_NOEXCEPT                                  \
	{                                              \
		return jstr_isctypestr(s, ctype_enum); \
	}

PJSTR_DEFINE_REPEAT_CTYPE(PJSTR_DEFINE_ISCTYPE_STR)

#undef PJSTR_DEFINE_ISCTYPE_STR

/* ASCII. */
JSTR_ATTR_INLINE
JSTR_FUNC_PURE
static char *
jstr_skipctype_rev(const char *const start,
                   const char *end,
                   const jstr_ctype_ty ctype)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)end;
	for (; (const unsigned char *)start != p && (jstr_ctype_table[*p] & ctype); --p)
		;
	return (char *)p;
}

#define PJSTR_DEFINE_SKIP_CTYPE_REV(ctype, ctype_enum)             \
	/* ASCII. */                                               \
	JSTR_ATTR_INLINE                                           \
	JSTR_FUNC_PURE                                             \
	static char *                                              \
	jstr_skip##ctype##_rev(const char *const start,            \
	                       const char *end)                    \
	JSTR_NOEXCEPT                                              \
	{                                                          \
		return jstr_skipctype_rev(start, end, ctype_enum); \
	}

PJSTR_DEFINE_REPEAT_CTYPE(PJSTR_DEFINE_SKIP_CTYPE_REV)

#undef PJSTR_DEFINE_SKIP_CTYPE_REV

/* ASCII. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstr_toupperstr_p(char *R s)
JSTR_NOEXCEPT
{
	for (; (*s = jstr_toupper(*s)); ++s)
		;
	return s;
}

/* ASCII. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstr_tolowerstr_p(char *R s)
JSTR_NOEXCEPT
{
	for (; (*s = jstr_tolower(*s)); ++s)
		;
	return s;
}

/*
   Copy SRC to DST touppered.
   Return pointer to '\0' in DST.
*/
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_toupperstrcpy_p(char *R dst,
                     const char *R src)
JSTR_NOEXCEPT
{
	while ((*dst++ = jstr_tolower(*src++)))
		;
	return dst - 1;
}

/*
   Copy SRC to DST tolowered.
   Return pointer to '\0' in DST.
*/
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_tolowerstrcpy_p(char *R dst,
                     const char *R src)
JSTR_NOEXCEPT
{
	while ((*dst++ = jstr_tolower(*src++)))
		;
	return dst - 1;
}

PJSTR_END_DECLS

#undef PJSTR_DEFINE_REPEAT_CTYPE
#undef R

#endif /* JSTR_CTYPE_H */
