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

#ifndef JSTRIO_TABLE_H
#define JSTRIO_TABLE_H 1

#include "jstr-macros.h"

JSTR_MAYBE_UNUSED
static const unsigned char pjstrio_binary_table[256] = {
	1, /* 0 */
	1, /* 1 */
	1, /* 2 */
	1, /* 3 */
	1, /* 4 */
	1, /* 5 */
	1, /* 6 */
	1, /* 7 */
	1, /* 8 */
	0, /* 9 */
	0, /* 10 */
	1, /* 11 */
	1, /* 12 */
#if JSTR_OS_WINDOWS || JSTR_OS_WINDOWSCE
	/* Windows uses \r\n as newline. */
	0, /* 13 */
#else
	1, /* 13 */
#endif
	1, /* 14 */
	1, /* 15 */
	1, /* 16 */
	1, /* 17 */
	1, /* 18 */
	1, /* 19 */
	1, /* 20 */
	1, /* 21 */
	1, /* 22 */
	1, /* 23 */
	1, /* 24 */
	1, /* 25 */
	1, /* 26 */
	1, /* 27 */
	1, /* 28 */
	1, /* 29 */
	1, /* 30 */
	1 /* 31 */
};

#endif /* JSTRIO_TABLE_H */
