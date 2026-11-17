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

	assert(jstr_assign_len(&s, &sz, &cap, "hello", 5) == JSTR_RET_SUCC);
	assert(sz == 5);
	assert(memcmp(s, "hello", 6) == 0);

	assert(jstr_assign_len(&s, &sz, &cap, "", 0) == JSTR_RET_SUCC);
	assert(sz == 0);
	assert(s[0] == '\0');

	assert(jstr_assign_len(&s, &sz, &cap, "ab", 2) == JSTR_RET_SUCC);
	assert(sz == 2);
	assert(memcmp(s, "ab", 3) == 0);

	jstr_free(&s, &sz, &cap);
	SUCCESS();
	return EXIT_SUCCESS;
}
