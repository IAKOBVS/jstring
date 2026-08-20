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

#define FILL(result, str)                                                                     \
	do {                                                                                  \
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&(result)), str, strlen(str)))); \
		(void)jstr_shrink_to_fit(&result.data, &result.size, &result.capacity);       \
	} while (0)

#define CHECK_CALL(func, ret_expr, expected_str, expected_ret)                                                                                                              \
	do {                                                                                                                                                                \
		const long long _ret = (long long)(ret_expr);                                                                                                               \
		TESTING(func);                                                                                                                                              \
		if (jstr_unlikely(_ret != (long long)(expected_ret) || (result).size != strlen(expected_str) || memcmp((result).data, expected_str, (result).size) != 0)) { \
			PRINTERR("Assertion failure: %s.\nResult:%s.\nExpected:%s.\n", #func, (result).data, expected_str);                                                 \
			assert(0);                                                                                                                                          \
		}                                                                                                                                                           \
	} while (0)

int
main(int argc, char **argv)
{
	START();
	jstr_ty result = JSTR_INIT;

	/* jstr_rmallspn_from: remove every run of reject chars. */
	FILL(result, "hello world");
	CHECK_CALL(jstr_rmallspn_from, jstr_rmallspn_from(result.data, &result.size, 0, "l"), "heo word", 3);
	FILL(result, "aaabc");
	CHECK_CALL(jstr_rmallspn_from, jstr_rmallspn_from(result.data, &result.size, 0, "a"), "bc", 3);
	FILL(result, "bcaaa");
	CHECK_CALL(jstr_rmallspn_from, jstr_rmallspn_from(result.data, &result.size, 0, "a"), "bc", 3);
	FILL(result, "aaa");
	CHECK_CALL(jstr_rmallspn_from, jstr_rmallspn_from(result.data, &result.size, 0, "a"), "", 3);
	FILL(result, "hello");
	CHECK_CALL(jstr_rmallspn_from, jstr_rmallspn_from(result.data, &result.size, 0, "z"), "hello", 0);
	FILL(result, "abacada");
	CHECK_CALL(jstr_rmallspn_from, jstr_rmallspn_from(result.data, &result.size, 1, "a"), "abcd", 3);
	FILL(result, "hello");
	CHECK_CALL(jstr_rmallspn_from, jstr_rmallspn_from(result.data, &result.size, 5, "l"), "hello", 0);
	FILL(result, "");
	CHECK_CALL(jstr_rmallspn_from, jstr_rmallspn_from(result.data, &result.size, 0, "a"), "", 0);

	/* jstr_rmnchr_len_from: remove up to n occurrences of a single char. */
	FILL(result, "banana");
	CHECK_CALL(jstr_rmnchr_len_from, jstr_rmnchr_len_from(result.data, &result.size, 0, 'a', 2), "bnna", 2);
	FILL(result, "banana");
	CHECK_CALL(jstr_rmnchr_len_from, jstr_rmnchr_len_from(result.data, &result.size, 0, 'a', (size_t)-1), "bnn", 3);
	FILL(result, "banana");
	CHECK_CALL(jstr_rmnchr_len_from, jstr_rmnchr_len_from(result.data, &result.size, 0, 'a', 0), "banana", 0);
	FILL(result, "hello");
	CHECK_CALL(jstr_rmnchr_len_from, jstr_rmnchr_len_from(result.data, &result.size, 0, 'x', (size_t)-1), "hello", 0);
	FILL(result, "ababa");
	CHECK_CALL(jstr_rmnchr_len_from, jstr_rmnchr_len_from(result.data, &result.size, 0, 'a', 1), "baba", 1);
	FILL(result, "banana");
	CHECK_CALL(jstr_rmnchr_len_from, jstr_rmnchr_len_from(result.data, &result.size, 2, 'a', 1), "banna", 1);
	FILL(result, "ababa");
	CHECK_CALL(jstr_rmnchr_len_from, jstr_rmnchr_len_from(result.data, &result.size, 4, 'a', 1), "abab", 1);

	/* jstr_rmnchr_from: NUL-terminated variant. */
	FILL(result, "banana");
	CHECK_CALL(jstr_rmnchr_from, jstr_rmnchr_from(result.data, &result.size, 0, 'a', 2), "bnna", 2);
	FILL(result, "banana");
	CHECK_CALL(jstr_rmnchr_from, jstr_rmnchr_from(result.data, &result.size, 0, 'a', (size_t)-1), "bnn", 3);
	FILL(result, "hello");
	CHECK_CALL(jstr_rmnchr_from, jstr_rmnchr_from(result.data, &result.size, 0, 'x', (size_t)-1), "hello", 0);
	FILL(result, "ababa");
	CHECK_CALL(jstr_rmnchr_from, jstr_rmnchr_from(result.data, &result.size, 4, 'a', 1), "abab", 1);

	/* jstr_stripspn_from: remove individual reject chars. */
	FILL(result, "aaabc");
	CHECK_CALL(jstr_stripspn_from, jstr_stripspn_from(result.data, &result.size, 0, "a"), "bc", 3);
	FILL(result, "aaa");
	CHECK_CALL(jstr_stripspn_from, jstr_stripspn_from(result.data, &result.size, 0, "a"), "", 3);
	FILL(result, "abaab");
	CHECK_CALL(jstr_stripspn_from, jstr_stripspn_from(result.data, &result.size, 0, "a"), "bb", 3);
	FILL(result, "hello");
	CHECK_CALL(jstr_stripspn_from, jstr_stripspn_from(result.data, &result.size, 0, "z"), "hello", 0);
	FILL(result, "abacada");
	CHECK_CALL(jstr_stripspn_from, jstr_stripspn_from(result.data, &result.size, 1, "a"), "abcd", 3);
	FILL(result, "");
	CHECK_CALL(jstr_stripspn_from, jstr_stripspn_from(result.data, &result.size, 0, "a"), "", 0);

	/* jstr_rmn_len_from: remove up to n occurrences of a substring. */
	FILL(result, "ababab");
	CHECK_CALL(jstr_rmn_len_from, jstr_rmn_len_from(result.data, &result.size, 0, "ab", 2, 2), "ab", 2);
	FILL(result, "ababab");
	CHECK_CALL(jstr_rmn_len_from, jstr_rmn_len_from(result.data, &result.size, 0, "ab", 2, (size_t)-1), "", 3);
	FILL(result, "ababab");
	CHECK_CALL(jstr_rmn_len_from, jstr_rmn_len_from(result.data, &result.size, 0, "ab", 2, 0), "ababab", 0);
	FILL(result, "abxab");
	CHECK_CALL(jstr_rmn_len_from, jstr_rmn_len_from(result.data, &result.size, 0, "xy", 2, (size_t)-1), "abxab", 0);
	FILL(result, "aaaaa");
	CHECK_CALL(jstr_rmn_len_from, jstr_rmn_len_from(result.data, &result.size, 0, "aa", 2, 2), "a", 2);
	FILL(result, "banana");
	CHECK_CALL(jstr_rmn_len_from, jstr_rmn_len_from(result.data, &result.size, 0, "a", 1, 2), "bnna", 2);
	FILL(result, "ababab");
	CHECK_CALL(jstr_rmn_len_from, jstr_rmn_len_from(result.data, &result.size, 0, "ab", 2, 1), "abab", 1);

	/* jstr_rplcn_len_from: equal-length, shrinking, and growing replacements. */
	FILL(result, "ababab");
	CHECK_CALL(jstr_rplcn_len_from, jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "xy", 2, (size_t)-1), "xyxyxy", 3);
	FILL(result, "ababab");
	CHECK_CALL(jstr_rplcn_len_from, jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "x", 1, (size_t)-1), "xxx", 3);
	FILL(result, "ababab");
	CHECK_CALL(jstr_rplcn_len_from, jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "xyz", 3, (size_t)-1), "xyzxyzxyz", 3);
	FILL(result, "ababab");
	CHECK_CALL(jstr_rplcn_len_from, jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "xyz", 3, 2), "xyzxyzab", 2);
	FILL(result, "ababab");
	CHECK_CALL(jstr_rplcn_len_from, jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "xyz", 3, 1), "xyzabab", 1);
	FILL(result, "hello");
	CHECK_CALL(jstr_rplcn_len_from, jstr_rplcn_len_from(jstr_struct(&result), 0, "xy", 2, "zz", 2, (size_t)-1), "hello", 0);
	FILL(result, "ababab");
	CHECK_CALL(jstr_rplcn_len_from, jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "x", 1, 1), "xabab", 1);
	FILL(result, "ababab");
	CHECK_CALL(jstr_rplcn_len_from, jstr_rplcn_len_from(jstr_struct(&result), 0, "ab", 2, "cd", 2, (size_t)-1), "cdcdcd", 3);

	/* Regex removal with zero-length matches and n limits. */
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "a*", 0)));
		FILL(result, "baaab");
		CHECK_CALL(jstr_re_rmn_exec, jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, (size_t)-1), "bb", 4);
		FILL(result, "baaab");
		CHECK_CALL(jstr_re_rmn_exec, jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, 2), "bb", 2);
		FILL(result, "");
		CHECK_CALL(jstr_re_rmn_exec, jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, (size_t)-1), "", 1);
		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));
		FILL(result, "abc123def456");
		CHECK_CALL(jstr_re_rmn_exec, jstr_re_rmn_exec(&preg, jstr_struct(&result), 0, (size_t)-1), "abcdef", 2);
		jstr_re_free(&preg);
	}

	/* Regex backref replacement, including zero-length matches and the
	 * growing (realloc) path. */
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([0-9]\\{1,\\}\\)-\\([0-9]\\{1,\\}\\)", 0)));
		FILL(result, "a12-34b");
		CHECK_CALL(jstr_re_rplcall_backref_len_exec, jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "\\2/\\1", 5, 0, 3), "a34/12b", 1);
		FILL(result, "a1-2b3-4c");
		CHECK_CALL(jstr_re_rplcn_backref_len_from_exec, jstr_re_rplcn_backref_len_from_exec(&preg, jstr_struct(&result), 1, "\\2\\1", 4, 0, 3, 1), "a21b3-4c", 1);
		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "a*", 0)));
		FILL(result, "baaab");
		CHECK_CALL(jstr_re_rplcall_backref_len_exec, jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "[\\0]", 4, 0, 1), "[]b[aaa][]b[]", 4);
		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([0-9]\\)", 0)));
		FILL(result, "1234567890");
		CHECK_CALL(jstr_re_rplcall_backref_len_exec, jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "\\1\\1\\1\\1\\1", 10, 0, 2), "11111222223333344444555556666677777888889999900000", 10);
		jstr_re_free(&preg);
	}

	jstr_free_j(&result);
	SUCCESS();
	return EXIT_SUCCESS;
}
