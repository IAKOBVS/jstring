/* SPDX-License-Identifier: MIT */

/* Adversarial short-needle benchmark: haystack is ALL first-byte hits
 * ('n' x 1MB), needle 'n'*(L-1)+'m' placed at the end. Every alignment
 * passes a first-byte prefilter, so engines must do real compares nearly
 * everywhere. Isolates the scanner/compare machinery from the prefilter. */

#include "bench.h"

#include "../include/string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFLEN (1 << 20)
#define ITERS 20

char *hs;
size_t hs_len;
char needle[64];
size_t needle_len;



static void *
scalar_memmem(const void *h, size_t hl, const void *n, size_t nl)
{
	if (!nl)
		return (void *)h;
	if (nl > hl)
		return NULL;
	const unsigned char *hb = (const unsigned char *)h;
	const unsigned char *nb = (const unsigned char *)n;
	for (const unsigned char *p = hb, *end = hb + hl - nl; p <= end; ++p)
		if (*p == *nb && memcmp(p, nb, nl) == 0)
			return (void *)p;
	return NULL;
}

#define MAKE(fn_name, impl)                                               \
	static size_t fn_name(void *params)                               \
	{                                                                 \
		size_t cs = 0;                                            \
		needle_len = (size_t)params;                              \
		memset(needle, 'n', needle_len - 1);                      \
		needle[needle_len - 1] = 'm';                             \
		memcpy(hs + hs_len - needle_len, needle, needle_len);     \
		for (size_t i = 0; i < ITERS; ++i)                        \
			cs += impl(hs, hs_len, needle, needle_len) != NULL; \
		hs[hs_len - needle_len] = 'n';                            \
		return cs;                                                \
	}

MAKE(b_jstr_memmem, jstr_memmem)
MAKE(b_scalar_memmem, scalar_memmem)

#if defined(__GLIBC__)
static size_t
b_libc_memmem(void *params)
{
	size_t cs = 0;
	needle_len = (size_t)params;
	memset(needle, 'n', needle_len - 1);
	needle[needle_len - 1] = 'm';
	memcpy(hs + hs_len - needle_len, needle, needle_len);
	for (size_t i = 0; i < ITERS; ++i)
		cs += memmem(hs, hs_len, needle, needle_len) != NULL;
	hs[hs_len - needle_len] = 'n';
	return cs;
}
#define NEED_LIBC 1
#else
#define NEED_LIBC 0
#endif

int main(void)
{
	hs = (char *)malloc(BUFLEN + 1);
	if (!hs)
		return 1;
	memset(hs, 'n', BUFLEN);
	hs_len = BUFLEN;

	RUN(b_jstr_memmem, 1);
	RUN(b_jstr_memmem, 2);
	RUN(b_jstr_memmem, 3);
	RUN(b_jstr_memmem, 4);
	RUN(b_jstr_memmem, 5);
	RUN(b_jstr_memmem, 6);
	RUN(b_jstr_memmem, 7);
	RUN(b_jstr_memmem, 8);
	RUN(b_jstr_memmem, 9);
	RUN(b_jstr_memmem, 10);
	RUN(b_jstr_memmem, 11);
	RUN(b_jstr_memmem, 12);
	RUN(b_jstr_memmem, 13);
	RUN(b_jstr_memmem, 14);
	RUN(b_jstr_memmem, 15);
	RUN(b_jstr_memmem, 16);
	RUN(b_scalar_memmem, 1);
	RUN(b_scalar_memmem, 2);
	RUN(b_scalar_memmem, 3);
	RUN(b_scalar_memmem, 4);
	RUN(b_scalar_memmem, 5);
	RUN(b_scalar_memmem, 6);
	RUN(b_scalar_memmem, 7);
	RUN(b_scalar_memmem, 8);
	RUN(b_scalar_memmem, 9);
	RUN(b_scalar_memmem, 10);
	RUN(b_scalar_memmem, 11);
	RUN(b_scalar_memmem, 12);
	RUN(b_scalar_memmem, 13);
	RUN(b_scalar_memmem, 14);
	RUN(b_scalar_memmem, 15);
	RUN(b_scalar_memmem, 16);
#if NEED_LIBC
	RUN(b_libc_memmem, 1);
	RUN(b_libc_memmem, 2);
	RUN(b_libc_memmem, 3);
	RUN(b_libc_memmem, 4);
	RUN(b_libc_memmem, 5);
	RUN(b_libc_memmem, 6);
	RUN(b_libc_memmem, 7);
	RUN(b_libc_memmem, 8);
	RUN(b_libc_memmem, 9);
	RUN(b_libc_memmem, 10);
	RUN(b_libc_memmem, 11);
	RUN(b_libc_memmem, 12);
	RUN(b_libc_memmem, 13);
	RUN(b_libc_memmem, 14);
	RUN(b_libc_memmem, 15);
	RUN(b_libc_memmem, 16);
#endif

	free(hs);
	return 0;
}
