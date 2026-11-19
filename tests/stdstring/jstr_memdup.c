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
	unsigned char src[SZ];
	rigor_srand(0x1234u + n);
	rigor_fill(src, n, 1); /* embedded NULs allowed */

	void *p = jstr_memdup(src, n);
	if (n == 0) {
		free(p); /* malloc(0) may return NULL or a unique pointer */
		return;
	}
	assert(p != NULL);
	assert(p != (void *)src);
	assert(memcmp(p, src, n) == 0);

	/* The copy must be independent of the source. */
	((unsigned char *)p)[0] ^= 0xffu;
	assert(((unsigned char *)p)[0] != src[0]);
	assert(memcmp((unsigned char *)p + 1, src + 1, n - 1) == 0);
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

	/* all-zero and all-0xff payloads. */
	{
		unsigned char src[64];
		memset(src, 0x00, sizeof src);
		void *p = jstr_memdup(src, sizeof src);
		assert(p != NULL);
		assert(memcmp(p, src, sizeof src) == 0);
		free(p);

		memset(src, 0xff, sizeof src);
		p = jstr_memdup(src, sizeof src);
		assert(p != NULL);
		assert(memcmp(p, src, sizeof src) == 0);
		free(p);
	}

	/* n == 0 needs no dereference of the source. */
	{
		void *p = jstr_memdup(NULL, 0);
		free(p);
	}

	return 0;
}
