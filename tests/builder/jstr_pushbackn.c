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

	assert(jstr_pushbackn(&s, &sz, &cap, 'a', 4) == JSTR_RET_SUCC);
	assert(sz == 4);
	assert(memcmp(s, "aaaa", 4) == 0);

	/* append more */
	assert(jstr_pushbackn(&s, &sz, &cap, 'b', 2) == JSTR_RET_SUCC);
	assert(sz == 6);
	assert(memcmp(s, "aaaabb", 6) == 0);

	/* n == 0 */
	assert(jstr_pushbackn(&s, &sz, &cap, 'c', 0) == JSTR_RET_SUCC);
	assert(sz == 6);

	jstr_free(&s, &sz, &cap);
	return EXIT_SUCCESS;
}
