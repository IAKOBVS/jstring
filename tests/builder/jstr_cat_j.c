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

	jstr_ty j = JSTR_INIT;
	jstr_cat_arg_ty args[] = { { "ab", 2 }, { "cd", 2 } };
	assert(jstr_cat_j(&j, args, 2) == JSTR_RET_SUCC);
	assert(j.size == 4);
	assert(memcmp(j.data, "abcd", 5) == 0);

	/* empty total */
	jstr_cat_arg_ty e[] = { { NULL, 0 } };
	assert(jstr_cat_j(&j, e, 1) == JSTR_RET_SUCC);
	assert(j.size == 4);

	jstr_free_j(&j);
	SUCCESS();
	return EXIT_SUCCESS;
}
