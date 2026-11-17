/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <stddef.h>
#include <string.h>

#include "../rigor.h"

#ifndef JSTR_TEST_SLOW
#	define JSTR_TEST_SLOW 0
#endif

static size_t
ref_count_len(const void *s, size_t sz, const void *find, size_t fl)
{
	const unsigned char *h = (const unsigned char *)s;
	const unsigned char *n = (const unsigned char *)find;
	if (fl == 0)
		return 0;
	size_t cnt = 0, i = 0;
	while (i + fl <= sz) {
		if (memcmp(h + i, n, fl) == 0) {
			++cnt;
			i += fl;
		} else {
			++i;
		}
	}
	return cnt;
}

int
main(void)
{
	unsigned char h[1100], n[1100];
	jstr_twoway_ty t;

	jstr_memmem_comp(&t, "x", 1);
	RCHECK(jstr_count_len_exec(&t, "hello", 5, "", 0) == 0);
	jstr_memmem_comp(&t, "l", 1);
	RCHECK(jstr_count_len_exec(&t, "hello", 5, "l", 1) == 2);
	jstr_memmem_comp(&t, "aa", 2);
	RCHECK(jstr_count_len_exec(&t, "aaaa", 4, "aa", 2) == 2);
	RCHECK(jstr_count_len_exec(&t, "aaaaa", 5, "aa", 2) == 2);
	RCHECK(jstr_count_len_exec(&t, "aaa", 3, "aa", 2) == 1);
	RCHECK(jstr_count_len_exec(&t, "abcabcabc", 9, "abc", 3) == 3);
	RCHECK(jstr_count_len_exec(&t, "abcabcabc", 9, "abcd", 4) == 0);
	{
		const char bin[8] = { 'a', 0, 'b', 'a', 0, 'b', 'c', 0 };
		jstr_memmem_comp(&t, "a\0b", 3);
		RCHECK(jstr_count_len_exec(&t, bin, 8, "a\0b", 3) == 2);
		jstr_memmem_comp(&t, "\0b", 2);
		RCHECK(jstr_count_len_exec(&t, bin, 8, "\0b", 2) == 2);
	}

	rigor_srand(0x5566aabbu);
	for (size_t li = 0; li < RIGOR_NLENS; ++li) {
		const size_t hl = rigor_lens[li];
		if (!JSTR_TEST_SLOW && hl > 128)
			continue;
		rigor_fill(h, hl, 1);
		for (size_t ni = 0; ni < RIGOR_NLENS; ++ni) {
			const size_t nl = rigor_lens[ni];
			if (!JSTR_TEST_SLOW && nl > 128)
				continue;
			rigor_fill(n, nl, 1);
			if (nl && nl <= hl)
				memcpy(n, h + (hl - nl) / 2, nl);
			jstr_memmem_comp(&t, n, nl);
			RCHECK(jstr_count_len_exec(&t, (const char *)h, hl, (const char *)n, nl) ==
			       ref_count_len(h, hl, n, nl));
		}
	}

	return 0;
}
