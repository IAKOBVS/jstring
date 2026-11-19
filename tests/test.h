/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#ifndef JSTR_TEST_H
#define JSTR_TEST_H

#define JSTR_TEST 1
/* Force use of slower simple functions. */
#ifndef JSTR_USE_SIMPLE
#	define JSTR_USE_SIMPLE 0
#endif
/* Do more tests. */
#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

#include <ctype.h>
#include <stddef.h>
#include <string.h>

#include "../include/jstr.h"
#include "simple.h"
#include "test-types.h"
#include "testutil.h"

typedef struct test_ty {
	const char *hs;
	size_t hs_len;
	const char *ne;
	size_t ne_len;
	const char *s1;
	size_t s1_len;
	const char *s2;
	size_t s2_len;
	const char *result;
	size_t result_len;
	const char *expected;
	size_t expected_len;
	size_t n;
	size_t result_n;
	size_t expected_n;
	const char *src;
	size_t src_len;
	const char *dst;
	size_t dst_len;
} test_ty;
static test_ty t;

JSTR_ATTR_MAYBE_UNUSED static void
t_debug(void)
{
	if (!t.hs_len && t.hs)
		t.hs_len = strlen(t.hs);
	if (!t.ne_len && t.ne)
		t.ne_len = strlen(t.ne);
	if (!t.result_len && t.result)
		t.result_len = strlen(t.result);
	if (!t.expected_len && t.expected)
		t.expected_len = strlen(t.expected);
	fprintf(stderr, "t.hs:%s\n", test_str(t.hs));
	fprintf(stderr, "t.hs_len:%zu\n", t.hs_len);
	fprintf(stderr, "t.ne:%s\n", test_str(t.ne));
	fprintf(stderr, "t.ne_len:%zu\n", t.ne_len);
	fprintf(stderr, "t.n:%zu\n", t.n);
	fprintf(stderr, "t.result_n:%zu\n", t.result_n);
	fprintf(stderr, "t.expected_n:%zu\n", t.expected_n);
	fprintf(stderr, "t.result:%s\n", test_str(t.result));
	fprintf(stderr, "t.result_len:%zu\n", t.result_len);
	fprintf(stderr, "t.expected:%s\n", test_str(t.expected));
	fprintf(stderr, "t.expected_len:%zu\n", t.expected_len);
	fprintf(stderr, "t.s1:%s\n", test_str(t.s1));
	fprintf(stderr, "t.s1_len:%zu\n", t.s1_len);
	fprintf(stderr, "t.s2:%s\n", test_str(t.s2));
	fprintf(stderr, "t.s2_len:%zu\n", t.s2_len);
}

JSTR_ATTR_MAYBE_UNUSED static void
t_init(void)
{
	memset(&t, 0, sizeof(t));
}

JSTR_ATTR_MAYBE_UNUSED static void
t_check(const char *name, int cond)
{
	if (!cond) {
		fprintf(stderr, "%s\n", name);
		t_debug();
		assert(cond);
	}
}

typedef char *(*t_strcpy_fn)(char *, const char *);
typedef char *(*t_strstr_fn)(const char *, const char *);
typedef void *(*t_memmem_fn)(const void *, size_t, const void *, size_t);
typedef char *(*t_strnstr_fn)(const char *, const char *, size_t);
typedef int (*t_memcmp_fn)(const void *, const void *, size_t);
typedef int (*t_strcmp_fn)(const char *, const char *);

JSTR_ATTR_MAYBE_UNUSED static void
t_strcpy(const char *name, t_strcpy_fn fn, t_strcpy_fn simple_fn, const struct test_array_memmem *test_array, size_t count)
{
	char buf_r[4096];
	char buf_e[4096];
	for (size_t i = 0; i < count; ++i) {
		t_init();
		t.src = test_array[i].hs;
		t.src_len = 0;
		for (size_t ne_i = 0; t.src_len != (size_t)-1 && ne_i < 32; ++t.src, --t.src_len, ++ne_i) {
			t.expected_len = JSTR_PTR_DIFF(fn(buf_r, t.src), buf_r);
			t.result_len = JSTR_PTR_DIFF(simple_fn(buf_e, t.src), buf_e);
			t_check(name, !strcmp(buf_e, buf_r));
			t_check(name, t.result_len == t.expected_len);
			if (!JSTR_TEST_SLOW)
				break;
		}
	}
}

