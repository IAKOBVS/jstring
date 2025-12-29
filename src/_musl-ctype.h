/* SPDX-License-Identifier: MIT */
/* Taken from or based on musl's ctype functions with modifications.
 * Copyright © 2005-2020 Rich Felker, et al.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#ifndef MUSL_CTYPE_H
#	define MUSL_CTYPE_H 1

#	include "jstr-macros.h"

JSTR__BEGIN_DECLS

#	define ISALPHA(c) (((unsigned)c | 32) - 'a' < 26)
#	define ISDIGIT(c) ((unsigned)c - '0' < 10)
#	define ISGRAPH(c) ((unsigned)c - 0x21 < 0x5e)
#	define ISASCII(c) (!(c & ~0x7f))
#	define ISBLANK(c) ((c == ' ') | (c == '\t'))
#	define ISCNTRL(c) ((unsigned)c < 0x20 || c == 0x7f)
#	define ISLOWER(c) ((unsigned)c - 'a' < 26)
#	define ISUPPER(c) ((unsigned)c - 'A' < 26)
#	define ISPRINT(c) ((unsigned)c - 0x20 < 0x5f)
#	define ISALNUM(c) (ISALPHA(c) | ISDIGIT(c))
#	define ISPUNCT(c) (ISGRAPH(c) ^ ISALNUM(c))
#	define ISSPACE(c) ((c == ' ') | ((unsigned)c - '\t' < 5))
#	define TOLOWER(c) (c | (ISUPPER(c) ? 32 : 0))
#	define TOUPPER(c) (c & (ISLOWER(c) ? 0x5f : (int)-1))
#	define ISXDIGIT(c) (ISDIGIT(c) | (((unsigned)c|32)-'a' < 6))

JSTR_ATTR_INLINE
JSTR_ATTR_CONST
int
jstr_isalpha(int c)
{
	return ISALPHA((unsigned char)c);
}

JSTR_ATTR_INLINE
JSTR_ATTR_CONST
int
jstr_isdigit(int c)
{
	return ISDIGIT((unsigned char)c);
}

JSTR_ATTR_INLINE
JSTR_ATTR_CONST
int
jstr_isalnum(int c)
{
	return ISALNUM((unsigned char)c);
}

JSTR_ATTR_INLINE
JSTR_ATTR_CONST
int
jstr_isascii(int c)
{
	return ISASCII((unsigned char)c);
}

JSTR_ATTR_INLINE
JSTR_ATTR_CONST
int
jstr_isblank(int c)
{
	return ISBLANK((unsigned char)c);
}

JSTR_ATTR_INLINE
JSTR_ATTR_CONST
int
jstr_iscntrl(int c)
{
	return ISCNTRL((unsigned char)c);
}

JSTR_ATTR_INLINE
JSTR_ATTR_CONST
int
jstr_isgraph(int c)
{
	return ISGRAPH((unsigned char)c);
}

JSTR_ATTR_INLINE
JSTR_ATTR_CONST
int
jstr_islower(int c)
{
	return ISLOWER((unsigned char)c);
}

JSTR_ATTR_INLINE
JSTR_ATTR_CONST
int
jstr_isupper(int c)
{
	return ISUPPER((unsigned char)c);
}

JSTR_ATTR_INLINE
JSTR_ATTR_CONST
int
jstr_isprint(int c)
{
	return ISPRINT((unsigned char)c);
}

JSTR_ATTR_INLINE
JSTR_ATTR_CONST
int
jstr_ispunct(int c)
{
	return ISPUNCT((unsigned char)c);
}

JSTR_ATTR_INLINE
JSTR_ATTR_CONST
int
jstr_isspace(int c)
{
	return ISSPACE((unsigned char)c);
}

JSTR_ATTR_INLINE
JSTR_ATTR_CONST
int
jstr_isxdigit(int c)
{
	return ISXDIGIT((unsigned char)c);
}

JSTR_ATTR_INLINE
JSTR_ATTR_CONST
int
jstr_tolower(int c)
{
	return TOLOWER((unsigned char)c);
}

JSTR_ATTR_INLINE
JSTR_ATTR_CONST
int
jstr_toupper(int c)
{
	return TOUPPER((unsigned char)c);
}

#	undef ISALPHA
#	undef ISDIGIT
#	undef ISGRAPH
#	undef ISASCII
#	undef ISBLANK
#	undef ISCNTRL
#	undef ISLOWER
#	undef ISUPPER
#	undef ISPRINT
#	undef ISALNUM
#	undef ISPUNCT
#	undef ISSPACE
#	undef TOLOWER
#	undef TOUPPER
#	undef ISXDIGIT

JSTR__END_DECLS

#endif /* MUSL_CTYPE_H */
