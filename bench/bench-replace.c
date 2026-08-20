/* SPDX-License-Identifier: MIT */

#include "bench.h"

#include "../include/replace.h"
#include "../include/string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFLEN 500000

char *buf;
size_t buf_sz;
size_t buf_cap;

static void
simple_rplcall_naive(char *s, size_t *sz, const char *find, size_t find_len, const char *rplc, size_t rplc_len)
{
	const char *p;
	size_t off = 0;
	while ((p = jstr_memmem(s + off, *sz - off, find, find_len)) != NULL) {
		size_t at = (size_t)(p - s);
		size_t tail = *sz - (at + find_len);
		if (rplc_len != find_len)
			memmove(s + at + rplc_len, s + at + find_len, tail);
		memcpy(s + at, rplc, rplc_len);
		*sz += rplc_len - find_len;
		off = at + rplc_len;
	}
	s[*sz] = '\0';
}

static size_t
b_jstr_rplcall_len(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < 10; i++) {
		buf_sz = BUFLEN;
		jstr_rplcall_len(&buf, &buf_sz, &buf_cap, "a", 1, "bb", 2);
		cs += buf_sz;
		buf[0] ^= 1;
	}
	return cs;
}

static size_t
b_jstr_rplcall_len_samelen(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < 10; i++) {
		buf_sz = BUFLEN;
		jstr_rplcall_len(&buf, &buf_sz, &buf_cap, "ab", 2, "xy", 2);
		cs += buf_sz;
		buf[0] ^= 1;
	}
	return cs;
}

static size_t
b_simple_rplcall_naive(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < 10; i++) {
		buf_sz = BUFLEN;
		simple_rplcall_naive(buf, &buf_sz, "a", 1, "bb", 2);
		cs += buf_sz;
		buf[0] ^= 1;
	}
	return cs;
}

static size_t
b_jstr_insertafterallchr_len(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < 10; i++) {
		buf_sz = BUFLEN;
		jstr_insertafterallchr_len(&buf, &buf_sz, &buf_cap, 'a', "xx", 2);
		cs += buf_sz;
		buf[0] ^= 1;
	}
	return cs;
}

static size_t
b_jstr_rplc_len(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < 10; i++) {
		buf_sz = BUFLEN;
		jstr_rplc_len(&buf, &buf_sz, &buf_cap, "abc", 3, "xyz", 3);
		cs += buf_sz;
		buf[0] ^= 1;
	}
	return cs;
}

static size_t
b_jstr_rplcchr_len(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < 50; i++) {
		buf_sz = BUFLEN;
		jstr_rplcchr_len(buf, buf_sz, 'a', 'z');
		cs += buf_sz;
		buf[0] ^= 1;
	}
	return cs;
}

static size_t
b_jstr_rplcallchr(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < 50; i++) {
		buf_sz = BUFLEN;
		jstr_rplcallchr(buf, 'a', 'z');
		cs += buf_sz;
		buf[0] ^= 1;
	}
	return cs;
}

static size_t
b_jstr_count(void *dummy)
{
	size_t cs = 0;
	for (size_t i = 0; i < 10; i++) {
		cs += jstr_count(buf, "abc");
		buf[0] ^= 1;
	}
	return cs;
}

int
main()
{
	buf_cap = BUFLEN + 64;
	buf = malloc(buf_cap);
	assert(buf);

	for (size_t i = 0; i < BUFLEN; i++)
		buf[i] = 'a' + (char)(i % 26);
	buf[BUFLEN] = '\0';

	RUN(b_jstr_rplcall_len, 0);
	RUN(b_jstr_rplcall_len_samelen, 0);
	RUN(b_simple_rplcall_naive, 0);
	RUN(b_jstr_insertafterallchr_len, 0);
	RUN(b_jstr_rplc_len, 0);
	RUN(b_jstr_rplcchr_len, 0);
	RUN(b_jstr_rplcallchr, 0);
	RUN(b_jstr_count, 0);

	free(buf);
	return 0;
}
