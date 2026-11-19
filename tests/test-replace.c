/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Fixes regex.h problems with tcc */
#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/jstr.h"
#include "test-array.h"

/* Diagnostic helper shared by the table-driven differential runners below. */
static void
check_case(const char *func, const char *str, const char *find, const char *rplc, size_t n, int cond, const jstr_ty *result, const jstr_ty *expected)
{
	if (!cond) {
		fprintf(stderr, "\nAssertion failure: %s().\nString: %s\nfind  : %s\nrplc  : %s\nn     : %zu\nResult:\n", func, test_str(str), test_str(find), test_str(rplc), n);
		jstr_debug(result);
		fprintf(stderr, "Expected:\n");
		jstr_debug(expected);
		abort();
	}
}

static void
rplc_init(jstr_ty *buf, const char *str, size_t str_len)
{
	jstr_empty(buf->data, &buf->size);
	assert(!jstr_chk(jstr_assign_len(jstr_struct(buf), str, str_len)));
	assert(buf->size == str_len);
	assert(!memcmp(buf->data, str, str_len));
}

static void
check_ret(const char *func, jstr_ret_ty got, jstr_ret_ty want, const jstr_ty *buf)
{
	if (got != want) {
		jstr_debug(buf);
		jstr_errdie("%s", func);
	}
}

typedef size_t (*rmchr_fn)(char *, size_t *, size_t, int, size_t);
typedef size_t (*rplcchr_fn)(char *, size_t, char, char, size_t);
typedef size_t (*rm_fn)(char *, size_t *, const char *, size_t, size_t);
typedef size_t (*rplc_fn)(char **, size_t *, size_t *, size_t, const char *, size_t, const char *, size_t, size_t);

static void
run_rmchr(const char *name, rmchr_fn fn, rmchr_fn simple_fn, struct test_pair *pair)
{
	test_testing(name);
	for (size_t i = 0; i < JSTR_ARRAY_COUNT(test_array_memcmp); ++i) {
		const char *str = test_array_memcmp[i].s1;
		const char find = *test_array_memcmp[i].s2;
		const char find_str[2] = { find, '\0' };
		size_t str_len = strlen(str);
		rplc_init(&pair->result, str, str_len);
		rplc_init(&pair->expected, str, str_len);
		t.n = i;
		t.result_n = fn(pair->result.data, &pair->result.size, 0, find, t.n);
		t.expected_n = simple_fn(pair->expected.data, &pair->expected.size, 0, find, t.n);
		check_case(name, str, find_str, "", i, t.expected_n == t.result_n, &pair->result, &pair->expected);
		check_case(name, str, find_str, "", i, strlen(pair->result.data) == strlen(pair->expected.data), &pair->result, &pair->expected);
		check_case(name, str, find_str, "", i, pair->result.size == pair->expected.size, &pair->result, &pair->expected);
		check_case(name, str, find_str, "", i, !memcmp(pair->result.data, pair->expected.data, pair->result.size), &pair->result, &pair->expected);
	}
}

static void
run_rplcchr(const char *name, rplcchr_fn fn, rplcchr_fn simple_fn, struct test_pair *pair)
{
	test_testing(name);
	for (size_t i = 0; i < JSTR_ARRAY_COUNT(test_array_memcmp); ++i) {
		const char *str = test_array_memcmp[i].s1;
		const char find = *test_array_memcmp[i].s2;
		const char rplc = *test_array_memcmp[i].s1;
		const char find_str[2] = { find, '\0' };
		const char rplc_str[2] = { rplc, '\0' };
		size_t str_len = strlen(str);
		rplc_init(&pair->result, str, str_len);
		rplc_init(&pair->expected, str, str_len);
		t.n = i;
		t.result_n = fn(pair->result.data, pair->result.size, find, rplc, t.n);
		t.expected_n = simple_fn(pair->expected.data, pair->expected.size, find, rplc, t.n);
		check_case(name, str, find_str, rplc_str, i, t.result_n == t.expected_n, &pair->result, &pair->expected);
		check_case(name, str, find_str, rplc_str, i, !memcmp(pair->result.data, pair->expected.data, pair->result.size), &pair->result, &pair->expected);
	}
}

