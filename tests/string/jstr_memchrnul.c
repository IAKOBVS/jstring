/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static const void *
ref_memchrnul(const void *s, int c, size_t n)
{
	const unsigned char *p = (const unsigned char *)memchr(s, c, n);
	return p ? (const void *)p : (const void *)((const unsigned char *)s + n);
}

int
main(void)
{
	unsigned char buf[1100];

	/* No-match sentinel is s + n and must not inspect bytes past n. */
	memcpy(buf, "abcXqrst", 9);
	RCHECK(jstr_memchrnul(buf, 'X', 3) == buf + 3);
	RCHECK(jstr_memchrnul(buf, 'q', 4) == buf + 4);
	RCHECK(jstr_memchrnul(buf, 'X', 0) == buf);
	RCHECK(jstr_memchrnul(buf, 'a', 1) == buf);
	RCHECK(jstr_memchrnul(buf, 'c', 3) == buf + 2);

	/* Every position, including an embedded NUL, and every byte value. */
	for (size_t len = 0; len <= 40; ++len) {
		for (int c = 0; c < 256; ++c) {
			memset(buf, (unsigned char)(c ^ 0x5a), len + 3);
			buf[len] = 0;
			const size_t pos = len ? (size_t)c % (len + 1) : 0;
			if (pos < len)
				buf[pos] = (unsigned char)c;
			const void *got = jstr_memchrnul(buf, c, len);
			const void *want = ref_memchrnul(buf, c, len);
			RCHECK(got == want);
			RCHECK(got == buf + (pos < len ? pos : len));
		}
	}

	/* Differential sweep over boundary lengths with binary payloads. */
	rigor_srand(0x1234u);
	for (size_t i = 0; i < RIGOR_NLENS; ++i) {
		const size_t len = rigor_lens[i];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill(buf, len, 1);
		for (int c = 0; c < 256; ++c)
			RCHECK(jstr_memchrnul(buf, c, len) == ref_memchrnul(buf, c, len));
		/* n + 1 / n + 2 must not look past the requested length. */
		memset(buf, 0x5a, len);
		buf[len] = 0x5b;
		buf[len + 1] = 0x5c;
		RCHECK(jstr_memchrnul(buf, 0x5b, len) == buf + len);
		RCHECK(jstr_memchrnul(buf, 0x5c, len + 1) == buf + len + 1);
	}

	return 0;
}
