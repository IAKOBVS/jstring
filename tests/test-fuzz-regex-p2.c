/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Fuzz test for regex functions — patterns 15-21 only. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/regex.h"
#include "fuzz-regex.h"

static const struct regex_pattern patterns[] = {
	{ "^hello",                              JSTR_RE_CF_ICASE,        0 },
	{ "[a-z]+",                              JSTR_RE_CF_ICASE,        0 },
	{ "colou?r",                             JSTR_RE_CF_EXTENDED | JSTR_RE_CF_ICASE, 0 },
	{ "[a-z]+",                              JSTR_RE_CF_NEWLINE,      0 },
	{ "\\([a-z]\\{1,\\}\\)",                0,                       1 },
	{ "\\([0-9]\\{1,\\}\\)",                0,                       1 },
	{ "\\([a-z]\\{1,\\}\\)_\\([a-z]\\{1,\\}\\)", 0,                1 },
};
static const size_t npatterns = sizeof(patterns) / sizeof(patterns[0]);

int
main(int argc, char **argv)
{
	START();
	unsigned seed = 0;
	if (argc > 1)
		seed = (unsigned)atoi(argv[1]);
	size_t iter = FITER;
#ifdef JSTR_TEST_SLOW
	iter *= 2;
#else
	if (iter > 50)
		iter = 50;
#endif
	if (iter < 1)
		iter = 1;
	{
		srand(seed);
		TESTING(fuzz_regex_p2);
		fuzz_regex_patterns(iter, patterns, npatterns);
	}
	SUCCESS();
	return EXIT_SUCCESS;
}
