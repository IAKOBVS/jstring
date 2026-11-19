/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"
#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

#define SZ 1400

static void
check_len(size_t n)
{
	char src[SZ];
	rigor_srand(0x9999u + n);
	rigor_fill(src, n, 0); /* non-NUL payload, NUL at [n] */

	char *p = jstr_strdup(src);
	assert(p != NULL);
	assert(p != src);
	assert(strlen(p) == n);
	assert(p[n] == 0);
	assert(memcmp(p, src, n) == 0);

	/* Differential against libc strdup. */
	{
		char *lib = strdup(src);
		assert(lib != NULL);
		assert(strcmp(p, lib) == 0);
		assert(strlen(p) == strlen(lib));
		free(lib);
	}

	/* The result is an independent allocation. */
	if (n) {
		p[0] ^= 0x5a;
		assert(p[0] != src[0]);
	}
	free(p);
}

int
main(void)
{

	for (size_t n = 0; n <= 128; ++n)
		check_len(n);
#if JSTR_TEST_SLOW
	for (size_t i = 0; i < RIGOR_NLENS; ++i)
		check_len(rigor_lens[i]);
#endif

	/* Empty string yields a non-NULL, empty, heap-allocated copy. */
	{
		char *p = jstr_strdup("");
		assert(p != NULL);
		assert(p[0] == 0);
		assert(strlen(p) == 0);
		free(p);
	}

	/* strdup stops at the first embedded NUL. */
	{
		const char src[] = "ab\0cd";
		char *p = jstr_strdup(src);
		assert(p != NULL);
		assert(strcmp(p, "ab") == 0);
		assert(strlen(p) == 2);
		free(p);
	}

	/* all-0xff payload. */
	{
		char src[64];
		memset(src, (char)0xff, sizeof src);
		src[sizeof src - 1] = 0;
		char *p = jstr_strdup(src);
		assert(p != NULL);
		assert(memcmp(p, src, sizeof src) == 0);
		free(p);
	}

	return 0;
}
