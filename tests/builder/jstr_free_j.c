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
	assert(jstr_assign_len(&j.data, &j.size, &j.capacity, "hi", 2) == JSTR_RET_SUCC);
	jstr_free_j(&j);
	assert(j.data == NULL);
	assert(j.size == 0);
	assert(j.capacity == 0);

	SUCCESS();
	return EXIT_SUCCESS;
}
