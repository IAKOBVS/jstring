/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static const void *
ref_memrchrnul(const void *s, int c, size_t n)
{
	const unsigned char *p = (const unsigned char *)s + n;
	const unsigned char *const end = p;
	for (; n--;)
		if (*--p == (unsigned char)c)
			return (const void *)p;
	return (const void *)end;
}

int
main(void)
{
	unsigned char buf[1100];

	/* No-match sentinel is s + n, even with a matching byte just past n. */
	memcpy(buf, "abcXqrst", 9);
	RCHECK(jstr_memrchrnul(buf, 'X', 3) == buf + 3);
	RCHECK(jstr_memrchrnul(buf, 'q', 4) == buf + 4);
	RCHECK(jstr_memrchrnul(buf, 'X', 0) == buf);
	RCHECK(jstr_memrchrnul(buf, 'a', 1) == buf);
	RCHECK(jstr_memrchrnul(buf, 'c', 3) == buf + 2);
	memcpy(buf, "abcXabcY", 8);
	RCHECK(jstr_memrchrnul(buf, 'a', 8) == buf + 4);
	RCHECK(jstr_memrchrnul(buf, 'c', 8) == buf + 6);
	RCHECK(jstr_memrchrnul(buf, 0, 8) == buf + 8);

	/* Every position, including embedded NUL, for every byte value. */
	for (size_t len = 0; len <= 40; ++len) {
		for (int c = 0; c < 256; ++c) {
			memset(buf, (unsigned char)(c ^ 0x5a), len + 3);
			const size_t pos = len ? (size_t)c % (len + 1) : 0;
			if (pos < len)
				buf[pos] = (unsigned char)c;
			const void *got = jstr_memrchrnul(buf, c, len);
			RCHECK(got == buf + (pos < len ? pos : len));
			RCHECK(got == ref_memrchrnul(buf, c, len));
		}
	}

	/* Differential sweep over boundary lengths with binary payloads. */
	rigor_srand(0x5678u);
	for (size_t i = 0; i < RIGOR_NLENS; ++i) {
		const size_t len = rigor_lens[i];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill(buf, len, 1);
		for (int c = 0; c < 256; ++c)
			RCHECK(jstr_memrchrnul(buf, c, len) == ref_memrchrnul(buf, c, len));
		memset(buf, 0x5a, len);
		buf[len] = 0x5b;
		buf[len + 1] = 0x5c;
		RCHECK(jstr_memrchrnul(buf, 0x5b, len) == buf + len);
		RCHECK(jstr_memrchrnul(buf, 0x5c, len + 1) == buf + len + 1);
	}

	return 0;
}
