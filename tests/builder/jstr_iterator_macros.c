/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* RIGOR: builder pointer/iterator macros used by the documented API. */

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "jstr.h"

int
main(void)
{
	jstr_ty j = JSTR_INIT;
	assert(!jstr_chk(jstr_assign_len(jstr_struct(&j), "abc", 3)));

	assert(jstr_start(&j) == j.data);
	assert(jstr_end(&j) == j.data + j.size);
	assert(jstr_at(&j, 1) == j.data + 1);
	assert(jstr_internal_at(&j, 2) == j.data + 2);
	assert(jstr_index(&j, j.data + 2) == 2);

	size_t n = 0;
	jstr_foreach(&j, p)
	{
		assert(*p == j.data[n]);
		++n;
	}
	assert(n == j.size);

	size_t sum = 0;
	jstr_foreachi(&j, i)
		sum += i;
	assert(sum == 3);

	jstr_free_j(&j);
	return 0;
}
