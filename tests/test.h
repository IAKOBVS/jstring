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

#ifndef JSTR_TEST_H
#define JSTR_TEST_H

#define __STRICT_ANSI__

#include "../src/jstr.h"
#include <assert.h>

#define PRINT(fmt, ...) \
	printf(fmt, __VA_ARGS__)

#define PRINTERR(fmt, ...) \
	fprintf(stdout, fmt, __VA_ARGS__)

#define SUCCESS() \
	PRINTERR("%s succeeded.\n", argv[0])

#define ASSERT(expr, result, expected)            \
	do {                                      \
		if (jstr_unlikely(!(expr))) {     \
			PRINTERR("result:%s\n"    \
				 "expected:%s\n", \
				 result,          \
				 expected);       \
			assert(expr);             \
		}                                 \
	} while (0)

#define TESTING(func) PRINTERR("Testing %s.\n", JSTR_STRING(func) "()")

#endif /* JSTR_TEST_H */
