/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

int
main(void)
{

	char *s = NULL;
	size_t sz = 0, cap = 0;

	assert(jstr_pushfrontn(&s, &sz, &cap, 'a', 3) == JSTR_RET_SUCC);
	assert(sz == 3);
	assert(memcmp(s, "aaa", 4) == 0);

	assert(jstr_pushfrontn(&s, &sz, &cap, 'b', 2) == JSTR_RET_SUCC);
	assert(sz == 5);
	assert(memcmp(s, "bbaaa", 6) == 0);

	assert(jstr_pushfrontn(&s, &sz, &cap, 'c', 0) == JSTR_RET_SUCC);
	assert(sz == 5);

	jstr_free(&s, &sz, &cap);
	return EXIT_SUCCESS;
}
