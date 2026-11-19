/* SPDX-License-Identifier: MIT */
/* This file is part of the jstring library, and a modified version
 * of functional/strchr.c taken from musl's libc-test
 *
 * libc-test is licensed under the following standard MIT license:
 *
 * Copyright © 2005-2013 libc-test AUTHORS
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

#include "test.h"
#include "../include/jstr.h"

static JSTR_ATTR_MAYBE_UNUSED char buf[512];

static JSTR_ATTR_MAYBE_UNUSED void *
aligned(void *p)
{
	return (void *)(((uintptr_t)p + 63) & (unsigned long)-64);
}

static JSTR_ATTR_MAYBE_UNUSED void *
aligncpy(const void *p, size_t len, size_t a)
{
	return memcpy((char *)aligned(buf) + a, p, len);
}

static void
check_ptr(const char *name, const char *got, const char *want)
{
	t.result = got;
	t.expected = want;
	t_check(name, got == want);
}

static void
check_size(const char *name, size_t got, size_t want)
{
	t.result_n = got;
	t.expected_n = want;
	t_check(name, got == want);
}

void
T_ALL(const char *s, int c)
{
	size_t align;
	for (align = 0; align < 8; align++) {
		const char *p = (const char *)aligncpy(s, strlen(s), (size_t)align);
		t_init();
		t.hs = p;
		t.n = strlen(p);
		check_ptr("jstr_strncasechr", jstr_strncasechr(p, c, t.n), simple_strncasechr(p, c, t.n));
		check_ptr("jstr_strnchr", jstr_strnchr(p, c, t.n), simple_strnchr(p, c, t.n));
		check_ptr("jstr_memrchr", jstr_memrchr(p, c, t.n), simple_memrchr(p, c, t.n));
		check_ptr("jstr_memcasechr", jstr_memcasechr(p, c, t.n), simple_memcasechr(p, c, t.n));
		check_ptr("jstr_strchrnul", jstr_strchrnul(p, c), simple_strchrnul(p, c));
		check_ptr("jstr_strcasechr", jstr_strcasechr(p, c), simple_strcasechr(p, c));
		check_ptr("jstr_strcasechrnul", jstr_strcasechrnul(p, c), simple_strcasechrnul(p, c));
		check_size("jstr_countchr", jstr_countchr(p, c), simple_countchr(p, c));
		check_size("jstr_countchr_len", jstr_countchr_len(p, c, t.n), simple_countchr_len(p, c, t.n));
	}
}

void
T_ALL_SPN(const char *s, const char *a)
{
	size_t align;
	for (align = 0; align < 8; align++) {
		const char *p = (const char *)aligncpy(s, strlen(s), (size_t)align);
		t_init();
		t.hs = p;
		t.n = strlen(p);
		check_size("jstr_strrspn_len", jstr_strrspn_len(p, a, t.n), simple_memrspn(p, a, t.n));
		check_size("jstr_strrcspn_len", jstr_strrcspn_len(p, a, t.n), simple_memrcspn(p, a, t.n));
	}
}

int
main(void)
{
	int i;
	char a[128];
	char s[256];

	for (i = 0; i < 128; i++)
		a[i] = (i + 1) & 127;
	for (i = 0; i < 256; i++)
		*((unsigned char *)s + i) = i + 1;

	T_ALL("\0aaa", 'a');
	T_ALL("a\0bb", 'b');
	T_ALL("ab\0c", 'c');
	T_ALL("abc\0d", 'd');
	T_ALL("abc abc\0x", 'x');

	T_ALL("\0aaa", 'b');
	T_ALL("a\0bb", 'c');
	T_ALL("ab\0c", 'c');
	T_ALL("abc\0d", '\0');
	T_ALL("abc abc\0x", 'x');

	T_ALL("\0AAA", 'A');
	T_ALL("A\0bb", 'b');
	T_ALL("Ab\0c", 'c');
	T_ALL("Abc\0d", 'd');
	T_ALL("Abc Abc\0x", 'x');

	T_ALL(a, 128);
	T_ALL(a, 255);

	T_ALL("", 0);
	T_ALL("a", 'a');
	T_ALL("a", 'a' + 256);
	T_ALL("a", 0);
	T_ALL("abb", 'b');
	T_ALL("aabb", 'b');
	T_ALL("aaabb", 'b');
	T_ALL("aaaabb", 'b');
	T_ALL("aaaaabb", 'b');
	T_ALL("aaaaaabb", 'b');
	T_ALL("abc abc", 'c');

	T_ALL("", 0);
	T_ALL("A", 'A');
	T_ALL("A", 'A' + 256);
	T_ALL("A", 0);
	T_ALL("Abb", 'b');
	T_ALL("AAbb", 'b');
	T_ALL("AAAbb", 'b');
	T_ALL("AAAAbb", 'b');
	T_ALL("AAAAAbb", 'b');
	T_ALL("AAAAAAbb", 'b');
	T_ALL("Abc Abc", 'c');

	T_ALL(s, 1);
	T_ALL(s, 2);
	T_ALL(s, 10);
	T_ALL(s, 11);
	T_ALL(s, 127);
	T_ALL(s, 128);
	T_ALL(s, 255);
	T_ALL(s, 0);

	T_ALL_SPN("hello", "h");
	T_ALL_SPN("hello", "he");
	T_ALL_SPN("hello", "hel");
	T_ALL_SPN("hello", "hell");
	T_ALL_SPN("hello", "hello");

	T_ALL_SPN("hello", "e");
	T_ALL_SPN("hello", "el");
	T_ALL_SPN("hello", "ell");
	T_ALL_SPN("hello", "ello");

	T_ALL_SPN("hello", "l");
	T_ALL_SPN("hello", "ll");
	T_ALL_SPN("hello", "llo");

	T_ALL_SPN("hello", "l");
	T_ALL_SPN("hello", "ll");
	T_ALL_SPN("hello", "llo");

	T_ALL_SPN("hello", "l");
	T_ALL_SPN("hello", "lo");

	T_ALL_SPN("hello", "o");

	T_ALL_SPN("hello", "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ");

	return EXIT_SUCCESS;
}
