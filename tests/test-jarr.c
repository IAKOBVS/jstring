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

#include "test.h"
#include "../src/jarr.h"

#define T_ZERO(func)                               \
	do {                                       \
		func;                              \
		ASSERT_ERRFUNC(func, j.data);      \
		ASSERT_ERRFUNC(func, j.size == 0); \
	} while (0)

#define T(func, ...)                                                               \
	do {                                                                       \
		T expected[] = { (int)__VA_ARGS__ };                               \
		func;                                                              \
		ASSERT_ERRFUNC(func, j.data);                                      \
		ASSERT_ERRFUNC(func, !memcmp(j.data, expected, sizeof(expected))); \
		ASSERT_ERRFUNC(func, j.size == JSTR_ARRAY_COUNT(expected));        \
	} while (0)

int
main(int argc, char **argv)
{
	START();
	typedef int T;
	jarr_ty(T, j) = JARR_INIT;
#ifndef __STRICT_ANSI__
	T(jarr_cat(&j, 1), 1);
#else
	T(jarr_pushback(&j, 1), 1);
#endif
	T_ZERO(jarr_popback(&j));
	T_ZERO(jarr_popback(&j));
	T_ZERO(jarr_popfront(&j));
	T_ZERO(jarr_popfront(&j));
	T(jarr_pushback(&j, 1), 1);
	T(jarr_pushback(&j, 2), 1, 2);
	T(jarr_popback(&j), 1);
	T(jarr_pushfront(&j, 2), 2, 1);
	T(jarr_popfront(&j), 1);
	T_ZERO(jarr_popfront(&j));
	jarr_free(&j);
	SUCCESS();
	return 0;
	(void)argc;
}
