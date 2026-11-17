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
	assert(jstr_assign_len(&j.data, &j.size, &j.capacity, "abc", 3) == JSTR_RET_SUCC);
	assert(jstr_debug(&j) == JSTR_RET_SUCC);

	jstr_ty e = JSTR_INIT;
	assert(jstr_debug(&e) == JSTR_RET_SUCC);

	/* null data */
	jstr_ty n = JSTR_INIT;
	n.data = NULL;
	n.size = 0;
	n.capacity = 0;
	assert(jstr_debug(&n) == JSTR_RET_SUCC);

	jstr_free_j(&j);
	jstr_free_j(&e);
	SUCCESS();
	return EXIT_SUCCESS;
}
