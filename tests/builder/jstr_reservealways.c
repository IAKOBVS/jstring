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

	/* new_cap 0 still reallocates to at least JSTR_MIN_CAP */
	assert(jstr_reservealways(&s, &sz, &cap, 0) == JSTR_RET_SUCC);
	assert(s != NULL);
	assert(cap >= 1);
	assert(sz == 0);

	/* growth to requested capacity */
	size_t oldcap = cap;
	assert(jstr_reservealways(&s, &sz, &cap, 100) == JSTR_RET_SUCC);
	assert(cap >= 100);
	assert(cap > oldcap);
	assert(sz == 0);

	/* sz unchanged */
	sz = 7;
	/* don't actually write; just ensure size preserved */
	assert(jstr_reservealways(&s, &sz, &cap, 101) == JSTR_RET_SUCC);
	assert(sz == 7);

	jstr_free(&s, &sz, &cap);
	return EXIT_SUCCESS;
}
