/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Correctness tests for the two-pass single-allocation rewrite of
 * jstr_insertafterallchr_len / jstr_insertafterall_len. Every case is
 * checked against a naive per-insertion reference. */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

static void
naive_insertafterallchr(char *s, size_t *sz, int c, const char *src, size_t src_len)
{
	char tmp[4096];
	assert(*sz + 1 <= sizeof(tmp));
	memcpy(tmp, s, *sz + 1);
	size_t nsz = *sz;
	size_t off = 0;
	const char *p;
	while ((p = (char *)memchr(tmp + off, c, nsz - off))) {
		off = (size_t)(p - tmp) + 1; /* insert AFTER the match */
		memmove(tmp + off + src_len, tmp + off, nsz - off);
		memcpy(tmp + off, src, src_len);
		nsz += src_len;
		off += src_len;
	}
	memcpy(s, tmp, nsz + 1);
	*sz = nsz;
}

static void
check_case(const char *input, int c, const char *src)
{
	const size_t src_len = strlen(src);
	char *a = NULL, *b = NULL;
	size_t sza = 0, capa = 0, szb = 0, capb = 0;
	const size_t in_len = strlen(input);

	assert(!jstr_chk(jstr_assign_len(&a, &sza, &capa, input, in_len)));
	assert(!jstr_chk(jstr_assign_len(&b, &szb, &capb, input, in_len)));

	assert(jstr_insertafterallchr_len(&a, &sza, &capa, c, src, src_len) == JSTR_RET_SUCC);
	naive_insertafterallchr(b, &szb, c, src, src_len);

	assert(sza == szb);
	assert(memcmp(a, b, sza + 1) == 0);

	free(a);
	free(b);
}

int
main(void)
{
	static const char *const inputs[] = {"", "a", "aa", "abc", "aaa", "ababab", "xyz", "aabbaabb", "a"};
	static const char *const srcs[] = {"", "x", "yy", "zzz"};

	for (size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); ++i) {
		for (size_t k = 0; k < sizeof(srcs) / sizeof(srcs[0]); ++k) {
			check_case(inputs[i], 'a', srcs[k]);
			check_case(inputs[i], 'z', srcs[k]);
		}
	}

	/* Match at end of string. */
	check_case("cba", 'a', "<>");
	/* Multi-byte haystack sanity. */
	check_case("banana", 'n', "-");

	/* Large input: many matches, forces reallocation during reserve. */
	{
		char *a = NULL;
		size_t sza = 0, capa = 0;
		const size_t n = 10000;
		assert(!jstr_chk(jstr_reserve(&a, &sza, &capa, n + 1)));
		memset(a, 'a', n);
		sza = n;
		a[n] = '\0';
		assert(jstr_insertafterallchr_len(&a, &sza, &capa, 'a', "bc", 2) == JSTR_RET_SUCC);
		assert(sza == n + n * 2);
		for (size_t i = 0; i < n; ++i)
			assert(a[i * 3] == 'a' && a[i * 3 + 1] == 'b' && a[i * 3 + 2] == 'c');
		assert(a[sza] == '\0');
		jstr_free(&a, &sza, &capa);
	}

	/* String-needle variant delegating to the chr path for len-1 needles. */
	{
		char *a = NULL;
		size_t sza = 0, capa = 0;
		assert(!jstr_chk(jstr_assign_len(&a, &sza, &capa, "abab", 4)));
		assert(jstr_insertafterall_len(&a, &sza, &capa, "b", 1, "-", 1) == JSTR_RET_SUCC);
		assert(sza == 6);
		assert(memcmp(a, "ab-ab-", 7) == 0);
		jstr_free(&a, &sza, &capa);
	}

	return EXIT_SUCCESS;
}
