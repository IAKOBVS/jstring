/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include "test.h"
#include "test-array.h"
#include "../include/string.h"

int
main(void)
{
	t_memcmp("jstr_strncasecmp", (t_memcmp_fn)jstr_strncasecmp, (t_memcmp_fn)simple_strncasecmp, test_array_memcmp, JSTR_ARRAY_COUNT(test_array_memcmp), 0);
	t_strcmp("jstr_strcasecmp", jstr_strcasecmp, simple_strcasecmp, test_array_memcmp, JSTR_ARRAY_COUNT(test_array_memcmp), 0);
	return EXIT_SUCCESS;
}
