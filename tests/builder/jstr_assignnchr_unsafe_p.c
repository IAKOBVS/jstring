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
	char *p = jstr_assignnchr_unsafe_p(buf, 'a', 4);
	assert(memcmp(buf, "aaaa", 5) == 0);
	assert(p == buf + 4);

	/* n == 0 writes only NUL */
	char *p2 = jstr_assignnchr_unsafe_p(buf, 'b', 0);
	assert(buf[0] == '\0');
	assert(p2 == buf);

	SUCCESS();
	return EXIT_SUCCESS;
}
