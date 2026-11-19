/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Edge cases for the in-place remove/replace loops in replace.h and regex.h.
 * These loops were rewritten to avoid goto and to share the zero-length-match
 * handling helper in regex.h, so this test pins down their exact semantics:
 * matched-at-start/end, n limits, start_idx, equal/shrinking/growing
 * replacements, and zero-length regex matches. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/regex.h"

int
main(void)
{
	jstr_ty result = JSTR_INIT;

	/* jstr_rmallspn_from: remove every run of reject chars. */
	test_fill_shrunk(&result, "hello world");
	test_expect_call("jstr_rmallspn_from", (long long)(jstr_rmallspn_from(result.data, &result.size, 0, "l")), (long long)(3), &result, "heo word");
	test_fill_shrunk(&result, "aaabc");
	test_expect_call("jstr_rmallspn_from", (long long)(jstr_rmallspn_from(result.data, &result.size, 0, "a")), (long long)(3), &result, "bc");
	test_fill_shrunk(&result, "bcaaa");
	test_expect_call("jstr_rmallspn_from", (long long)(jstr_rmallspn_from(result.data, &result.size, 0, "a")), (long long)(3), &result, "bc");
	test_fill_shrunk(&result, "aaa");
	test_expect_call("jstr_rmallspn_from", (long long)(jstr_rmallspn_from(result.data, &result.size, 0, "a")), (long long)(3), &result, "");
	test_fill_shrunk(&result, "hello");
	test_expect_call("jstr_rmallspn_from", (long long)(jstr_rmallspn_from(result.data, &result.size, 0, "z")), (long long)(0), &result, "hello");
	test_fill_shrunk(&result, "abacada");
	test_expect_call("jstr_rmallspn_from", (long long)(jstr_rmallspn_from(result.data, &result.size, 1, "a")), (long long)(3), &result, "abcd");
	test_fill_shrunk(&result, "hello");
	test_expect_call("jstr_rmallspn_from", (long long)(jstr_rmallspn_from(result.data, &result.size, 5, "l")), (long long)(0), &result, "hello");
	test_fill_shrunk(&result, "");
	test_expect_call("jstr_rmallspn_from", (long long)(jstr_rmallspn_from(result.data, &result.size, 0, "a")), (long long)(0), &result, "");

	/* jstr_rmnchr_len_from: remove up to n occurrences of a single char. */
	test_fill_shrunk(&result, "banana");
	test_expect_call("jstr_rmnchr_len_from", (long long)(jstr_rmnchr_len_from(result.data, &result.size, 0, 'a', 2)), (long long)(2), &result, "bnna");
	test_fill_shrunk(&result, "banana");
	test_expect_call("jstr_rmnchr_len_from", (long long)(jstr_rmnchr_len_from(result.data, &result.size, 0, 'a', (size_t)-1)), (long long)(3), &result, "bnn");
	test_fill_shrunk(&result, "banana");
	test_expect_call("jstr_rmnchr_len_from", (long long)(jstr_rmnchr_len_from(result.data, &result.size, 0, 'a', 0)), (long long)(0), &result, "banana");
	test_fill_shrunk(&result, "hello");
	test_expect_call("jstr_rmnchr_len_from", (long long)(jstr_rmnchr_len_from(result.data, &result.size, 0, 'x', (size_t)-1)), (long long)(0), &result, "hello");
	test_fill_shrunk(&result, "ababa");
	test_expect_call("jstr_rmnchr_len_from", (long long)(jstr_rmnchr_len_from(result.data, &result.size, 0, 'a', 1)), (long long)(1), &result, "baba");
	test_fill_shrunk(&result, "banana");
	test_expect_call("jstr_rmnchr_len_from", (long long)(jstr_rmnchr_len_from(result.data, &result.size, 2, 'a', 1)), (long long)(1), &result, "banna");
	test_fill_shrunk(&result, "ababa");
	test_expect_call("jstr_rmnchr_len_from", (long long)(jstr_rmnchr_len_from(result.data, &result.size, 4, 'a', 1)), (long long)(1), &result, "abab");

	/* jstr_rmnchr_from: NUL-terminated variant. */
	test_fill_shrunk(&result, "banana");
	test_expect_call("jstr_rmnchr_from", (long long)(jstr_rmnchr_from(result.data, &result.size, 0, 'a', 2)), (long long)(2), &result, "bnna");
	test_fill_shrunk(&result, "banana");
	test_expect_call("jstr_rmnchr_from", (long long)(jstr_rmnchr_from(result.data, &result.size, 0, 'a', (size_t)-1)), (long long)(3), &result, "bnn");
	test_fill_shrunk(&result, "hello");
	test_expect_call("jstr_rmnchr_from", (long long)(jstr_rmnchr_from(result.data, &result.size, 0, 'x', (size_t)-1)), (long long)(0), &result, "hello");
	test_fill_shrunk(&result, "ababa");
	test_expect_call("jstr_rmnchr_from", (long long)(jstr_rmnchr_from(result.data, &result.size, 4, 'a', 1)), (long long)(1), &result, "abab");

	/* jstr_stripspn_from: remove individual reject chars. */
	test_fill_shrunk(&result, "aaabc");
	test_expect_call("jstr_stripspn_from", (long long)(jstr_stripspn_from(result.data, &result.size, 0, "a")), (long long)(3), &result, "bc");
	test_fill_shrunk(&result, "aaa");
	test_expect_call("jstr_stripspn_from", (long long)(jstr_stripspn_from(result.data, &result.size, 0, "a")), (long long)(3), &result, "");
	test_fill_shrunk(&result, "abaab");
	test_expect_call("jstr_stripspn_from", (long long)(jstr_stripspn_from(result.data, &result.size, 0, "a")), (long long)(3), &result, "bb");
	test_fill_shrunk(&result, "hello");
	test_expect_call("jstr_stripspn_from", (long long)(jstr_stripspn_from(result.data, &result.size, 0, "z")), (long long)(0), &result, "hello");
	test_fill_shrunk(&result, "abacada");
	test_expect_call("jstr_stripspn_from", (long long)(jstr_stripspn_from(result.data, &result.size, 1, "a")), (long long)(3), &result, "abcd");
	test_fill_shrunk(&result, "");
	test_expect_call("jstr_stripspn_from", (long long)(jstr_stripspn_from(result.data, &result.size, 0, "a")), (long long)(0), &result, "");

	/* jstr_rmn_len_from: remove up to n occurrences of a substring. */
	test_fill_shrunk(&result, "ababab");
	test_expect_call("jstr_rmn_len_from", (long long)(jstr_rmn_len_from(result.data, &result.size, 0, "ab", 2, 2)), (long long)(2), &result, "ab");
	test_fill_shrunk(&result, "ababab");
	test_expect_call("jstr_rmn_len_from", (long long)(jstr_rmn_len_from(result.data, &result.size, 0, "ab", 2, (size_t)-1)), (long long)(3), &result, "");
	test_fill_shrunk(&result, "ababab");
	test_expect_call("jstr_rmn_len_from", (long long)(jstr_rmn_len_from(result.data, &result.size, 0, "ab", 2, 0)), (long long)(0), &result, "ababab");
	test_fill_shrunk(&result, "abxab");
	test_expect_call("jstr_rmn_len_from", (long long)(jstr_rmn_len_from(result.data, &result.size, 0, "xy", 2, (size_t)-1)), (long long)(0), &result, "abxab");
	test_fill_shrunk(&result, "aaaaa");
	test_expect_call("jstr_rmn_len_from", (long long)(jstr_rmn_len_from(result.data, &result.size, 0, "aa", 2, 2)), (long long)(2), &result, "a");
	test_fill_shrunk(&result, "banana");
	test_expect_call("jstr_rmn_len_from", (long long)(jstr_rmn_len_from(result.data, &result.size, 0, "a", 1, 2)), (long long)(2), &result, "bnna");
	test_fill_shrunk(&result, "ababab");
	test_expect_call("jstr_rmn_len_from", (long long)(jstr_rmn_len_from(result.data, &result.size, 0, "ab", 2, 1)), (long long)(1), &result, "abab");

	/* jstr_rplcn_len_from: equal-length, shrinking, and growing replacements. */
	test_fill_shrunk(&result, "ababab");
	test_expect_call("jstr_rplcn_len_from", (long long)(jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "xy", 2, (size_t)-1)), (long long)(3), &result, "xyxyxy");
	test_fill_shrunk(&result, "ababab");
	test_expect_call("jstr_rplcn_len_from", (long long)(jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "x", 1, (size_t)-1)), (long long)(3), &result, "xxx");
	test_fill_shrunk(&result, "ababab");
	test_expect_call("jstr_rplcn_len_from", (long long)(jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "xyz", 3, (size_t)-1)), (long long)(3), &result, "xyzxyzxyz");
	test_fill_shrunk(&result, "ababab");
	test_expect_call("jstr_rplcn_len_from", (long long)(jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "xyz", 3, 2)), (long long)(2), &result, "xyzxyzab");
	test_fill_shrunk(&result, "ababab");
	test_expect_call("jstr_rplcn_len_from", (long long)(jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "xyz", 3, 1)), (long long)(1), &result, "xyzabab");
	test_fill_shrunk(&result, "hello");
	test_expect_call("jstr_rplcn_len_from", (long long)(jstr_rplcn_len_from(jstr_struct(&result), 0, "xy", 2, "zz", 2, (size_t)-1)), (long long)(0), &result, "hello");
	test_fill_shrunk(&result, "ababab");
	test_expect_call("jstr_rplcn_len_from", (long long)(jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "x", 1, 1)), (long long)(1), &result, "xabab");
	test_fill_shrunk(&result, "ababab");
	test_expect_call("jstr_rplcn_len_from", (long long)(jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "cd", 2, (size_t)-1)), (long long)(3), &result, "cdcdcd");

	/* Regex removal with zero-length matches and n limits. */
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "a*", 0)));
		test_fill_shrunk(&result, "baaab");
		test_expect_call("jstr_re_rmn_exec", (long long)(jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, (size_t)-1)), (long long)(4), &result, "bb");
		test_fill_shrunk(&result, "baaab");
		test_expect_call("jstr_re_rmn_exec", (long long)(jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, 2)), (long long)(2), &result, "bb");
		test_fill_shrunk(&result, "");
		test_expect_call("jstr_re_rmn_exec", (long long)(jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, (size_t)-1)), (long long)(1), &result, "");
		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));
		test_fill_shrunk(&result, "abc123def456");
		test_expect_call("jstr_re_rmn_exec", (long long)(jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, (size_t)-1)), (long long)(2), &result, "abcdef");
		jstr_re_free(&preg);
	}

	/* Regex backref replacement, including zero-length matches and the
	 * growing (realloc) path. */
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([0-9]\\{1,\\}\\)-\\([0-9]\\{1,\\}\\)", 0)));
		test_fill_shrunk(&result, "a12-34b");
		test_expect_call("jstr_re_rplcall_backref_len_exec", (long long)(jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "\\2/\\1", 5, 0, 3)), (long long)(1), &result, "a34/12b");
		test_fill_shrunk(&result, "a1-2b3-4c");
		test_expect_call("jstr_re_rplcn_backref_len_from_exec", (long long)(jstr_re_rplcn_backref_len_from_exec(&preg, jstr_struct(&result), 1, "\\2\\1", 4, 0, 3, 1)), (long long)(1), &result, "a21b3-4c");
		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "a*", 0)));
		test_fill_shrunk(&result, "baaab");
		test_expect_call("jstr_re_rplcall_backref_len_exec", (long long)(jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "[\\0]", 4, 0, 1)), (long long)(4), &result, "[]b[aaa][]b[]");
		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([0-9]\\)", 0)));
		test_fill_shrunk(&result, "1234567890");
		test_expect_call("jstr_re_rplcall_backref_len_exec", (long long)(jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1\\1\\1\\1", 10, 0, 2)), (long long)(10), &result, "11111222223333344444555556666677777888889999900000");
		jstr_re_free(&preg);
	}

	jstr_free_j(&result);
	return EXIT_SUCCESS;
}
