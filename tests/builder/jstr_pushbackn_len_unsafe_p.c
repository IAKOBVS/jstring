/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

	char buf[64];
	strcpy(buf, "hi");
	size_t sz = 2;

	char *p = jstr_pushbackn_len_unsafe_p(buf, sz, 'z', 3);
	/* fills 3 bytes at end, no NUL; returns buf+sz+3 */
	assert(memcmp(buf + sz, "zzz", 3) == 0);
	assert(p == buf + sz + 3);

	/* n == 0 returns same pointer */
	char *p2 = jstr_pushbackn_len_unsafe_p(buf, sz, 'z', 0);
	assert(p2 == buf + sz);

	SUCCESS();
	return EXIT_SUCCESS;
}
