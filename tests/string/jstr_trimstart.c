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
ref_lead(const char *s, size_t n)
{
	size_t i = 0;
	while (i < n && ws((unsigned char)s[i]))
		++i;
	return i;
}

int
main(void)
{
	static const char *const cases[] = { "", "abc", "   ", "  abc", "abc  ",
	                                     "  abc  ", " \t\nabc", " \t\n\v\f\r " };
	char buf[1200], want[1200];

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
		const char *c = cases[i];
		const size_t cl = strlen(c);
		const size_t lead = ref_lead(c, cl);
		const size_t wl = cl - lead;
		memcpy(want, c + lead, wl);
		want[wl] = '\0';
		strcpy(buf, c);
		jstr_trimstart(buf);
		RCHECK(!strcmp(buf, want));
	}

	rigor_srand(0x75139eu);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		for (size_t i = 0; i < len; ++i)
			buf[i] = (rigor_rand() & 3u) ? 'a' : " \t\n\v\f\r"[rigor_randn(6)];
		buf[len] = 0;
		const size_t lead = ref_lead(buf, len);
		memcpy(want, buf + lead, len - lead);
		want[len - lead] = '\0';
		jstr_trimstart(buf);
		RCHECK(!strcmp(buf, want));
	}

	return 0;
}
