/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Fixes regex.h problems with tcc */
#define __STDC_NO_VLA__ 1

#include "test.h"

#define T_ASSERT(func, result, expected)                                                                           \
	do {                                                                                                       \
		ASSERT_RESULT(func, (result).size == strlen(expected), (result).data, expected);                   \
		ASSERT_RESULT(func, memcmp((result).data, expected, (result).size) == 0, (result).data, expected); \
	} while (0)

#define FILL(result, str)                                                                     \
	do {                                                                                  \
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&(result)), str, strlen(str)))); \
		(void)jstr_shrink_to_fit(&result.data, &result.size, &result.capacity);             \
	} while (0)

#define T_APPEND_NORET(func, ...)                              \
	do {                                                   \
		TESTING(func);                                 \
		func(__VA_ARGS__);                             \
		T_ASSERT(func(__VA_ARGS__), result, expected); \
		(result).size = 0;                             \
		*(result).data = '\0';                         \
	} while (0)

int
main(int argc, char **argv)
{
	START();
	jstr_ty result = JSTR_INIT;
	const char *find;
	const char *rplc;
	const char *expected;

	/* 1. Empty find string */
	/* jstr_rplcn_len_from_exec with find_len=0 should return 0, buffer unchanged */
	FILL(result, "hello");
	{
		jstr_twoway_ty t;
		jstr_memmem_comp(&t, "", (size_t)0);
		TESTING(jstr_rplcn_len_from_exec);
		size_t n = jstr_rplcn_len_from_exec(&t, jstr_struct(&result), 0, "", (size_t)0, "x", 1, (size_t)-1);
		assert(n == 0);
		assert(result.size == 5);
		assert(!memcmp(result.data, "hello", 5));
	}
	result.size = 0;
	*result.data = '\0';
	/* jstr_rplc_len_from_exec with find_len=0 should return 0, buffer unchanged */
	FILL(result, "hello");
	{
		jstr_twoway_ty t;
		jstr_memmem_comp(&t, "", (size_t)0);
		TESTING(jstr_rplc_len_from_exec);
		int n = jstr_rplc_len_from_exec(&t, jstr_struct(&result), 0, "", (size_t)0, "x", 1);
		assert(n == 0);
		assert(result.size == 5);
		assert(!memcmp(result.data, "hello", 5));
	}
	result.size = 0;
	*result.data = '\0';
	/* jstr_rplcn_len_from wrapper with find_len=0 */
	FILL(result, "hello");
	find = "";
	rplc = "x";
	expected = "hello";
	T_APPEND_NORET(jstr_rplcn_len_from, jstr_struct(&result), 0, find, (size_t)0, rplc, strlen(rplc), (size_t)-1);
	/* jstr_rplc_len_from wrapper with find_len=0 */
	FILL(result, "hello");
	expected = "hello";
	T_APPEND_NORET(jstr_rplc_len_from, jstr_struct(&result), 0, find, (size_t)0, rplc, strlen(rplc));

	/* 2. Empty replace string */
	FILL(result, "abc");
	find = "b";
	rplc = "";
	expected = "ac";
	T_APPEND_NORET(jstr_rplcn_len_from, jstr_struct(&result), 0, find, strlen(find), rplc, strlen(rplc), (size_t)-1);

	/* 3. Find longer than input */
	FILL(result, "abc");
	find = "abcdef";
	rplc = "x";
	expected = "abc";
	T_APPEND_NORET(jstr_rplcn_len_from, jstr_struct(&result), 0, find, strlen(find), rplc, strlen(rplc), (size_t)-1);

	/* 4. Multiple replacements (n=3) */
	FILL(result, "a a a a a");
	find = "a";
	rplc = "X";
	expected = "X X X a a";
	T_APPEND_NORET(jstr_rplcn_len_from, jstr_struct(&result), 0, find, strlen(find), rplc, strlen(rplc), 3);

	/* 5. Shorter replacement (in-place path) */
	FILL(result, "abcxyzdef");
	find = "xyz";
	rplc = "!";
	expected = "abc!def";
	T_APPEND_NORET(jstr_rplcn_len_from, jstr_struct(&result), 0, find, strlen(find), rplc, strlen(rplc), (size_t)-1);

	/* 6. Longer replacement (allocation path) */
	FILL(result, "axb");
	find = "x";
	rplc = "XYZ";
	expected = "aXYZb";
	T_APPEND_NORET(jstr_rplcn_len_from, jstr_struct(&result), 0, find, strlen(find), rplc, strlen(rplc), (size_t)-1);

	/* 7. Overlapping matches (Two-Way searcher does not overlap) */
	FILL(result, "aaaa");
	find = "aa";
	rplc = "x";
	expected = "xx";
	T_APPEND_NORET(jstr_rplcn_len_from, jstr_struct(&result), 0, find, strlen(find), rplc, strlen(rplc), (size_t)-1);

	/* 8. N=0 should return 0, buffer unchanged */
	FILL(result, "hello world");
	find = "hello";
	rplc = "x";
	expected = "hello world";
	T_APPEND_NORET(jstr_rplcn_len_from, jstr_struct(&result), 0, find, strlen(find), rplc, strlen(rplc), 0);

	jstr_free_j(&result);
	SUCCESS();
	return EXIT_SUCCESS;
}
