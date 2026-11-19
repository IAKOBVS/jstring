/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stdlib.h>

#include "../test.h"

int
main(void)
{

	char *s = NULL;
	size_t sz = 0, cap = 0;

	/* new_cap == 0 leaves capacity unchanged and returns success */
	assert(jstr_reserveexactalways(&s, &sz, &cap, 0) == JSTR_RET_SUCC);
	assert(s == NULL);
	assert(cap == 0);

	/* non-zero sets exact capacity and allocates */
	assert(jstr_reserveexactalways(&s, &sz, &cap, 32) == JSTR_RET_SUCC);
	assert(s != NULL);
	assert(cap == 32);

	/* reallocates to new exact size */
	assert(jstr_reserveexactalways(&s, &sz, &cap, 64) == JSTR_RET_SUCC);
	assert(cap == 64);

	jstr_free(&s, &sz, &cap);
	return EXIT_SUCCESS;
}
