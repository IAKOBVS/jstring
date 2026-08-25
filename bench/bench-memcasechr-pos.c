/* SPDX-License-Identifier: MIT */

/* memcasechr speed vs MATCH POSITION (and needle class).
 * Separates raw per-byte scan rate from end-of-buffer effects:
 *   pos%   : first occurrence placed at frac*len (letter needle)
 *   absent : full scan
 * Also checks the degenerate paths: non-letter needle (plain memchr) and
 * a >=0x80 byte (tolower==toupper -> plain memchr). */

#include "bench.h"

#include "../include/ctype.h"
#include "../include/string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN ((size_t)32 << 20)

static char *hs;
static size_t hs_len;
static int cur_iters;
static int cur_c;

static void *
memchr_twice(const void *s, int c, size_t n)
{
	const unsigned char lo = (unsigned char)jstr_tolower(c);
	const unsigned char up = (unsigned char)jstr_toupper(c);
	if (lo == up)
		return (void *)memchr(s, c, n);
	const void *r1 = memchr(s, lo, n);
	const void *r2 = memchr(s, up, n);
	if (r1 == NULL)
		return (void *)r2;
	return r2 == NULL || (const char *)r1 < (const char *)r2 ? (char *)r1 : (char *)r2;
}

static void *(*volatile fp_op)(const void *, int, size_t);

#define MAKE(fn_name, impl)                                     \
	static size_t fn_name(void *params)                     \
	{                                                       \
		size_t cs = 0;                                  \
		fp_op = (void *(*)(const void *, int, size_t))impl; \
		for (int i = 0; i < cur_iters; ++i) {           \
			const int c = cur_c ^ (i & 1 ? 0 : 'a' - 'A'); \
			cs += fp_op(hs, c, hs_len) != NULL;     \
		}                                               \
		return cs;                                      \
	}

MAKE(b_jstr_memcasechr, jstr_memcasechr)
MAKE(b_memchr_twice, memchr_twice)

int
main(void)
{
	hs = malloc(MAXLEN + 1);
	if (!hs)
		return 1;

	const size_t sizes[2] = {(size_t)1 << 20, (size_t)32 << 20};
	const int iters[2] = {20, 3};
	const float fracs[] = {0.0f, 0.01f, 0.25f, 0.5f, 0.9f, 0.9999f};
	const size_t nfr = sizeof(fracs) / sizeof(fracs[0]);

	for (int si = 0; si < 2; ++si) {
		hs_len = sizes[si];
		cur_iters = iters[si];
		cur_c = 'a';
		memset(hs, 'x', hs_len);

		for (size_t f = 0; f < nfr; ++f) {
			const size_t pos = (size_t)((double)fracs[f] * (double)hs_len);
			hs[pos] = 'a';
			RUN(b_jstr_memcasechr, f);
			RUN(b_memchr_twice, f);
			hs[pos] = 'x';
		}
		cur_c = 'a';
		RUN(b_jstr_memcasechr, 99); /* absent */
		RUN(b_memchr_twice, 99);

		/* non-letter control: '-' never present */
		cur_c = '-';
		RUN(b_jstr_memcasechr, 98);
		RUN(b_memchr_twice, 98);

		/* >=0x80 control: tolower(0xE9)==toupper(0xE9) -> plain memchr */
		memset(hs, (char)0xEE, hs_len);
		cur_c = (char)0xE9;
		RUN(b_jstr_memcasechr, 97);
		RUN(b_memchr_twice, 97);
		memset(hs, 'x', hs_len);
	}

	free(hs);
	return 0;
}
