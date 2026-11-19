/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include "test-array.h"
#include "test.h"

int
main(void)
{
	t_memmem("jstr_memmem", (t_memmem_fn)jstr_memmem, (t_memmem_fn)simple_memmem, test_array_memmem, JSTR_ARRAY_COUNT(test_array_memmem));
	return EXIT_SUCCESS;
}
