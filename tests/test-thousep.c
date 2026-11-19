/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include "test.h"
#include "../include/itoa.h"

static char buf[1024];

static void
check_thousep(const char *string, const char *expected_result)
{
	memcpy(buf, string, strlen(string) + 1);
	test_expect_true("jstr_thousep_p", "advance to end", jstr_thousep_p(buf, ',') == buf + strlen(buf));
	test_expect_true("jstr_thousep_p", "string content", !strcmp(buf, expected_result));
	test_expect_true("jstr_thousepcpy_p", "advance to end", jstr_thousepcpy_p(buf, string, ',') == buf + strlen(buf));
	test_expect_true("jstr_thousepcpy_p", "string content", !strcmp(buf, expected_result));
}

int
main(void)
{
	test_testing("jstr_thousep_p");
	test_testing("jstr_thousepcpy_p");
	check_thousep("1", "1");
	check_thousep("12", "12");
	check_thousep("123", "123");
	check_thousep("1234", "1,234");
	check_thousep("12345", "12,345");
	check_thousep("123456", "123,456");
	check_thousep("1234567", "1,234,567");
	check_thousep("12345678", "12,345,678");
	check_thousep("123456789", "123,456,789");
	return EXIT_SUCCESS;
}
