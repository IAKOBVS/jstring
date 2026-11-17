/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: jstr_re_err formatting into stderr plus its I/O failure path. */

#define __STDC_NO_VLA__ 1

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>

#include "../rigor.h"
#include "regex.h"

static FILE *
capture_begin(int *saved)
{
	FILE *f = tmpfile();
	if (!f)
		return NULL;
	fflush(stderr);
	*saved = dup(STDERR_FILENO);
	if (*saved < 0) {
		fclose(f);
		return NULL;
	}
	if (dup2(fileno(f), STDERR_FILENO) < 0) {
		close(*saved);
		fclose(f);
		return NULL;
	}
	return f;
}

static size_t
capture_end(FILE *f, int saved, char *buf, size_t cap)
{
	fflush(stderr);
	dup2(saved, STDERR_FILENO);
	close(saved);
	rewind(f);
	size_t n = fread(buf, 1, cap - 1, f);
	buf[n] = '\0';
	fclose(f);
	return n;
}

int
main(void)
{
	jstr_re_ty re;
	memset(&re, 0, sizeof(re));
	RCHECK(jstr_re_chkcomp(jstr_re_comp(&re, "\\", JSTR_RE_CF_EXTENDED)));

	char buf[512];
	int saved;
	FILE *f = capture_begin(&saved);
	RCHECK(f != NULL);
	const int rc = jstr_re_err(JSTR_RE_RET_EESCAPE, &re, "ctx %d/%s\n", 42, "x");
	const size_t n = capture_end(f, saved, buf, sizeof(buf));
	RCHECK_EQ(rc, 0);
	RCHECK(n > 0);
	RCHECK(strstr(buf, "POSIX regex error") != NULL);
	RCHECK(strstr(buf, "ctx 42/x") != NULL);
	RCHECK(strchr(buf, '\n') != NULL);

	/* NOERROR is a valid (if unusual) code to report. */
	f = capture_begin(&saved);
	RCHECK(f != NULL);
	RCHECK_EQ(jstr_re_err(JSTR_RE_RET_NOERROR, &re, "plain %s\n", "ok"), 0);
	capture_end(f, saved, buf, sizeof(buf));
	RCHECK(strstr(buf, "plain ok") != NULL);

	/* Closing stderr makes the underlying write fail: -1, no abort. */
	const int err_saved = dup(STDERR_FILENO);
	RCHECK(err_saved >= 0);
	close(STDERR_FILENO);
	const int fail_rc = jstr_re_err(JSTR_RE_RET_EESCAPE, &re, "unwritable %d\n", 1);
	dup2(err_saved, STDERR_FILENO);
	close(err_saved);
	RCHECK_EQ(fail_rc, -1);

	jstr_re_free(&re);
	return EXIT_SUCCESS;
}
