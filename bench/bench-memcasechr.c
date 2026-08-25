/* SPDX-License-Identifier: MIT */

/* Case-insensitive single-char search shootout across WORKING SET SIZES:
 *   jstr_memcasechr - linked .so decides: SIMD (normal build) or vendored
 *                     musl (build-musl)
 *   memchr twice    - glibc memchr(lower) + memchr(upper), take first hit
 *   naive           - byte loop with per-byte tolower
 *
 * Sizes span cache-resident (1MB) to well past typical L3 (64MB) to show
 * where the two-pass idiom's extra DRAM traffic starts to cost. */

#include "bench.h"

#include "../include/ctype.h"
#include "../include/string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN ((size_t)64 << 20)
#define N_SIZES 3

static char *hs;
static size_t hs_len;
static int cur_iters;

static void *
memchr_twice(const void *s, int c, size_t n)
{
	const unsigned char lo = (unsigned char)jstr_tolower(c);
	const unsigned char up = (unsigned char)jstr_toupper(c);
	if (lo == up)
		return (void *)memchr(s, c, n);
	const unsigned char *p = (const unsigned char *)s;
	const void *r1 = memchr(p, lo, n);
	const void *r2 = memchr(p, up, n);
	if (r1 == NULL)
		return (void *)r2;
	return (char *)r1;
}

static void *
naive_casechr(const void *s, int c, size_t n)
{
	const unsigned char lo = (unsigned char)jstr_tolower(c);
	const unsigned char up = (unsigned char)jstr_toupper(c);
	const unsigned char *p = (const unsigned char *)s;
	for (; n--; ++p)
		if (*p == lo || *p == up)
			return (void *)p;
	return NULL;
}

enum { HIT_UPPER = 0, NO_HIT };

static void *(*volatile fp_op)(const void *, int, size_t);

#define MAKE(fn_name, impl)                                     \
	static size_t fn_name(void *params)                     \
	{                                                       \
		size_t cs = 0;                                  \
		fp_op = (void *(*)(const void *, int, size_t))impl; \
		for (int i = 0; i < cur_iters; ++i) {           \
			const int c = (i & 1) ? 'A' : 'a';      \
			cs += fp_op(hs, c, hs_len) != NULL;     \
		}                                               \
		return cs;                                      \
	}

MAKE(b_jstr_memcasechr, jstr_memcasechr)
MAKE(b_memchr_twice, memchr_twice)
MAKE(b_naive, naive_casechr)

int
main(void)
{
	hs = malloc(MAXLEN + 1);
	if (!hs)
		return 1;

	const size_t sizes[N_SIZES] = {
		(size_t)1 << 20,
		(size_t)16 << 20,
		(size_t)64 << 20,
	};
	const int iters[N_SIZES] = {20, 4, 2};

	for (int si = 0; si < N_SIZES; ++si) {
		hs_len = sizes[si];
		cur_iters = iters[si];
		memset(hs, 'x', hs_len);

		RUN(b_jstr_memcasechr, NO_HIT);
		RUN(b_memchr_twice, NO_HIT);
		RUN(b_naive, NO_HIT);

		hs[hs_len - 1] = 'A';
		RUN(b_jstr_memcasechr, HIT_UPPER);
		RUN(b_memchr_twice, HIT_UPPER);
		RUN(b_naive, HIT_UPPER);
		hs[hs_len - 1] = 'x';
	}

	free(hs);
	return 0;
}
