/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Bounds-safety tests for the regex backreference replacement engine:
 * - A trailing lone '\' in the replacement must be emitted literally,
 *   not read past the end of the replacement buffer.
 * - Backreferences to groups that did not participate in the match
 *   expand as empty strings, never as (size_t)-2 arithmetic.
 * - Backreferences >= nmatch expand as empty strings (no uninitialized
 *   rm[] access in release builds). */

#define __STDC_NO_VLA__ 1

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../include/regex.h"

int
main(void)
{
	jstr_ty result = JSTR_INIT;

	{
		/* Trailing lone backslash in replacement: "aa" -> "X<group>Y\". */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\(a\\)", 0)));

		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "aa", 2)));
		jstr_re_off_ty rn = jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "X\\1Y\\", 5, 0, 2);
		assert(rn == 2);
		assert(result.size == strlen("XaY\\XaY\\"));
		assert(memcmp(result.data, "XaY\\XaY\\", result.size + 1) == 0);

		jstr_re_free(&preg);
	}

	{
		/* Backref to an absent group (pattern has no groups): the pair
		 * must degrade to literal text with matching length accounting. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "a", 0)));

		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "aa", 2)));
		jstr_re_off_ty rn = jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "[\\3]", 4, 0, 2);
		assert(rn == 2);
		assert(result.size == strlen("[\\3][\\3]"));
		assert(memcmp(result.data, "[\\3][\\3]", result.size + 1) == 0);

		jstr_re_free(&preg);
	}

	{
		/* Non-participating group degrades to literal text (chosen
		 * semantics; POSIX unspecified): pattern "(a)|(b)" on "b",
		 * replacement "[\1\2]" -> "[" + "\1"(literal) + "b" + "]". */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\(a\\)\\|\\(b\\)", 0)));

		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "b", 1)));
		jstr_re_off_ty rn = jstr_re_rplcall_backref_len_exec(&preg, jstr_struct(&result), "[\\1\\2]", 6, 0, 3);
		assert(rn == 1);
		assert(result.size == strlen("[\\1b]"));
		assert(memcmp(result.data, "[\\1b]", result.size + 1) == 0);

		jstr_re_free(&preg);
	}

	{
		/* Backref index >= nmatch degrades to literal text: replacement
		 * "\9" with nmatch=2 must not read rm[9]. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\(a\\)", 0)));

		assert(!jstr_chk(jstr_assign_len(jstr_struct(&result), "a", 1)));
		jstr_re_off_ty rn = jstr_re_rplc_backref_len_exec(&preg, jstr_struct(&result), "<\\9>", 4, 0, 2);
		assert(rn == 1);
		assert(result.size == strlen("<\\9>"));
		assert(memcmp(result.data, "<\\9>", result.size + 1) == 0);

		jstr_re_free(&preg);
	}

	jstr_free_j(&result);
	return EXIT_SUCCESS;
}
