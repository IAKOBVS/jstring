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
check_len(size_t n, int c)
{
	unsigned char buf[SZ];
	unsigned char ref[SZ];

	for (size_t i = 0; i < SZ; ++i)
		buf[i] = ref[i] = (unsigned char)(0x3cu ^ (unsigned)i);

	char *r = jstr_stpset_len((char *)buf, c, n);
	memset(ref, c, n); /* memset converts c through unsigned char */

	assert(r == (char *)buf + n);
	assert(memcmp(buf, ref, SZ) == 0);
}

int
main(void)
{

	static const int fillers[] = { 0, 'A', 'z', 0xff, 0x100, 0x141, -1, -128, 256 + 0x55 };
	for (size_t k = 0; k < sizeof fillers / sizeof fillers[0]; ++k) {
		for (size_t n = 0; n <= 128; ++n)
			check_len(n, fillers[k]);
#if JSTR_TEST_SLOW
		for (size_t i = 0; i < RIGOR_NLENS; ++i)
			check_len(rigor_lens[i], fillers[k]);
#endif
	}

	/* n == 0 must not touch the buffer; return is the base pointer. */
	{
		char buf[8];
		memset(buf, 0x5a, sizeof buf);
		char *r = jstr_stpset_len(buf, 'Q', 0);
		assert(r == buf);
		for (size_t i = 0; i < sizeof buf; ++i)
			assert((unsigned char)buf[i] == 0x5a);
	}

	/* Only [0, n) is written. */
	{
		char buf[8];
		memset(buf, 0x5a, sizeof buf);
		char *r = jstr_stpset_len(buf, 'Q', 3);
		assert(r == buf + 3);
		assert(memcmp(buf, "QQQ", 3) == 0);
		for (size_t i = 3; i < sizeof buf; ++i)
			assert((unsigned char)buf[i] == 0x5a);
	}

	return 0;
}
