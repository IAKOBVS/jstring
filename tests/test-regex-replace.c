/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Fixes regex.h problems with tcc */
#define __STDC_NO_VLA__ 1

#include "test.h"

#include <stdio.h>
#include "../include/regex.h"

/* Like T_APPEND_NORET but checks return count too. */

int
main(void)
{
	jstr_ty result = JSTR_INIT;
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "abc123def");

		{
			test_expect_call("jstr_re_rplc_len_exec", (long long)(jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "X", 1, 0)), (long long)(1), &result, "abcXdef");
			test_reset(&result);
		};

		test_shrink_fill(&result, "abc");

		{
			test_expect_call("jstr_re_rplc_len_exec", (long long)(jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "X", 1, 0)), (long long)(0), &result, "abc");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "abc123def456");

		{
			test_expect_call("jstr_re_rplc_len_from_exec", (long long)(jstr_re_rplc_len_from_exec(&preg, jstr_struct(&result), 6, "X", 1, 0)), (long long)(1), &result, "abc123defX");
			test_reset(&result);
		};

		test_shrink_fill(&result, "abc123def456");

		{
			test_expect_call("jstr_re_rplc_len_from_exec", (long long)(jstr_re_rplc_len_from_exec(&preg, jstr_struct(&result), 12, "X", 1, 0)), (long long)(0), &result, "abc123def456");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "1a2b3c");

		{
			test_expect_call("jstr_re_rplcn_len_exec", (long long)(jstr_re_rplcn_len_exec(&preg, jstr_struct(&result), "X", 1, 0, 2)), (long long)(2), &result, "XaXb3c");
			test_reset(&result);
		};

		test_shrink_fill(&result, "1a2b3c");

		{
			test_expect_call("jstr_re_rplcn_len_exec", (long long)(jstr_re_rplcn_len_exec(&preg, jstr_struct(&result), "X", 1, 0, 0)), (long long)(0), &result, "1a2b3c");
			test_reset(&result);
		};

		test_shrink_fill(&result, "1a2b3c");

		{
			test_expect_call("jstr_re_rplcn_len_exec", (long long)(jstr_re_rplcn_len_exec(&preg, jstr_struct(&result), "X", 1, 0, (size_t)-1)), (long long)(3), &result, "XaXbXc");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "1a2b3c4d");

		{
			test_expect_call("jstr_re_rplcn_len_from_exec", (long long)(jstr_re_rplcn_len_from_exec(&preg, jstr_struct(&result), 2, "X", 1, 0, 2)), (long long)(2), &result, "1aXbXc4d");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "1a2b3c");

		{
			test_expect_call("jstr_re_rplcall_len_exec", (long long)(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "NUM", 3, 0)), (long long)(3), &result, "NUMaNUMbNUMc");
			test_reset(&result);
		};

		test_shrink_fill(&result, "abc");

		{
			test_expect_call("jstr_re_rplcall_len_exec", (long long)(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "NUM", 3, 0)), (long long)(0), &result, "abc");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "1a2b3c4d");

		{
			test_expect_call("jstr_re_rplcall_len_from_exec", (long long)(jstr_re_rplcall_len_from_exec(&preg, jstr_struct(&result), 2, "X", 1, 0)), (long long)(3), &result, "1aXbXcXd");
			test_reset(&result);
		};

		test_shrink_fill(&result, "1a2b3c4d");

		{
			test_expect_call("jstr_re_rplcall_len_from_exec", (long long)(jstr_re_rplcall_len_from_exec(&preg, jstr_struct(&result), 0, "X", 1, 0)), (long long)(4), &result, "XaXbXcXd");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]\\{1,\\}", 0)));

		test_shrink_fill(&result, "hello world");

		{
			test_expect_call("jstr_re_rplcall_len_exec", (long long)(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "word", 4, 0)), (long long)(2), &result, "word word");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "_\\{1,\\}", 0)));

		test_shrink_fill(&result, "a_b_c");

		{
			test_expect_call("jstr_re_rplcall_len_exec", (long long)(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), " ", 1, 0)), (long long)(2), &result, "a b c");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "abc123def");

		{
			test_expect_call("jstr_re_rplc_len_exec", (long long)(jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "", 0, 0)), (long long)(1), &result, "abcdef");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]\\{1,\\}", 0)));

		test_shrink_fill(&result, "a b c");

		{
			test_expect_call("jstr_re_rplcn_len_exec", (long long)(jstr_re_rplcn_len_exec(&preg, jstr_struct(&result), "long", 4, 0, 2)), (long long)(2), &result, "long long c");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]\\{4,\\}", 0)));

		test_shrink_fill(&result, "hello_world_foo");

		{
			test_expect_call("jstr_re_rplcall_len_exec", (long long)(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "x", 1, 0)), (long long)(2), &result, "x_x_foo");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]+", JSTR_RE_CF_EXTENDED)));

		test_shrink_fill(&result, "a1b2c3");

		{
			test_expect_call("jstr_re_rplcall_len_exec", (long long)(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "X", 1, 0)), (long long)(3), &result, "X1X2X3");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "colou?r", JSTR_RE_CF_EXTENDED)));

		test_shrink_fill(&result, "color colour");

		{
			test_expect_call("jstr_re_rplcall_len_exec", (long long)(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "X", 1, 0)), (long long)(2), &result, "X X");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "hello", JSTR_RE_CF_ICASE)));

		test_shrink_fill(&result, "Hello HELLO hello");

		{
			test_expect_call("jstr_re_rplcall_len_exec", (long long)(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "hi", 2, 0)), (long long)(3), &result, "hi hi hi");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		/* rplc_len < find_len: replacement shorter than match. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]\\{4,\\}", 0)));

		test_shrink_fill(&result, "xxxx");
		{
			test_expect_call("jstr_re_rplc_len_exec", (long long)(jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "y", 1, 0)), (long long)(1), &result, "y");
			test_reset(&result);
		};

		test_shrink_fill(&result, "aaaa_bbbb");
		{
			test_expect_call("jstr_re_rplcall_len_exec", (long long)(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "x", 1, 0)), (long long)(2), &result, "x_x");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		/* rplc_len > find_len: replacement longer than match. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "a1b");
		{
			test_expect_call("jstr_re_rplc_len_exec", (long long)(jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "long", 4, 0)), (long long)(1), &result, "alongb");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		/* rplc_len == find_len: same size replacement. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "a1b2c");
		{
			test_expect_call("jstr_re_rplcall_len_exec", (long long)(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "X", 1, 0)), (long long)(2), &result, "aXbXc");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		/* _from_exec with start_idx == sz (boundary). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "abc");
		{
			test_expect_call("jstr_re_rplc_len_from_exec", (long long)(jstr_re_rplc_len_from_exec(&preg, jstr_struct(&result), 3, "X", 1, 0)), (long long)(0), &result, "abc");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		/* _from_exec with start_idx > sz. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "abc");
		{
			test_expect_call("jstr_re_rplc_len_from_exec", (long long)(jstr_re_rplc_len_from_exec(&preg, jstr_struct(&result), 99, "X", 1, 0)), (long long)(0), &result, "abc");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		/* rplcn_from_exec with start_idx > sz. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "1a2b3c");
		{
			test_expect_call("jstr_re_rplcn_len_from_exec", (long long)(jstr_re_rplcn_len_from_exec(&preg, jstr_struct(&result), 99, "X", 1, 0, (size_t)-1)), (long long)(0), &result, "1a2b3c");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		/* rplcall_from_exec with start_idx > sz. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "1a2b3c");
		{
			test_expect_call("jstr_re_rplcall_len_from_exec", (long long)(jstr_re_rplcall_len_from_exec(&preg, jstr_struct(&result), 99, "X", 1, 0)), (long long)(0), &result, "1a2b3c");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		/* Empty haystack. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "");
		{
			test_expect_call("jstr_re_rplc_len_exec", (long long)(jstr_re_rplc_len_exec(&preg, jstr_struct(&result), "X", 1, 0)), (long long)(0), &result, "");
			test_reset(&result);
		};
		{
			test_expect_call("jstr_re_rplcall_len_exec", (long long)(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "X", 1, 0)), (long long)(0), &result, "");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		/* rplcn with n=0 (no replacement). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "1a2b3c");
		{
			test_expect_call("jstr_re_rplcn_len_exec", (long long)(jstr_re_rplcn_len_exec(&preg, jstr_struct(&result), "X", 1, 0, 0)), (long long)(0), &result, "1a2b3c");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		/* rplcn with n=1 (single replacement via rplcn path). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "1a2b3c");
		{
			test_expect_call("jstr_re_rplcn_len_exec", (long long)(jstr_re_rplcn_len_exec(&preg, jstr_struct(&result), "X", 1, 0, 1)), (long long)(1), &result, "Xa2b3c");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		/* rplcn with n > matches (should replace all). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{1,\\}", 0)));

		test_shrink_fill(&result, "1a2");
		{
			test_expect_call("jstr_re_rplcn_len_exec", (long long)(jstr_re_rplcn_len_exec(&preg, jstr_struct(&result), "X", 1, 0, 99)), (long long)(2), &result, "XaX");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		/* Multiple digits vs single digit replacement (rplc_len < find_len). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[0-9]\\{2,\\}", 0)));

		test_shrink_fill(&result, "ab12cd34e");
		{
			test_expect_call("jstr_re_rplcall_len_exec", (long long)(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "X", 1, 0)), (long long)(2), &result, "abXcdXe");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		/* Single char match replaced with multiple chars (rplc_len > find_len). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "[a-z]", 0)));

		test_shrink_fill(&result, "a1b2c");
		{
			test_expect_call("jstr_re_rplcall_len_exec", (long long)(jstr_re_rplcall_len_exec(&preg, jstr_struct(&result), "long", 4, 0)), (long long)(3), &result, "long1long2long");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	{
		/* Pattern with boundary assertions and NOTBOL. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "^hello", 0)));

		test_shrink_fill(&result, "hello hello");
		{
			test_expect_call("jstr_re_rplc_len_from_exec", (long long)(jstr_re_rplc_len_from_exec(&preg, jstr_struct(&result), 6, "X", 1, JSTR_RE_EF_NOTBOL)), (long long)(0), &result, "hello hello");
			test_reset(&result);
		};

		jstr_re_free(&preg);
	}
	jstr_free_j(&result);
	return 0;
}
