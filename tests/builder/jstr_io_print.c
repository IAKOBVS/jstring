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

	/* positive */
	assert(jstr_io_print(&j) == JSTR_RET_SUCC);

	/* empty */
	jstr_ty e = JSTR_INIT;
	assert(jstr_io_print(&e) == JSTR_RET_SUCC);

	/* embedded? size authoritative */
	assert(jstr_assign_len(&j.data, &j.size, &j.capacity, "a\0b", 3) == JSTR_RET_SUCC);
	assert(jstr_io_print(&j) == JSTR_RET_SUCC);

	jstr_free_j(&j);
	jstr_free_j(&e);
	return EXIT_SUCCESS;
}
