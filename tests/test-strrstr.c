/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include "test.h"
#include "test-array.h"

int
main(int argc, char **argv)
{
	START();
	T_MEMMEM(jstr_strrstr_len, simple_strrstr_len, test_array_memmem);
	SUCCESS();
	return EXIT_SUCCESS;
}
