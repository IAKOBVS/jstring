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

	/* total == 0: no changes */
	jstr_cat_arg_ty empty[1] = { { NULL, 0 } };
	assert(jstr_cat(&s, &sz, &cap, empty, 1) == JSTR_RET_SUCC);
	assert(sz == 0);

	/* multiple chunks */
	jstr_cat_arg_ty args[] = {
		{ "foo", 3 },
		{ "bar", 3 },
	};
	assert(jstr_cat(&s, &sz, &cap, args, 2) == JSTR_RET_SUCC);
	assert(sz == 6);
	assert(memcmp(s, "foobar", 7) == 0);

	/* append to existing */
	jstr_cat_arg_ty more[] = { { "!", 1 } };
	assert(jstr_cat(&s, &sz, &cap, more, 1) == JSTR_RET_SUCC);
	assert(sz == 7);
	assert(memcmp(s, "foobar!", 8) == 0);

	/* nargs 0 */
	assert(jstr_cat(&s, &sz, &cap, NULL, 0) == JSTR_RET_SUCC);

	jstr_free(&s, &sz, &cap);
	SUCCESS();
	return EXIT_SUCCESS;
}
