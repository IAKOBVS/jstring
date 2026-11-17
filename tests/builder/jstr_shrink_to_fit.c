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

	char *s = NULL;
	size_t sz = 0, cap = 0;
	assert(jstr_assign_len(&s, &sz, &cap, "abc", 3) == JSTR_RET_SUCC);

	/* shrink to size + 1 */
	assert(jstr_shrink_to_fit(&s, &sz, &cap) == JSTR_RET_SUCC);
	assert(cap == sz + 1);
	assert(strcmp(s, "abc") == 0);
	assert(sz == 3);

	/* empty string: capacity becomes 1 */
	char *e = NULL;
	size_t esz = 0, ecap = 0;
	assert(jstr_shrink_to_fit(&e, &esz, &ecap) == JSTR_RET_SUCC);
	assert(ecap == 1);

	jstr_free(&s, &sz, &cap);
	jstr_free(&e, &esz, &ecap);
	SUCCESS();
	return EXIT_SUCCESS;
}
