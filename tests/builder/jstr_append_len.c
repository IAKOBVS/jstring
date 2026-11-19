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

	assert(jstr_append_len(&s, &sz, &cap, "abc", 3) == JSTR_RET_SUCC);
	assert(sz == 3);
	assert(memcmp(s, "abc", 4) == 0);

	/* append more */
	assert(jstr_append_len(&s, &sz, &cap, "de", 2) == JSTR_RET_SUCC);
	assert(sz == 5);
	assert(memcmp(s, "abcde", 6) == 0);

	/* zero length */
	assert(jstr_append_len(&s, &sz, &cap, "x", 0) == JSTR_RET_SUCC);
	assert(sz == 5);

	jstr_free(&s, &sz, &cap);
	return EXIT_SUCCESS;
}
