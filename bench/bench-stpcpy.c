/* SPDX-License-Identifier: MIT */

#include "bench.h"

#include "../include/string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFLEN 500000
#define ITERS 200

char *buf;
char *dst;
size_t dst_cap;

static void
simple_stpcpy(char *d, const char *s)
{
	while ((*d++ = *s++)) {}
}

static size_t
b_jstr_stpcpy(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < ITERS; i++) {
		char *e = jstr_stpcpy(dst, buf);
		cs += (size_t)(e - dst);
		buf[0] ^= 1;
	}
	return cs;
}

static size_t
b_simple_stpcpy(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < ITERS; i++) {
		simple_stpcpy(dst, buf);
		cs += BUFLEN;
		buf[0] ^= 1;
	}
	return cs;
}

static size_t
b_libc_stpcpy(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < ITERS; i++) {
		char *e = stpcpy(dst, buf);
		cs += (size_t)(e - dst);
		buf[0] ^= 1;
	}
	return cs;
}

int main(void)
{
	buf = (char *)malloc(BUFLEN + 1);
	dst = (char *)malloc(BUFLEN + 1);
	if (!buf || !dst)
		return 1;

	/* Long string: worst case for scalar, best case for SIMD. */
	memset(buf, 'a', BUFLEN - 1);
	buf[BUFLEN - 1] = '\0';
	RUN(b_jstr_stpcpy, 1);
	RUN(b_libc_stpcpy, 1);
	RUN(b_simple_stpcpy, 1);

	/* Short string: overhead-dominated. */
	memset(buf, 'a', 31);
	buf[31] = '\0';
	RUN(b_jstr_stpcpy, 0);
	RUN(b_libc_stpcpy, 0);
	RUN(b_simple_stpcpy, 0);

	free(buf);
	free(dst);
	return 0;
}