JSTR_ATTR_MAYBE_UNUSED static void
t_strstr(const char *name, t_strstr_fn fn, t_strstr_fn simple_fn, const struct test_array_memmem *test_array, size_t count)
{
	for (size_t i = 0; i < count; ++i) {
		t_init();
		t.hs = test_array[i].hs;
		t.ne = test_array[i].ne;
		t.result = fn(t.hs, t.ne);
		t.expected = simple_fn(t.hs, t.ne);
		t_check(name, t.result == t.expected);
	}
}

JSTR_ATTR_MAYBE_UNUSED static void
t_memmem(const char *name, t_memmem_fn fn, t_memmem_fn simple_fn, const struct test_array_memmem *test_array, size_t count)
{
	for (size_t i = 0; i < count; ++i) {
		t_init();
		t.hs = test_array[i].hs;
		t.ne = test_array[i].ne;
		t.hs_len = strlen(t.hs);
		t.ne_len = strlen(t.ne);
		for (size_t ne_i = 0; t.ne_len != (size_t)-1 && ne_i < 32; ++t.ne, --t.ne_len, ++ne_i) {
			t.result = fn(t.hs, t.hs_len, t.ne, t.ne_len);
			t.expected = simple_fn(t.hs, t.hs_len, t.ne, t.ne_len);
			t_check(name, t.result == t.expected);
			if (i < t.hs_len) {
				t.result = simple_fn(t.hs, i, t.ne, t.ne_len);
				t.expected = fn(t.hs, i, t.ne, t.ne_len);
				t_check(name, t.result == t.expected);
			}
			if (i < t.ne_len) {
				t.result = fn(t.hs, t.hs_len, t.ne, i);
				t.expected = simple_fn(t.hs, t.hs_len, t.ne, i);
				t_check(name, t.result == t.expected);
			}
			if (!JSTR_TEST_SLOW)
				break;
		}
	}
}

JSTR_ATTR_MAYBE_UNUSED static void
t_strnstr(const char *name, t_strnstr_fn fn, t_strnstr_fn simple_fn, const struct test_array_memmem *test_array, size_t count)
{
	for (size_t i = 0; i < count; ++i) {
		t_init();
		t.n = strlen(test_array[i].hs);
		t.hs = test_array[i].hs;
		t.ne = test_array[i].ne;
		t.ne_len = strlen(t.ne);
		for (size_t ne_i = 0; t.ne_len != (size_t)-1 && ne_i < 32; ++t.ne, --t.ne_len, ++ne_i) {
			t.result = fn(t.hs, t.ne, t.n);
			t.expected = simple_fn(t.hs, t.ne, t.n);
			t_check(name, t.result == t.expected);
			if (!JSTR_TEST_SLOW)
				break;
		}
	}
}

JSTR_ATTR_MAYBE_UNUSED static void
t_memcmp(const char *name, t_memcmp_fn fn, t_memcmp_fn simple_fn, const struct test_array_memcmp *test_array, size_t count, int negate)
{
	for (size_t i = 0; i < count; ++i) {
		t_init();
		t.s1 = test_array[i].s1;
		t.s2 = test_array[i].s2;
		t.s1_len = strlen(t.s1);
		t.s2_len = strlen(t.s2);
		t.n = JSTR_MIN(t.s1_len, JSTR_MIN(i, t.s2_len));
		t.result_n = (size_t)(negate ? !fn(t.s1, t.s2, t.n) : fn(t.s1, t.s2, t.n));
		t.expected_n = (size_t)(negate ? !simple_fn(t.s1, t.s2, t.n) : simple_fn(t.s1, t.s2, t.n));
		t_check(name, t.result_n == t.expected_n);
		if (i < t.n) {
			t.result_n = (size_t)(negate ? !fn(t.s1, t.s2, i) : fn(t.s1, t.s2, i));
			t.expected_n = (size_t)(negate ? !simple_fn(t.s1, t.s2, i) : simple_fn(t.s1, t.s2, i));
			t_check(name, t.result_n == t.expected_n);
		}
	}
}

JSTR_ATTR_MAYBE_UNUSED static void
t_strcmp(const char *name, t_strcmp_fn fn, t_strcmp_fn simple_fn, const struct test_array_memcmp *test_array, size_t count, int negate)
{
	for (size_t i = 0; i < count; ++i) {
		t_init();
		t.s1 = test_array[i].s1;
		t.s2 = test_array[i].s2;
		t.result_n = (size_t)(negate ? !fn(t.s1, t.s2) : fn(t.s1, t.s2));
		t.expected_n = (size_t)(negate ? !simple_fn(t.s1, t.s2) : simple_fn(t.s1, t.s2));
		t_check(name, t.expected_n == t.result_n);
	}
}

#endif /* JSTR_TEST_H */
