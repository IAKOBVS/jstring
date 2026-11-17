/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stdlib.h>

#include "../test.h"

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

	char *s = NULL;
	size_t sz = 0, cap = 0;

	/* no-op when new_cap <= cap */
	assert(jstr_reserve(&s, &sz, &cap, 0) == JSTR_RET_SUCC);
	assert(s == NULL);
	assert(cap == 0);

	assert(jstr_reserve(&s, &sz, &cap, 8) == JSTR_RET_SUCC);
	assert(s != NULL);
	size_t c = cap;
	assert(jstr_reserve(&s, &sz, &cap, c) == JSTR_RET_SUCC);
	assert(cap == c);
	assert(jstr_reserve(&s, &sz, &cap, c - 1) == JSTR_RET_SUCC);
	assert(cap == c);

	/* grows when needed */
	assert(jstr_reserve(&s, &sz, &cap, c + 1) == JSTR_RET_SUCC);
	assert(cap >= c + 1);

	jstr_free(&s, &sz, &cap);
	SUCCESS();
	return EXIT_SUCCESS;
}
