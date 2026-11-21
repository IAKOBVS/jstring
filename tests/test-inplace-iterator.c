/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Regression tests for the explicit in-place iterator in
 * include/internal/inplace.h: zero-length handling, grow/backref rebasing,
 * capacity preservation, and the jstr_stripspn_from start_idx guard. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/regex.h"

int
main(void)
{
	jstr_ty result = JSTR_INIT;

	/* start_idx beyond the string is a no-op (RP-4 guard). */
	test_fill_shrunk(&result, "abc");
	test_expect_call("jstr_stripspn_from", (long long)jstr_stripspn_from(result.data, &result.size, 10, "b"), (long long)0, &result, "abc");

	/* ZERO_ADVANCE policy: zero-length regex matches terminate correctly. */
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "a*", 0)));
		test_fill_shrunk(&result, "baaab");
		test_expect_call("jstr_re_rmn_exec", (long long)jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, (size_t)-1), (long long)4, &result, "bb");
		jstr_re_free(&preg);
	}

	/* GROW state with repeated realloc and backref emission. */
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([0-9]\\)", 0)));
		test_fill_shrunk(&result, "1234567890");
		test_expect_call("jstr_re_rplcall_backref_len_exec", (long long)jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1\\1\\1\\1", 10, 0, 2), (long long)10, &result, "11111222223333344444555556666677777888889999900000");
		jstr_re_free(&preg);
	}

	/* Capacity is untouched by remove/equal paths; grow keeps it sufficient. */
	test_fill_shrunk(&result, "ababab");
	const size_t tight_cap = result.capacity;
	test_expect_true("iterator", "tight capacity", tight_cap == result.size + 1);
	(void)jstr_rmn_len_from(result.data, &result.size, 0, "ab", 2, 1);
	test_expect_true("iterator", "remove keeps capacity", result.capacity == tight_cap);
	test_fill_shrunk(&result, "ababab");
	(void)jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "cd", 2, (size_t)-1);
	test_expect_true("iterator", "equal replace keeps capacity", result.capacity == tight_cap);
	test_fill_shrunk(&result, "ababab");
	(void)jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "xyz", 3, (size_t)-1);
	test_expect_true("iterator", "grow reserves enough", result.capacity >= result.size + 1);

	jstr_free_j(&result);
	return EXIT_SUCCESS;
}
