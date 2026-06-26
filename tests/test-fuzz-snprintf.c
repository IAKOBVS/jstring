/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Fuzz test for sprintf/asprintf variants with format strings and random
 * arguments. */

#define __STDC_NO_VLA__ 1

#include "test.h"
#include "../include/jstr.h"
#include "fuzz-shared.h"

#include <stdio.h>

struct fmt_spec {
	const char *fmt;
	int nargs;
	enum { ARG_INT, ARG_STR } types[4];
};

static const struct fmt_spec formats[] = {
	{ "%d",               1, {ARG_INT} },
	{ "%u",               1, {ARG_INT} },
	{ "%x",               1, {ARG_INT} },
	{ "%d%s",             2, {ARG_INT, ARG_STR} },
	{ "%s%d",             2, {ARG_STR, ARG_INT} },
	{ "|%d|%u|",          2, {ARG_INT, ARG_INT} },
	{ "[%s]",             1, {ARG_STR} },
	{ "%s%s",             2, {ARG_STR, ARG_STR} },
	{ "pre%dmid%ssuf",    2, {ARG_INT, ARG_STR} },
	{ "%d,%d,%d",         3, {ARG_INT, ARG_INT, ARG_INT} },
	{ "%d%%%d",           2, {ARG_INT, ARG_INT} },
};
static const size_t nformats = sizeof(formats) / sizeof(formats[0]);

static const char *str_args[] = {"", "x", "hello", "AB", "longer_string", "123"};
static const size_t nstrargs = sizeof(str_args) / sizeof(str_args[0]);

JSTR_ATTR_MAYBE_UNUSED
static void
fuzz_snprintf(size_t iter)
{
	char ref_buf[1024];

	for (size_t i = 0; i < iter; ++i) {
		const struct fmt_spec *fs = &formats[rand() % nformats];

		/* Build variadic argument list values */
		int iargs[4];
		const char *sargs[4];
		for (int j = 0; j < fs->nargs; ++j) {
			if (fs->types[j] == ARG_INT)
				iargs[j] = (int)(rand() & 0xFFFF) - (int)(rand() & 0x7FFF);
			else
				sargs[j] = str_args[rand() % nstrargs];
		}

		/* Build call based on argument types.
		 * We test jstr_asprintf (full replacement) and compare to snprintf. */
		char *s = NULL;
		size_t sz = 0, cap = 0;
		int ret;
		switch (fs->nargs) {
		case 1:
			if (fs->types[0] == ARG_INT) {
				ret = jstr_asprintf(&s, &sz, &cap, fs->fmt, iargs[0]);
				snprintf(ref_buf, sizeof(ref_buf), fs->fmt, iargs[0]);
			} else {
				ret = jstr_asprintf(&s, &sz, &cap, fs->fmt, sargs[0]);
				snprintf(ref_buf, sizeof(ref_buf), fs->fmt, sargs[0]);
			}
			break;
		case 2:
			if (fs->types[0] == ARG_INT && fs->types[1] == ARG_INT) {
				ret = jstr_asprintf(&s, &sz, &cap, fs->fmt, iargs[0], iargs[1]);
				snprintf(ref_buf, sizeof(ref_buf), fs->fmt, iargs[0], iargs[1]);
			} else if (fs->types[0] == ARG_INT) {
				ret = jstr_asprintf(&s, &sz, &cap, fs->fmt, iargs[0], sargs[1]);
				snprintf(ref_buf, sizeof(ref_buf), fs->fmt, iargs[0], sargs[1]);
			} else if (fs->types[1] == ARG_INT) {
				ret = jstr_asprintf(&s, &sz, &cap, fs->fmt, sargs[0], iargs[1]);
				snprintf(ref_buf, sizeof(ref_buf), fs->fmt, sargs[0], iargs[1]);
			} else {
				ret = jstr_asprintf(&s, &sz, &cap, fs->fmt, sargs[0], sargs[1]);
				snprintf(ref_buf, sizeof(ref_buf), fs->fmt, sargs[0], sargs[1]);
			}
			break;
		case 3:
			ret = jstr_asprintf(&s, &sz, &cap, fs->fmt, iargs[0], iargs[1], iargs[2]);
			snprintf(ref_buf, sizeof(ref_buf), fs->fmt, iargs[0], iargs[1], iargs[2]);
			break;
		default:
			ret = JSTR_RET_ERR;
			break;
		}
		(void)ret;
		if (s) {
			assert(strcmp(s, ref_buf) == 0);
			assert(sz == strlen(s));
			free(s);
		}
	}

	/* Test asprintf_append with simple formats */
	for (size_t i = 0; i < iter / 10 + 1; ++i) {
		char *s = NULL;
		size_t sz = 0, cap = 0;
		int ret = jstr_assign_len(&s, &sz, &cap, "base", 4);
		assert(ret == JSTR_RET_SUCC);
		ret = jstr_asprintf_append(&s, &sz, &cap, "|%d|", 7);
		assert(ret == JSTR_RET_SUCC);
		assert(strcmp(s, "base|7|") == 0);
		assert(sz == 7);
		free(s);
	}
}

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
	iter *= 10;
#else
	if (iter > 500)
		iter = 500;
#endif
	if (iter < 1)
		iter = 1;
#ifdef FUZZ_AFL
	while (__AFL_LOOP(10000))
#endif
	{
		srand(seed);
		TESTING(fuzz_snprintf);
		fuzz_snprintf(iter);
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
	fuzz_snprintf(iter);
	return 0;
}

#endif
