/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static int
ws(int c)
{
	return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

static size_t
ref_trimend(const char *s, size_t n)
{
	while (n && ws((unsigned char)s[n - 1]))
		--n;
	return n;
}

int
main(void)
{
	static const char *const cases[] = { "", "abc", "   ", "  abc", "abc  ",
	                                     "  abc  ", "a\tb\r\n", " \t\n\v\f\r ",
	                                     "abc" };
	char buf[1200], want[1200];

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
		const char *c = cases[i];
		const size_t cl = strlen(c);
		const size_t wl = ref_trimend(c, cl);
		memcpy(want, c, cl + 1);
		want[wl] = '\0';
		memcpy(buf, c, cl + 1);
		char *p = jstr_trimend_len_p(buf, cl);
		RCHECK(p == buf + wl);
		RCHECK(!strcmp(buf, want));
	}
	{
		char b[4] = "";
		char *p = jstr_trimend_len_p(b, 0);
		RCHECK(p == b);
		RCHECK(b[0] == '\0');
	}

	rigor_srand(0x74134eu);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		for (size_t i = 0; i < len; ++i)
			buf[i] = (rigor_rand() & 3u) ? 'a' : " \t\n\v\f\r"[rigor_randn(6)];
		buf[len] = 0;
		const size_t wl = ref_trimend(buf, len);
		memcpy(want, buf, len + 1);
		want[wl] = '\0';
		char *p = jstr_trimend_len_p(buf, len);
		RCHECK(p == buf + wl);
		RCHECK(!memcmp(buf, want, wl + 1));
	}

	return 0;
}
