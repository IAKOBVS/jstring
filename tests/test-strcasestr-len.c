/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include "test-array.h"
#include "test.h"

int
main(int argc, char **argv)
{
	START();
	T_MEMMEM(jstr_strcasestr_len, simple_strcasestr_len, test_array_memmem);
	SUCCESS();
	return EXIT_SUCCESS;
}
