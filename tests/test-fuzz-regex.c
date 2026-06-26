/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Fuzz test for regex functions (all patterns).
 * Split into test-fuzz-regex-p{0,1,2}.c for parallel runs. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/regex.h"
#include "fuzz-regex.h"

static const struct regex_pattern patterns[] = {
	{ "[0-9]",                               0,                       0 },
	{ "[0-9]\\{1,\\}",                       0,                       0 },
	{ "[a-z]\\{1,\\}",                       0,                       0 },
	{ "[a-zA-Z]",                            0,                       0 },
	{ ".",                                   0,                       0 },
	{ ".*",                                  0,                       0 },
	{ "hello",                               0,                       0 },
	{ "^hello",                              0,                       0 },
	{ "world$",                              0,                       0 },
	{ "[a-z]+",                              JSTR_RE_CF_EXTENDED,     0 },
	{ "[0-9]+",                              JSTR_RE_CF_EXTENDED,     0 },
	{ "colou?r",                             JSTR_RE_CF_EXTENDED,     0 },
	{ "(foo|bar)",                           JSTR_RE_CF_EXTENDED,     0 },
	{ "^.",                                  JSTR_RE_CF_EXTENDED,     0 },
	{ "hello",                               JSTR_RE_CF_ICASE,        0 },
	{ "^hello",                              JSTR_RE_CF_ICASE,        0 },
	{ "[a-z]+",                              JSTR_RE_CF_ICASE,        0 },
	{ "colou?r",                             JSTR_RE_CF_EXTENDED | JSTR_RE_CF_ICASE, 0 },
	{ "[a-z]+",                              JSTR_RE_CF_NEWLINE,      0 },
	{ "\\([a-z]\\{1,\\}\\)",                0,                       1 },
	{ "\\([0-9]\\{1,\\}\\)",                0,                       1 },
	{ "\\([a-z]\\{1,\\}\\)_\\([a-z]\\{1,\\}\\)", 0,                1 },
};
static const size_t npatterns = sizeof(patterns) / sizeof(patterns[0]);

#ifndef FUZZ_LIBFUZZER

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
#ifdef FUZZ_AFL
	while (__AFL_LOOP(10000))
#endif
	{
		srand(seed);
		TESTING(fuzz_regex);
		fuzz_regex_patterns(iter, patterns, npatterns);
	}
	SUCCESS();
	return EXIT_SUCCESS;
}

#else

int
LLVMFuzzerTestOneInput(const unsigned char *data, size_t size)
{
	if (size < 4)
		return 0;
	unsigned seed = (unsigned)data[0] | ((unsigned)data[1] << 8) |
	                ((unsigned)data[2] << 16) | ((unsigned)data[3] << 24);
	srand(seed);
	data += 4;
	size -= 4;
	size_t iter = size > 0 ? size : 10;
	if (iter > 100)
		iter = 100;
	fuzz_regex_patterns(iter, patterns, npatterns);
	return 0;
}

#endif
