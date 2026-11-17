/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

int
main(void)
{
	char buf[1100];

	RCHECK(jstr_strnlen("hello world", 5) == 5);
	RCHECK(jstr_strnlen("hello world", 11) == 11);
	RCHECK(jstr_strnlen("hello world", 50) == 11);
	RCHECK(jstr_strnlen("hello world", 0) == 0);
	RCHECK(jstr_strnlen("", 0) == 0);
	RCHECK(jstr_strnlen("", 1) == 0);
	RCHECK(jstr_strnlen("a", 0) == 0);
	RCHECK(jstr_strnlen("a", 1) == 1);
	RCHECK(jstr_strnlen("a", 2) == 1);

	rigor_srand(0x12345678u);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t len = rigor_lens[li];
		if (!JSTR_TEST_SLOW && len > 128)
			continue;
		rigor_fill_printable(buf, len);
		const size_t want_full = strlen(buf);
		for (size_t n = 0; n <= len + 2; ++n) {
			const size_t want = want_full < n ? want_full : n;
			RCHECK(jstr_strnlen(buf, n) == want);
		}
		/* Insert an interior NUL and verify the cap is still honoured. */
		if (len) {
			const size_t z = len / 2;
			buf[z] = 0;
			for (size_t n = 0; n <= len + 2; ++n) {
				const size_t want = z < n ? z : n;
				RCHECK(jstr_strnlen(buf, n) == want);
			}
		}
	}

	return 0;
}
