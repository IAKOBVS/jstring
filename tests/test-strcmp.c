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
#include "test-array.h"
#include "../include/string.h"

int
main(int argc, char **argv)
{
	START();
	T_MEMCMP(!jstr_memcmpeq_loop, !memcmp, test_array_memcmp);
	T_MEMCMP(jstr_strncasecmp, simple_strncasecmp, test_array_memcmp);
	T_MEMCMP(!jstr_strcasecmpeq_len, !simple_strncasecmp, test_array_memcmp);
	T_MEMCMP(!jstr_strcasecmpeq_len_loop, !simple_strncasecmp, test_array_memcmp);
	T_STRCMP(jstr_strcasecmp, simple_strcasecmp, test_array_memcmp);
	T_STRCMP(!jstr_strcasecmpeq, !simple_strcasecmp, test_array_memcmp);
	T_STRCMP(!jstr_strcasecmpeq_loop, !simple_strcasecmp, test_array_memcmp);
	SUCCESS();
	return EXIT_SUCCESS;
}
