/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include "test.h"
#include "test-array.h"

int
main(void)
{
	t_memmem("jstr_strrstr_len", (t_memmem_fn)jstr_strrstr_len, (t_memmem_fn)simple_strrstr_len, test_array_memmem, JSTR_ARRAY_COUNT(test_array_memmem));
	return EXIT_SUCCESS;
}
