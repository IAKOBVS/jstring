/* SPDX-License-Identifier: MIT */

#include "bench.h"

#include "../include/string.h"
#include "../include/replace.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFLEN 500000
#define ITERS 200

char *buf;

static size_t
b_jstr_rmspn_from(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < ITERS; i++) {
		size_t sz = BUFLEN;
		cs += jstr_rmspn_from(buf, &sz, 0, "-");
		memcpy(buf + sz, buf, BUFLEN - sz); /* restore removed tail */
		buf[BUFLEN] = '\0';
		buf[0] ^= 1;
	}
	return cs;
}

static size_t
b_jstr_trimstart(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < ITERS; i++) {
		jstr_trimstart(buf);
		cs += buf[0];
		memmove(buf + 4, buf, BUFLEN - 4); /* re-insert leading ws */
		memcpy(buf, "    ", 4);
	}
	return cs;
}

int main(void)
{
	buf = (char *)malloc(BUFLEN + 1);
	if (!buf)
		return 1;

	memset(buf, '-', 64);
	memset(buf + 64, 'a', BUFLEN - 64 - 1);
	buf[BUFLEN - 1] = '\0';
	RUN(b_jstr_rmspn_from, 0);

	memcpy(buf, "    ", 4);
	memset(buf + 4, 'a', BUFLEN - 5);
	buf[BUFLEN - 1] = '\0';
	RUN(b_jstr_trimstart, 0);

	free(buf);
	return 0;
}
