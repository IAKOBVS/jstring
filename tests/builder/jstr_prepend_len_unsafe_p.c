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

	char *p = jstr_prepend_len_unsafe_p(buf, sz, "XY", 2);
	assert(memcmp(buf, "XYhi", 5) == 0);
	assert(p == buf + sz + 2);

	/* sz == 0: writes NUL at buf[src_len] */
	char buf2[64];
	char *p2 = jstr_prepend_len_unsafe_p(buf2, 0, "XY", 2);
	assert(buf2[2] == '\0');
	assert(memcmp(buf2, "XY", 3) == 0);
	assert(p2 == buf2 + 2);

	SUCCESS();
	return EXIT_SUCCESS;
}
