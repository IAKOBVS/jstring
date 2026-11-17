/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_errdie terminates with EXIT_FAILURE after printing the
 * diagnostic. Run it in a child so the parent can inspect exit status and
 * captured stderr. */

#define __STDC_NO_VLA__ 1

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <regex.h>

#include "../rigor.h"
#include "regex.h"

/* Fork a child that redirects stderr to a pipe and calls jstr_re_errdie.
 * Returns the captured bytes in buf and stores the wait status. */
static void
run_errdie(jstr_re_ret_ty code, const jstr_re_ty *re, char *buf, size_t cap, int *status)
{
	int p[2];
	RCHECK_EQ(pipe(p), 0);
	pid_t pid = fork();
	RCHECK(pid >= 0);
	if (pid == 0) {
		close(p[0]);
		if (dup2(p[1], STDERR_FILENO) < 0)
			_exit(70);
		close(p[1]);
		jstr_re_errdie(code, re, "dying %d/%s\n", 9, "now");
		_exit(0); /* must not be reached */
	}
	close(p[1]);
	size_t used = 0;
	for (;;) {
		char tmp[128];
		ssize_t n = read(p[0], tmp, sizeof(tmp));
		if (n <= 0)
			break;
		if (used + (size_t)n < cap) {
			memcpy(buf + used, tmp, (size_t)n);
			used += (size_t)n;
		}
	}
	buf[used < cap ? used : cap - 1] = '\0';
	close(p[0]);
	RCHECK(waitpid(pid, status, 0) == pid);
}

int
main(void)
{
	jstr_re_ty re;
	memset(&re, 0, sizeof(re));
	RCHECK(jstr_re_chkcomp(jstr_re_comp(&re, "\\", JSTR_RE_CF_EXTENDED)));

	char buf[512];
	int status = 0;
	run_errdie(JSTR_RE_RET_EESCAPE, &re, buf, sizeof(buf), &status);
	RCHECK(WIFEXITED(status));
	RCHECK_EQ(WEXITSTATUS(status), EXIT_FAILURE);
	RCHECK(strstr(buf, "POSIX regex error") != NULL);
	RCHECK(strstr(buf, "dying 9/now") != NULL);

	/* Also from a valid compiled object / NOERROR code: still exits. */
	memset(buf, 0, sizeof(buf));
	status = 0;
	run_errdie(JSTR_RE_RET_NOERROR, &re, buf, sizeof(buf), &status);
	RCHECK(WIFEXITED(status));
	RCHECK_EQ(WEXITSTATUS(status), EXIT_FAILURE);
	RCHECK(strstr(buf, "dying 9/now") != NULL);

	jstr_re_free(&re);
	return EXIT_SUCCESS;
}
