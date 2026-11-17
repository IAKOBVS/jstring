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

	assert(jstr_reserveexact(&s, &sz, &cap, 16) == JSTR_RET_SUCC);
	assert(cap == 16);

	/* no-op when new_cap < cap */
	size_t c = cap;
	assert(jstr_reserveexact(&s, &sz, &cap, c - 1) == JSTR_RET_SUCC);
	assert(cap == c);

	/* equal reallocates (strict less-than no-op) */
	assert(jstr_reserveexact(&s, &sz, &cap, c) == JSTR_RET_SUCC);
	assert(cap == c);

	/* larger reallocates */
	assert(jstr_reserveexact(&s, &sz, &cap, c + 8) == JSTR_RET_SUCC);
	assert(cap == c + 8);

	jstr_free(&s, &sz, &cap);
	SUCCESS();
	return EXIT_SUCCESS;
}
