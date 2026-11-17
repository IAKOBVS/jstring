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
	char *p = jstr_assign_len_unsafe_p(buf, "hello", 5);
	assert(memcmp(buf, "hello", 6) == 0);
	assert(p == buf + 5);

	char *p2 = jstr_assign_len_unsafe_p(buf, "hi", 2);
	assert(memcmp(buf, "hi", 3) == 0);
	assert(p2 == buf + 2);

	SUCCESS();
	return EXIT_SUCCESS;
}
