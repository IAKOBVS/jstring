/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Fixes regex.h problems with tcc */
#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/regex.h"
#include <unistd.h>
#include <sys/wait.h>

int
main(int argc, char **argv)
{
	START();
	{
		/* jstr_re_err with invalid pattern error code.
		 * Covers jstr_re_verr (lines 187-196) and jstr_re_err (lines 205-212). */
		jstr_re_ty preg;
		jstr_re_ret_ty err = jstr_re_comp(&preg, "\\", JSTR_RE_CF_EXTENDED);
		assert(jstr_re_chkcomp(err));
		assert(err != JSTR_RE_RET_NOERROR);
		assert(jstr_re_err(err, &preg, "error code: %d\n", (int)err) == -1);
		jstr_re_free(&preg);
	}
	{
		/* jstr_re_errdie via fork (covers lines 223-230).
		 * Child process calls erddie which should exit(EXIT_FAILURE). */
		jstr_re_ty preg;
		jstr_re_ret_ty err = jstr_re_comp(&preg, "\\", JSTR_RE_CF_EXTENDED);
		assert(jstr_re_chkcomp(err));
		pid_t pid = fork();
		if (pid == 0) {
			jstr_re_errdie(err, &preg, "dying: %d", 99);
			_exit(0);
		}
		int status;
		assert(waitpid(pid, &status, 0) == pid);
		assert(WIFEXITED(status));
		assert(WEXITSTATUS(status) == EXIT_FAILURE);
		jstr_re_free(&preg);
	}
	{
		/* IS_NOTBOL returning 0 after newline with REG_NEWLINE (line 138).
		 * start_idx=4, cflags=REG_NEWLINE, char before idx is \n,
		 * so IS_NOTBOL returns 0 (not NOTBOL) and ^hello matches. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "^hello", JSTR_RE_CF_EXTENDED | JSTR_RE_CF_NEWLINE)));
		jstr_ty s = JSTR_INIT;
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&s), "xyz\nhello", 9)));
		assert(jstr_re_rm_from_exec(&preg, jstr_struct(&s), 4, 0) == 1);
		assert(s.size == 4);
		assert(memcmp(s.data, "xyz\n", 4) == 0);
		jstr_free_j(&s);
		jstr_re_free(&preg);
	}
	{
		/* IS_NOTBOL_INLOOP returning 0 after newline (line 154).
		 * String "xa\nab\na\nc", rmall with pattern "a\n" + REG_NEWLINE.
		 * First match "a\n" at absolute pos1-2. After removal, loop
		 * searches from pos3. *(pos3-1)=pos2='\n', so IS_NOTBOL_INLOOP
		 * returns 0 (not NOTBOL). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "a\n", JSTR_RE_CF_EXTENDED | JSTR_RE_CF_NEWLINE)));
		jstr_ty s = JSTR_INIT;
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&s), "xa\nab\na\nc", 9)));
		assert(jstr_re_rmall_exec(&preg, jstr_struct(&s), 0) == 2);
		assert(s.size == 5);
		assert(memcmp(s.data, "xab\nc", 5) == 0);
		jstr_free_j(&s);
		jstr_re_free(&preg);
	}
	{
		/* find_len == 0 edge case in rmn_from_exec (line 455).
		 * Pattern () matches empty string, triggering zero-length match. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "()", JSTR_RE_CF_EXTENDED)));
		jstr_ty s = JSTR_INIT;
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&s), "abc", 3)));
		assert(jstr_re_rmn_exec(&preg, jstr_struct(&s), 0, 2) == 2);
		assert(s.size == 3);
		assert(memcmp(s.data, "abc", 3) == 0);
		jstr_free_j(&s);
		jstr_re_free(&preg);
	}
	{
		/* find_len == 0 edge case in rplcn_backref_len_from_exec (line 787).
		 * Pattern () matches empty string. */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "()", JSTR_RE_CF_EXTENDED)));
		jstr_ty s = JSTR_INIT;
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&s), "xyz", 3)));
		assert(jstr_re_rplcn_len_exec(&preg, jstr_struct(&s), "!", 1, 0, 2) == 2);
		assert(s.size == 5);
		assert(memcmp(s.data, "!x!yz", 5) == 0);
		jstr_free_j(&s);
		jstr_re_free(&preg);
	}
	{
		/* rplcbackreflast loop with \1\a (no trailing backref,
		 * searches from end and iterates). */
		jstr_re_ty preg;
		assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\([a-z]\\{1,\\}\\)", 0)));
		jstr_ty s = JSTR_INIT;
		assert(!jstr_chk(jstr_assign_len(jstr_struct(&s), "abc_def", 7)));
		assert(jstr_re_rplc_backref_len_exec(&preg, jstr_struct(&s), "\\1\\a", 4, 0, 2) == 1);
		assert(s.size == 9);
		assert(memcmp(s.data, "abc\\a_def", 9) == 0);
		jstr_free_j(&s);
		jstr_re_free(&preg);
	}
	SUCCESS();
	return 0;
	(void)argc;
	(void)argv;
}
