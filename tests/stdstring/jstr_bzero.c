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

	for (size_t i = 0; i < SZ; ++i)
		buf[i] = ref[i] = (unsigned char)(0xa5u ^ (unsigned)i);

	jstr_bzero(buf, n);
	memset(ref, 0, n);

	assert(memcmp(buf, ref, SZ) == 0);
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

	/* n == 0 must not write a single byte. */
	{
		unsigned char buf[16];
		for (size_t i = 0; i < sizeof buf; ++i)
			buf[i] = 0xff;
		jstr_bzero(buf, 0);
		for (size_t i = 0; i < sizeof buf; ++i)
			assert(buf[i] == 0xff);
	}

	/* all-0xff and all-0x00 payloads, whole-buffer and partial. */
	{
		unsigned char buf[64];
		memset(buf, 0xff, sizeof buf);
		jstr_bzero(buf, sizeof buf);
		for (size_t i = 0; i < sizeof buf; ++i)
			assert(buf[i] == 0);
		memset(buf, 0x00, sizeof buf);
		jstr_bzero(buf, sizeof buf);
		for (size_t i = 0; i < sizeof buf; ++i)
			assert(buf[i] == 0);
		memset(buf, 0xff, sizeof buf);
		jstr_bzero(buf + 8, 16);
		for (size_t i = 0; i < sizeof buf; ++i)
			assert(buf[i] == (i >= 8 && i < 24 ? 0 : 0xff));
	}

	return 0;
}
