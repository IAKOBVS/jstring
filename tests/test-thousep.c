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

#include "test.h"
#include "../include/itoa.h"

char buf[1024];

void
T(const char *string, const char *expected_result)
{
	memcpy(buf, string, strlen(string) + 1);
	ASSERT_RESULT(jstr_thousep_p, jstr_thousep_p(buf, ',') == buf + strlen(buf), buf, expected_result);
	ASSERT_RESULT(jstr_thousep_p, !strcmp(buf, expected_result), buf, expected_result);
	ASSERT_RESULT(jstr_thousepcpy_p, jstr_thousepcpy_p(buf, string, ',') == buf + strlen(buf), buf, expected_result);
	ASSERT_RESULT(jstr_thousepcpy_p, !strcmp(buf, expected_result), buf, expected_result);
}

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
	return EXIT_SUCCESS;
}
