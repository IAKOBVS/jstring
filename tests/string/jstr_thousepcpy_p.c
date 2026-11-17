/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static size_t
ref_thousep(char *out, const char *src, size_t len, char sep)
{
	size_t o = 0, i = 0;
	if (len == 0) {
		out[0] = '\0';
		return 0;
	}
	if (src[0] == '-')
		out[o++] = src[i++];
	const size_t digits = len - i;
	if (digits < 4) {
		while (i < len)
			out[o++] = src[i++];
		out[o] = '\0';
		return o;
	}
	size_t first = digits % 3;
	if (first == 0)
		first = 3;
	size_t k = 0;
	for (; k < first; ++k)
		out[o++] = src[i++];
	while (k < digits) {
		out[o++] = sep;
		for (int j = 0; j < 3; ++j, ++k)
			out[o++] = src[i++];
	}
	out[o] = '\0';
	return o;
}

int
main(void)
{
	static const char *const cases[] = { "", "0", "12", "123", "1234", "12345",
	                                     "123456", "1234567", "-1", "-123", "-1234",
	                                     "-12345", "-1234567", "-", "--1234" };
	char dst[2000], want[2000], src[1300];

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
		const char *c = cases[i];
		const size_t cl = strlen(c);
		size_t wl = ref_thousep(want, c, cl, ',');
		memset(dst, 0x7f, sizeof dst);
		char *p = jstr_thousepcpy_p(dst, c, ',');
		RCHECK(p == dst + wl);
		RCHECK(!memcmp(dst, want, wl + 1));
	}
	{
		char b[32];
		char *p = jstr_thousepcpy_p(b, "1234567", '_');
		RCHECK(p == b + 9);
		RCHECK(!strcmp(b, "1_234_567"));
	}
	{
		char b[4];
		char *p = jstr_thousepcpy_p(b, "", ',');
		RCHECK(p == b);
		RCHECK(b[0] == '\0');
	}

	rigor_srand(0x740060u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 256)
			continue;
		for (size_t i = 0; i < len; ++i)
			src[i] = (char)('0' + rigor_randn(10));
		if (len && (rigor_rand() & 1u))
			src[0] = '-';
		src[len] = 0;
		for (int s = 0; s < 3; ++s) {
			const char sep = ",.'"[s];
			const size_t wl = ref_thousep(want, src, len, sep);
			memset(dst, 0x7f, wl + 4);
			char *p = jstr_thousepcpy_p(dst, src, sep);
			RCHECK(p == dst + wl);
			RCHECK(!memcmp(dst, want, wl + 1));
			RCHECK(dst[wl + 1] == 0x7f);
		}
	}

	return 0;
}
