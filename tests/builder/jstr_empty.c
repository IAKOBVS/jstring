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

	char buf[16] = "hello";
	size_t sz = 5;

	/* positive: non-NULL buffer is emptied */
	jstr_empty(buf, &sz);
	assert(sz == 0);
	assert(buf[0] == '\0');

	/* capacity unchanged (caller retains storage) */
	buf[0] = 'x';
	sz = 3;
	jstr_empty(buf, &sz);
	assert(sz == 0);
	assert(buf[0] == '\0');

	/* NULL buffer: only size cleared */
	sz = 99;
	jstr_empty(NULL, &sz);
	assert(sz == 0);

	/* edge: size pointer already zero */
	sz = 0;
	buf[0] = 'a';
	jstr_empty(buf, &sz);
	assert(sz == 0);
	assert(buf[0] == '\0');

	SUCCESS();
	return EXIT_SUCCESS;
}
