/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

int
main(void)
{

	jstr_ty j = JSTR_INIT;
	assert(jstr_assign_len(&j.data, &j.size, &j.capacity, "abc", 3) == JSTR_RET_SUCC);
	assert(jstr_io_println(&j) == JSTR_RET_SUCC);

	jstr_ty e = JSTR_INIT;
	assert(jstr_io_println(&e) == JSTR_RET_SUCC);

	jstr_free_j(&j);
	jstr_free_j(&e);
	return EXIT_SUCCESS;
}
