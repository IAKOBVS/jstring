/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 * Shared struct types for test array data. */

#ifndef TEST_TYPES_H
#define TEST_TYPES_H

struct test_array_memmem {
	const char *hs;
	const char *ne;
};

struct test_array_memcmp {
	const char *s1;
	const char *s2;
};

#endif /* TEST_TYPES_H */
