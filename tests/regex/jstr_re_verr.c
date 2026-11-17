/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: va_list variant of the regex error printer. jstr_re_verr is static
 * in the implementation, hence JSTR_IMPLEMENTATION is defined here. */

#define JSTR_IMPLEMENTATION
#define __STDC_NO_VLA__ 1

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>

#include "../rigor.h"
#include "regex.h"

static int
call_verr(jstr_re_ty *re, jstr_re_ret_ty code, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int r = jstr_re_verr(code, re, fmt, ap);
	va_end(ap);
	return r;
}

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
	RCHECK(jstr_re_chkcomp(jstr_re_comp(&re, "[", JSTR_RE_CF_EXTENDED)));

	char buf[512];
	int saved;
	FILE *f = capture_begin(&saved);
	RCHECK(f != NULL);
	RCHECK_EQ(call_verr(&re, JSTR_RE_RET_EBRACK, "detail=%d\n", 7), 0);
	capture_end(f, saved, buf, sizeof(buf));
	RCHECK(strstr(buf, "POSIX regex error") != NULL);
	RCHECK(strstr(buf, "detail=7") != NULL);

	f = capture_begin(&saved);
	RCHECK(f != NULL);
	RCHECK_EQ(call_verr(&re, JSTR_RE_RET_NOERROR, "%s-%s\n", "a", "b"), 0);
	capture_end(f, saved, buf, sizeof(buf));
	RCHECK(strstr(buf, "a-b") != NULL);

	/* Write failure is reported as -1. */
	const int err_saved = dup(STDERR_FILENO);
	RCHECK(err_saved >= 0);
	close(STDERR_FILENO);
	RCHECK_EQ(call_verr(&re, JSTR_RE_RET_EBRACK, "nope\n"), -1);
	dup2(err_saved, STDERR_FILENO);
	close(err_saved);

	jstr_re_free(&re);
	return EXIT_SUCCESS;
}
