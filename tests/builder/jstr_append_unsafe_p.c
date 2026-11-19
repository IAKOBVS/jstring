/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

int
main(void)
{

	char buf[64];
	strcpy(buf, "hi");
	size_t sz = 2;

	char *p = jstr_append_unsafe_p(buf, sz, "XYZ");
	assert(strcmp(buf, "hiXYZ") == 0);
	assert(p == buf + 5);

	char *p2 = jstr_append_unsafe_p(buf, 5, "");
	assert(p2 == buf + 5);

	return EXIT_SUCCESS;
}
