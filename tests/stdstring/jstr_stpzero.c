/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stddef.h>
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
	unsigned char buf[SZ];
	unsigned char ref[SZ];

	rigor_srand(0x7777u + n);
	rigor_fill(buf, n, 0); /* non-NUL payload; buf[n] == 0 */
	for (size_t i = n + 1; i < SZ; ++i)
		buf[i] = (unsigned char)(0x91u ^ (unsigned)i);

	memcpy(ref, buf, SZ);
	memset(ref, 0, n); /* NUL at [n] is already there and preserved */

	char *r = jstr_stpzero((char *)buf);
	assert(r == (char *)buf + n);
	assert(memcmp(buf, ref, SZ) == 0);
}

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	for (size_t n = 0; n <= 128; ++n)
		check_len(n);
#if JSTR_TEST_SLOW
	for (size_t i = 0; i < RIGOR_NLENS; ++i)
		check_len(rigor_lens[i]);
#endif

	/* Empty string: no bytes are written and the original buffer survives. */
	{
		char buf[8] = { 0, 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
		char *r = jstr_stpzero(buf);
		assert(r == buf);
		assert(buf[0] == 0);
		assert(memcmp(buf + 1, "bcdefgh", 7) == 0);
	}

	/* all-0xff payload. */
	{
		char buf[64];
		memset(buf, (char)0xff, sizeof buf);
		buf[16] = 0;
		char *r = jstr_stpzero(buf);
		assert(r == buf + 16);
		for (size_t i = 0; i < 16; ++i)
			assert(buf[i] == 0);
		assert(buf[16] == 0);
		for (size_t i = 17; i < sizeof buf; ++i)
			assert((unsigned char)buf[i] == 0xff);
	}

	return 0;
}
