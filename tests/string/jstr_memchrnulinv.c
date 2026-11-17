/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static void *
ref_memchrnulinv(const void *s, int c, size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	size_t i = 0;
	for (; i < n && p[i] == (unsigned char)c; ++i) {}
	return (void *)(p + i);
}

int
main(void)
{
	unsigned char buf[1100];

	/* Sentinel is s + n (never NULL), n == 0 returns s. */
	memcpy(buf, "aaab", 4);
	RCHECK(jstr_memchrnulinv(buf, 'a', 4) == buf + 3);
	RCHECK(jstr_memchrnulinv(buf, 'a', 3) == buf + 3);
	RCHECK(jstr_memchrnulinv(buf, 'b', 4) == buf);
	RCHECK(jstr_memchrnulinv(buf, 'a', 0) == buf);
	RCHECK(jstr_memchrnulinv(buf, 0, 4) == buf);
	RCHECK(jstr_memchrnulinv(buf, 0, 0) == buf);

	for (size_t len = 0; len <= 40; ++len) {
		for (int c = 0; c < 256; ++c) {
			memset(buf, (unsigned char)c, len);
			const size_t pos = len ? (size_t)c % len : 0;
			if (pos < len)
				buf[pos] = (unsigned char)(c ^ 0x5a);
			void *got = jstr_memchrnulinv(buf, c, len);
			void *want = ref_memchrnulinv(buf, c, len);
			RCHECK(got == want);
			RCHECK(got == buf + (pos < len ? pos : len));
		}
	}

	rigor_srand(0xdeadbeefu);
	for (size_t i = 0; i < RIGOR_NLENS; ++i) {
		const size_t len = rigor_lens[i];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill(buf, len, 1);
		for (int c = 0; c < 256; ++c)
			RCHECK(jstr_memchrnulinv(buf, c, len) == ref_memchrnulinv(buf, c, len));
	}

	return 0;
}
