/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* jstr_assignnchr must behave like a truncating fill for every n relative
 * to the current size: shrink (truncate + terminate + update sz), keep,
 * and grow. The old code left a stale tail and the old size when n < sz. */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{
	char *s = NULL;
	size_t sz = 0, cap = 0;

	/* Shrink: n < sz. Old code kept sz == 5 with data "zzllo". */
	assert(!jstr_chk(jstr_assign_len(&s, &sz, &cap, "hello", 5)));
	assert(jstr_assignnchr(&s, &sz, &cap, 'z', 2) == JSTR_RET_SUCC);
	assert(sz == 2);
	assert(memcmp(s, "zz", 3) == 0); /* includes NUL at [2] */

	/* Keep: n == sz. */
	assert(jstr_assignnchr(&s, &sz, &cap, 'q', 2) == JSTR_RET_SUCC);
	assert(sz == 2);
	assert(memcmp(s, "qq", 3) == 0);

	/* Grow: n > sz. */
	assert(jstr_assignnchr(&s, &sz, &cap, 'w', 6) == JSTR_RET_SUCC);
	assert(sz == 6);
	assert(memcmp(s, "wwwwww", 7) == 0);

	/* Zero: truncates to empty. */
	assert(jstr_assignnchr(&s, &sz, &cap, 'x', 0) == JSTR_RET_SUCC);
	assert(sz == 0);
	assert(s[0] == '\0');

	jstr_free(&s, &sz, &cap);
	return EXIT_SUCCESS;
}
