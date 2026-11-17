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

	char *p = jstr_append_len_unsafe_p(buf, sz, "XYZ", 3);
	assert(memcmp(buf, "hiXYZ", 6) == 0);
	assert(buf[5] == '\0');
	assert(p == buf + 5);

	/* zero length append */
	char *p2 = jstr_append_len_unsafe_p(buf, 5, "abc", 0);
	assert(memcmp(buf, "hiXYZ", 6) == 0);
	assert(p2 == buf + 5);

	SUCCESS();
	return EXIT_SUCCESS;
}
