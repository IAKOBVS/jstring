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

#include "../jstr/jarr.h"
#include "test.h"

#define ASZ(arr) (sizeof(arr) / sizeof(arr[0]))
#define T(func, ...)                                              \
	do {                                                      \
		int expected[] = { __VA_ARGS__ };                 \
		func;                                             \
		assert(j.data);                                   \
		assert(!memcmp(j.data, expected, ASZ(expected))); \
	} while (0)

int
main(int argc, char **argv)
{
	START();
	jarr_ty(int, j) = JARR_INIT;
	TESTING(jarr_cat);
	T(jarr_cat(&j, 1, 2, 3, 4, 5),
	  1,
	  2,
	  3,
	  4,
	  5);
	TESTING(jarr_pushback);
	T(jarr_pushback(&j, 6),
	  1,
	  2,
	  3,
	  4,
	  5,
	  6);
	TESTING(jarr_popback);
	T(jarr_popback(&j),
	  1,
	  2,
	  3,
	  4,
	  5);
	TESTING(jarr_popfront);
	T(jarr_popfront(&j),
	  2,
	  3,
	  4,
	  5);
	TESTING(jarr_pushfront);
	T(jarr_pushfront(&j, 1),
	  1,
	  2,
	  3,
	  4,
	  5);
	jarr_free(&j);
	SUCCESS();
	return 0;
}
