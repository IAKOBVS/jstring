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

	char *s = malloc(16);
	size_t sz = 5, cap = 16;
	strcpy(s, "hello");

	jstr_free(&s, &sz, &cap);
	assert(s == NULL);
	assert(sz == 0);
	assert(cap == 0);

	/* free(NULL) safe */
	char *n = NULL;
	size_t ns = 0, nc = 0;
	jstr_free(&n, &ns, &nc);
	assert(n == NULL);

	SUCCESS();
	return EXIT_SUCCESS;
}
