/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static void *
ref_memchrinv(const void *s, int c, size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	for (size_t i = 0; i < n; ++i)
		if (p[i] != (unsigned char)c)
			return (void *)(p + i);
	return NULL;
}

int
main(void)
{
	unsigned char buf[1100];

	/* NULL exactly when every one of the n bytes equals c. */
	memcpy(buf, "aaab", 4);
	RCHECK(jstr_memchrinv(buf, 'a', 4) == buf + 3);
	RCHECK(jstr_memchrinv(buf, 'a', 3) == NULL);
	RCHECK(jstr_memchrinv(buf, 'b', 4) == buf);
	RCHECK(jstr_memchrinv(buf, 'a', 0) == NULL);
	RCHECK(jstr_memchrinv(buf, 0, 4) == buf);

	/* Only position pos differs, so the first non-c must be exactly pos. */
	for (size_t len = 0; len <= 40; ++len) {
		for (int c = 0; c < 256; ++c) {
			memset(buf, (unsigned char)c, len);
			const size_t pos = len ? (size_t)c % len : 0;
			if (pos < len)
				buf[pos] = (unsigned char)(c ^ 0x5a);
			void *got = jstr_memchrinv(buf, c, len);
			void *want = ref_memchrinv(buf, c, len);
			RCHECK(got == want);
			RCHECK(got == (pos < len ? (void *)(buf + pos) : NULL));
		}
	}

	/* Differential sweep over boundary lengths with binary payloads. */
	rigor_srand(0x9abcu);
	for (size_t i = 0; i < RIGOR_NLENS; ++i) {
		const size_t len = rigor_lens[i];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill(buf, len, 1);
		for (int c = 0; c < 256; ++c)
			RCHECK(jstr_memchrinv(buf, c, len) == ref_memchrinv(buf, c, len));
	}

	return 0;
}
