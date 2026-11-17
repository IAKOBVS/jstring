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
ref_trim(const char *s, size_t n, size_t *lead_out)
{
	size_t e = n;
	while (e && ws((unsigned char)s[e - 1]))
		--e;
	size_t l = 0;
	while (l < e && ws((unsigned char)s[l]))
		++l;
	*lead_out = l;
	return e - l;
}

int
main(void)
{
	static const char *const cases[] = { "", "abc", "   ", "  abc", "abc  ",
	                                     "  abc  ", " \t\nabc\r\n", " \t\n\v\f\r " };
	char buf[1200], want[1200];

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
		const char *c = cases[i];
		const size_t cl = strlen(c);
		size_t lead;
		const size_t wl = ref_trim(c, cl, &lead);
		memcpy(want, c + lead, wl);
		want[wl] = '\0';
		strcpy(buf, c);
		char *p = jstr_trim_p(buf);
		RCHECK(p == buf + wl);
		RCHECK(!strcmp(buf, want));
	}

	rigor_srand(0x76124eu);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		for (size_t i = 0; i < len; ++i)
			buf[i] = (rigor_rand() & 3u) ? 'a' : " \t\n\v\f\r"[rigor_randn(6)];
		buf[len] = 0;
		size_t lead;
		const size_t wl = ref_trim(buf, len, &lead);
		memcpy(want, buf + lead, wl);
		want[wl] = '\0';
		char *p = jstr_trim_p(buf);
		RCHECK(p == buf + wl);
		RCHECK(!strcmp(buf, want));
	}

	return 0;
}
