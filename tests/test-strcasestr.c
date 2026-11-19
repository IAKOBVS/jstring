/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#define _GNU_SOURCE

#include "test-array.h"
#include "test.h"

int
main(void)
{
	t_strstr("jstr_strcasestr", jstr_strcasestr, simple_strcasestr, test_array_memmem, JSTR_ARRAY_COUNT(test_array_memmem));
	return EXIT_SUCCESS;
}