static void
run_rm(const char *name, rm_fn fn, rm_fn simple_fn, struct test_pair *pair)
{
	test_testing(name);
	for (size_t i = 0; i < JSTR_ARRAY_COUNT(test_array_memmem); ++i) {
		const char *str = test_array_memmem[i].hs;
		const char *find = test_array_memmem[i].ne;
		size_t str_len = strlen(str);
		size_t find_len = strlen(find);
		rplc_init(&pair->result, str, str_len);
		rplc_init(&pair->expected, str, str_len);
		t.n = i;
		t.result_n = fn(pair->result.data, &pair->result.size, find, find_len, t.n);
		t.expected_n = simple_fn(pair->expected.data, &pair->expected.size, find, find_len, t.n);
		check_case(name, str, find, "", i, t.expected_n == t.result_n, &pair->result, &pair->expected);
		check_case(name, str, find, "", i, strlen(pair->result.data) == strlen(pair->expected.data), &pair->result, &pair->expected);
		check_case(name, str, find, "", i, pair->result.size == pair->expected.size, &pair->result, &pair->expected);
		check_case(name, str, find, "", i, !memcmp(pair->result.data, pair->expected.data, pair->result.size), &pair->result, &pair->expected);
	}
}

static void
run_rplc(const char *name, rplc_fn fn, rplc_fn simple_fn, struct test_pair *pair)
{
	test_testing(name);
	for (size_t i = 0; i < JSTR_ARRAY_COUNT(test_array_memmem); ++i) {
		const char *str = test_array_memmem[i].hs;
		const char *find = test_array_memmem[i].ne;
		size_t find_len = strlen(find);
		const char *rplc = find + find_len / 2;
		size_t str_len = strlen(str);
		size_t rplc_len = strlen(rplc);
		for (int loop = 1;; loop = 0) {
			t.n = i;
			rplc_init(&pair->result, str, str_len);
			rplc_init(&pair->expected, str, str_len);
			t.expected_n = fn(&pair->result.data, &pair->result.size, &pair->result.capacity, (i < str_len) ? i : 0, find, find_len, rplc, rplc_len, t.n);
			t.result_n = simple_fn(&pair->expected.data, &pair->expected.size, &pair->expected.capacity, (i < str_len) ? i : 0, find, find_len, rplc, rplc_len, t.n);
			check_case(name, str, find, rplc, i, t.expected_n == t.result_n, &pair->result, &pair->expected);
			check_case(name, str, find, rplc, i, strlen(pair->result.data) == strlen(pair->expected.data), &pair->result, &pair->expected);
			check_case(name, str, find, rplc, i, pair->result.size == pair->expected.size, &pair->result, &pair->expected);
			check_case(name, str, find, rplc, i, !memcmp(pair->result.data, pair->expected.data, pair->result.size), &pair->result, &pair->expected);
			if (!loop)
				break;
			const char *tmp_find = find;
			size_t tmp_find_len = find_len;
			find = rplc;
			find_len = rplc_len;
			rplc = tmp_find;
			rplc_len = tmp_find_len;
		}
	}
}

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rmnchr_len_from(char *s, size_t *sz, size_t start_idx, int remove, size_t n)
{
	if (n == 0)
		return 0;
	size_t changed = 0;
	const char *src = s + start_idx;
	char *dst = s + start_idx;
	size_t k = *sz - start_idx;
	for (; k--;) {
		if (*src == remove && n) {
			--n;
			++src;
			++changed;
		} else {
			*dst++ = *src++;
		}
	}
	*dst = '\0';
	*sz = JSTR_PTR_DIFF(dst, s);
	return changed;
}

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rplcnchr_len(char *s, size_t sz, char remove, char replace, size_t n)
{
	if (n == 0)
		return 0;
	size_t k = sz;
	size_t changed = 0;
	for (; k--; ++s)
		if (*s == remove) {
			if (n-- == 0)
				break;
			*s = replace;
			++changed;
		}
	return changed;
}

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rplcn_len_from(char **s, size_t *sz, size_t *cap, size_t start_idx, const char *find, size_t find_len, const char *rplc, size_t rplc_len, size_t n)
{
	if (n == 0 || *sz == 0 || find_len == 0)
		return 0;
	char *p = *s + start_idx;
	size_t changed = 0;
	for (; n-- && (p = simple_memmem(p, JSTR_PTR_DIFF(*s + *sz, p), find, find_len)); ++changed) {
		p = jstr_rplcat_len(s, sz, cap, JSTR_PTR_DIFF(p, *s), rplc, rplc_len, find_len);
		assert(p != NULL);
	}
	return changed;
}

JSTR_ATTR_MAYBE_UNUSED
static size_t
simple_rmn_len(char *s, size_t *sz, const char *find, size_t find_len, size_t n)
{
	size_t cap = (size_t)-1;
	return simple_rplcn_len_from(&s, sz, &cap, 0, find, find_len, "", 0, n);
}

