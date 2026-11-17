/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* jstr_memrchrinv() returns the last byte not equal to c, or NULL when every
 * byte matches (equivalently a reverse memrchr).  n == 0 returns NULL without
 * forming an out-of-range pointer. */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static void *
ref_memrchrinv(const void *s, int c, size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	for (size_t i = n; i > 0; --i)
		if (p[i - 1] == (unsigned char)c)
			return (void *)(p + i - 1);
	return NULL;
}

int
main(void)
{
	unsigned char buf[1100];

	memcpy(buf, "aaab", 4);
	RCHECK(jstr_memrchrinv(buf, 'b', 4) == buf + 3);
	RCHECK(jstr_memrchrinv(buf, 'a', 4) == buf + 2);
	RCHECK(jstr_memrchrinv(buf, 'a', 2) == buf + 1);
	RCHECK(jstr_memrchrinv(buf, 'a', 0) == NULL);
	RCHECK(jstr_memrchrinv(buf, 'z', 4) == NULL);
	RCHECK(jstr_memrchrinv(buf, 0, 4) == NULL);

	/* Only position pos equals c; the reverse search must find exactly pos. */
	for (size_t len = 0; len <= 40; ++len) {
		for (int c = 0; c < 256; ++c) {
			memset(buf, (unsigned char)(c ^ 0x5a), len);
			const size_t pos = len ? (size_t)c % len : 0;
			if (pos < len)
				buf[pos] = (unsigned char)c;
			void *got = jstr_memrchrinv(buf, c, len);
			void *want = ref_memrchrinv(buf, c, len);
			RCHECK(got == want);
			RCHECK(got == (pos < len ? (void *)(buf + pos) : NULL));
		}
	}

	rigor_srand(0x13579bdfu);
	for (size_t i = 0; i < RIGOR_NLENS; ++i) {
		const size_t len = rigor_lens[i];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill(buf, len, 1);
		for (int c = 0; c < 256; ++c)
			RCHECK(jstr_memrchrinv(buf, c, len) == ref_memrchrinv(buf, c, len));
	}

	return 0;
}
