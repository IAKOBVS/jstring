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
#include "../src/jstr-itoa.h"

#define T(string, expected_result)                                                              \
	do {                                                                                    \
		char s[1024];                                                                   \
		memcpy(s, string, strlen(string) + 1);                                          \
		ASSERT(jstr_thousep_p(s, ',') == s + strlen(s), s, expected_result);            \
		ASSERT(!strcmp(s, expected_result), s, expected_result);                        \
		ASSERT(jstr_thousepcpy_p(s, string, ',') == s + strlen(s), s, expected_result); \
		ASSERT(!strcmp(s, expected_result), s, expected_result);                        \
	} while (0)

int
main(int argc, char **argv)
{
	START();
	TESTING(jstr_thousep_p);
	TESTING(jstr_thousepcpy_p);
	T("1", "1");
	T("12", "12");
	T("123", "123");
	T("1234", "1,234");
	T("12345", "12,345");
	T("123456", "123,456");
	T("1234567", "1,234,567");
	T("12345678", "12,345,678");
	T("123456789", "123,456,789");
	SUCCESS();
	return 0;
}