int
main(void)
{
	jstr_ty result = JSTR_INIT;
	struct test_pair pair;
	test_pair_init(&pair);

	/* differential table-driven replace/remove tests */
	run_rmchr("jstr_rmnchr_len_from", jstr_rmnchr_len_from, simple_rmnchr_len_from, &pair);
	run_rplcchr("jstr_rplcnchr_len", jstr_rplcnchr_len, simple_rplcnchr_len, &pair);
	run_rm("jstr_rmn_len", jstr_rmn_len, simple_rmn_len, &pair);
	run_rplc("jstr_rplcn_len_from", (rplc_fn)jstr_rplcn_len_from, simple_rplcn_len_from, &pair);
	test_pair_free(&pair);
	jstr_empty(result.data, &result.size);

	const char *expected, *find, *rplc;

	/* builder tests */
	expected = "hello world";
	{
		jstr_cat_arg_ty args[] = { jstr_literal_init("hello"), jstr_literal_init(" "), jstr_literal_init("world") };
		jstr_ret_ty ret = jstr_cat(jstr_struct(&result), args, (size_t)3);
		check_ret("jstr_cat", ret, JSTR_RET_SUCC, &result);
		test_builder_expect("jstr_cat", ret, &result, expected);
		test_reset(&result);
	}
	expected = "hello world";
	{
		jstr_ret_ty ret = jstr_append_len(jstr_struct(&result), expected, strlen(expected));
		check_ret("jstr_append_len", ret, JSTR_RET_SUCC, &result);
		test_builder_expect("jstr_append_len", ret, &result, expected);
		test_reset(&result);
	}
	expected = "hello world";
	{
		jstr_ret_ty ret = jstr_prepend_len(jstr_struct(&result), expected, strlen(expected));
		check_ret("jstr_prepend_len", ret, JSTR_RET_SUCC, &result);
		test_builder_expect("jstr_prepend_len", ret, &result, expected);
		test_reset(&result);
	}
	expected = "hello world";
	{
		jstr_ret_ty ret = jstr_assign_len(jstr_struct(&result), expected, strlen(expected));
		check_ret("jstr_assign_len", ret, JSTR_RET_SUCC, &result);
		test_builder_expect("jstr_assign_len", ret, &result, expected);
		test_reset(&result);
	}

	/* replace tests */
	test_fill_shrunk(&result, "hello hello hello hello");
	find = "hello";
	rplc = "world";
	expected = "world hello hello hello";
	{
		size_t n = jstr_rplc_len(jstr_struct(&result), find, strlen(find), rplc, strlen(rplc));
		test_expect_buf("jstr_rplc_len", &result, expected);
		(void)n;
		test_reset(&result);
	}
	test_fill_shrunk(&result, "hello hello hello hello");
	find = "hello";
	rplc = "world";
	expected = "world world world world";
	{
		size_t n = jstr_rplcall_len(jstr_struct(&result), find, strlen(find), rplc, strlen(rplc));
		test_expect_buf("jstr_rplcall_len", &result, expected);
		(void)n;
		test_reset(&result);
	}
	test_fill_shrunk(&result, "hello hello hello hello");
	find = "hello";
	rplc = "";
	expected = "   ";
	{
		size_t n = jstr_rplcall_len(jstr_struct(&result), find, strlen(find), rplc, strlen(rplc));
		test_expect_buf("jstr_rplcall_len", &result, expected);
		(void)n;
		test_reset(&result);
	}
	test_fill_shrunk(&result, "hello hello hello hello");
	find = "hello";
	rplc = "";
	expected = "   ";
	{
		size_t n = jstr_rplcall_len(jstr_struct(&result), find, strlen(find), rplc, strlen(rplc));
		test_expect_buf("jstr_rplcall_len", &result, expected);
		(void)n;
		test_reset(&result);
	}
	test_fill_shrunk(&result, "world hello hello hello");
	find = "world";
	rplc = "q";
	expected = "world hello hello hello";
	{
		size_t n = jstr_rplcall_len_from(jstr_struct(&result), 1, find, strlen(find), rplc, strlen(rplc));
		test_expect_buf("jstr_rplcall_len_from", &result, expected);
		(void)n;
		test_reset(&result);
	}
	test_fill_shrunk(&result, "world hello hello hello");
	find = "world";
	rplc = "PworldQ";
	expected = "world hello hello hello";
	{
		size_t n = jstr_rplcall_len_from(jstr_struct(&result), 1, find, strlen(find), rplc, strlen(rplc));
		test_expect_buf("jstr_rplcall_len_from", &result, expected);
		(void)n;
		test_reset(&result);
	}
	test_fill_shrunk(&result, "world hello hello hello");
	find = "world";
	rplc = "q";
	expected = "q hello hello hello";
	{
		size_t n = jstr_rplcall_len_from(jstr_struct(&result), 0, find, strlen(find), rplc, strlen(rplc));
		test_expect_buf("jstr_rplcall_len_from", &result, expected);
		(void)n;
		test_reset(&result);
	}
	test_fill_shrunk(&result, "world hello hello hello");
	find = "world";
	rplc = "PworldQ";
	expected = "PworldQ hello hello hello";
	{
		size_t n = jstr_rplcall_len_from(jstr_struct(&result), 0, find, strlen(find), rplc, strlen(rplc));
		test_expect_buf("jstr_rplcall_len_from", &result, expected);
		(void)n;
		test_reset(&result);
	}
	test_fill_shrunk(&result, "world hello hello hello");
	find = "world";
	rplc = "";
	expected = " hello hello hello";
	{
		size_t n = jstr_rplcall_len_from(jstr_struct(&result), 0, find, strlen(find), rplc, strlen(rplc));
		test_expect_buf("jstr_rplcall_len_from", &result, expected);
		(void)n;
		test_reset(&result);
	}
	test_fill_shrunk(&result, "world hello hello hello");
	find = "world";
	rplc = "";
	expected = "world hello hello hello";
	{
		size_t n = jstr_rplcall_len_from(jstr_struct(&result), 1, find, strlen(find), rplc, strlen(rplc));
		test_expect_buf("jstr_rplcall_len_from", &result, expected);
		(void)n;
		test_reset(&result);
	}
	test_fill_shrunk(&result, "hello_world");
	find = "abcdefghijklmnopqrstuvwxyz";
	rplc = "q";
	expected = "qqqqq_qqqqq";
	{
		size_t n = jstr_rplcallspn(result.data, &result.size, find, *rplc);
		test_expect_buf("jstr_rplcallspn", &result, expected);
		(void)n;
		test_reset(&result);
	}

	/* builder tests */
	jstr_empty(result.data, &result.size);
	{
		jstr_cat_arg_ty args[] = { jstr_literal_init("hello"), jstr_literal_init(" world") };
		test_builder_expect("jstr_cat", jstr_cat(jstr_struct(&result), args, 2), &result, "hello world");
	}
	{
		jstr_cat_arg_ty args[] = { jstr_literal_init("a"), jstr_literal_init("b") };
		test_builder_expect("jstr_cat", jstr_cat(jstr_struct(&result), args, 2), &result, "hello worldab");
	}
	test_builder_expect("jstr_asprintf", jstr_asprintf(jstr_struct(&result), "%s", "c"), &result, "c");
	test_builder_expect("jstr_asprintf_append", jstr_asprintf_append(jstr_struct(&result), "%s", "z"), &result, "cz");
	test_builder_expect("jstr_assign_len", jstr_assign_len(jstr_struct(&result), "hello", strlen("hello")), &result, "hello");
	test_builder_expect("jstr_assign_len", jstr_assign_len(jstr_struct(&result), jstr_literal("hello")), &result, "hello");
	test_builder_expect("jstr_append_len", jstr_append_len(jstr_struct(&result), jstr_literal(" world")), &result, "hello world");
	jstr_empty(result.data, &result.size);
	result.size = JSTR_PTR_DIFF(jstr_pushback_unsafe_p(result.data, result.size, 'k'), result.data);
	test_expect_buf("jstr_pushback_unsafe_p", &result, "k");
	result.size = JSTR_PTR_DIFF(jstr_pushfront_unsafe_p(result.data, result.size, 'l'), result.data);
	test_expect_buf("jstr_pushfront_unsafe_p", &result, "lk");
	result.size = JSTR_PTR_DIFF(jstr_popfront_p(result.data, result.size), result.data);
	test_expect_buf("jstr_popfront_p", &result, "k");
	result.size = JSTR_PTR_DIFF(jstr_popfront_p(result.data, result.size), result.data);
	test_expect_buf("jstr_popfront_p", &result, "");
	result.size = JSTR_PTR_DIFF(jstr_popfront_p(result.data, result.size), result.data);
	test_expect_buf("jstr_popfront_p", &result, "");
	result.size = JSTR_PTR_DIFF(jstr_popback_p(result.data, result.size), result.data);
	test_expect_buf("jstr_popback_p", &result, "");
	result.size = JSTR_PTR_DIFF(jstr_popback_p(result.data, result.size), result.data);
	test_expect_buf("jstr_popback_p", &result, "");
	test_builder_expect("jstr_assign_len", jstr_assign_len(jstr_struct(&result), "hello", strlen("hello")), &result, "hello");
	result.size = JSTR_PTR_DIFF(jstr_popback_p(result.data, result.size), result.data);
	test_expect_buf("jstr_popback_p", &result, "hell");
	result.size = JSTR_PTR_DIFF(jstr_popback_p(result.data, result.size), result.data);
	test_expect_buf("jstr_popback_p", &result, "hel");
	result.size = JSTR_PTR_DIFF(jstr_popfront_p(result.data, result.size), result.data);
	test_expect_buf("jstr_popfront_p", &result, "el");
	result.size = JSTR_PTR_DIFF(jstr_popfront_p(result.data, result.size), result.data);
	test_expect_buf("jstr_popfront_p", &result, "l");

	jstr_free_j(&result);
	return EXIT_SUCCESS;
}
