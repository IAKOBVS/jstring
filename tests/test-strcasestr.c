/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#define _GNU_SOURCE

#include "test-array.h"
#include "test.h"

int
main(int argc, char **argv)
{
	START();
	T_STRSTR(jstr_strcasestr, simple_strcasestr, test_array_memmem);
	SUCCESS();
	return EXIT_SUCCESS;
}
