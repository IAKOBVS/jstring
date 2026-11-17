/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

static int
call_vsprintf_maxlen(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int r = jstr_vsprintf_maxlen(ap, fmt);
	va_end(ap);
	return r;
}

/* Verify the advertised maximum is the exact output length plus the NUL. */
static void
check_maxlen(int expected, const char *fmt, ...)
{
	va_list ap;
	char buf[8192];

	va_start(ap, fmt);
	const int got = jstr_vsprintf_maxlen(ap, fmt);
	va_end(ap);

	va_start(ap, fmt);
	const int wrote = vsnprintf(buf, sizeof buf, fmt, ap);
	va_end(ap);

	assert(wrote >= 0);
	assert((size_t)wrote < sizeof buf);
	assert(got == expected);
	assert((size_t)got == strlen(buf) + 1);
	assert(buf[wrote] == '\0');
}

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

	/* The result always includes the NUL terminator. */
	check_maxlen(1, "");
	check_maxlen(4, "abc");
	check_maxlen(7, "num=%d", 42);
	check_maxlen(6, "%s", "hello");
	check_maxlen(2, "%c", 'A');
	check_maxlen(2, "%%");
	check_maxlen(3, "%x", 255u);
	check_maxlen(6, "%05d", 42);
	check_maxlen(9, "%-8s", "x");
	check_maxlen(4, "%.3s", "abcdef");
	check_maxlen(4, "%+d", 42);
	check_maxlen(21, "%lld", (long long)LLONG_MIN);
	check_maxlen(11, "%u", UINT_MAX);

	/* A long expansion must be counted without truncation. */
	{
		char big[3001];
		memset(big, 'q', sizeof big - 1);
		big[sizeof big - 1] = '\0';
		check_maxlen(3001, "%s", big);
		check_maxlen(3002, "%s%s", "ab", big + 1);
	}

	/* Repeated calls must be independent and stable. */
	for (int i = 0; i < 25; ++i) {
		assert(call_vsprintf_maxlen("n=%d", i) == (i < 10 ? 4 : (i < 100 ? 5 : 6)));
	}

	/* asprintf consumes the same max length and produces an exact buffer. */
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(jstr_asprintf(&s, &sz, &cap, "%s-%d", "abc", 42) == JSTR_RET_SUCC);
		assert(sz == strlen("abc-42"));
		assert(strlen(s) == sz);
		assert(s[sz] == '\0');
		assert(strcmp(s, "abc-42") == 0);
		assert(cap > sz);

		assert(jstr_asprintf(&s, &sz, &cap, "%s", "replaced") == JSTR_RET_SUCC);
		assert(sz == strlen("replaced"));
		assert(strcmp(s, "replaced") == 0);
		assert(s[sz] == '\0');

		jstr_free(&s, &sz, &cap);
		assert(s == NULL && sz == 0 && cap == 0);
	}

	/* A trailing conversion specifier is an error, not a bogus length. */
	errno = 0;
	assert(call_vsprintf_maxlen("%") < 0);
	assert(errno == EINVAL);

	SUCCESS();
	return EXIT_SUCCESS;
}
