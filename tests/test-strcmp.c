/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include "test.h"
#include "test-array.h"
#include "../include/string.h"

int
main(int argc, char **argv)
{
	START();
	T_MEMCMP(jstr_strncasecmp, simple_strncasecmp, test_array_memcmp);
	T_STRCMP(jstr_strcasecmp, simple_strcasecmp, test_array_memcmp);
	SUCCESS();
	return EXIT_SUCCESS;
}
