/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include "test.h"

int
main(int argc, char **argv)
{
	START();

	// Test 1: Empty string (NUL-terminated)
	{
		jstr_line_iter_ty iter;
		jstr_line_iter_init(&iter, "");
		assert(jstr_line_iter_next(&iter) == 0);
	}

	// Test 2: Empty string (bounded length)
	{
		jstr_line_iter_ty iter;
		jstr_line_iter_init_len(&iter, "", 0);
		assert(jstr_line_iter_next(&iter) == 0);
	}

	// Test 3: Single line without newline (NUL-terminated)
	{
		jstr_line_iter_ty iter;
		jstr_line_iter_init(&iter, "hello");
		assert(jstr_line_iter_next(&iter) == 1);
		assert(iter.len == 5);
		assert(strncmp(iter.line, "hello", 5) == 0);
		assert(jstr_line_iter_next(&iter) == 0);
	}

	// Test 4: Single line with newline (bounded)
	{
		jstr_line_iter_ty iter;
		jstr_line_iter_init_len(&iter, "hello\n", 6);
		assert(jstr_line_iter_next(&iter) == 1);
		assert(iter.len == 5);
		assert(strncmp(iter.line, "hello", 5) == 0);
		assert(jstr_line_iter_next(&iter) == 0);
	}

	// Test 5: Multiple lines with LF (NUL-terminated)
	{
		jstr_line_iter_ty iter;
		jstr_line_iter_init(&iter, "line1\nline2\nline3");
		assert(jstr_line_iter_next(&iter) == 1);
		assert(iter.len == 5);
		assert(strncmp(iter.line, "line1", 5) == 0);

		assert(jstr_line_iter_next(&iter) == 1);
		assert(iter.len == 5);
		assert(strncmp(iter.line, "line2", 5) == 0);

		assert(jstr_line_iter_next(&iter) == 1);
		assert(iter.len == 5);
		assert(strncmp(iter.line, "line3", 5) == 0);

		assert(jstr_line_iter_next(&iter) == 0);
	}

	// Test 6: CRLF endings (NUL-terminated)
	{
		jstr_line_iter_ty iter;
		jstr_line_iter_init(&iter, "hello\r\nworld\r\n");
		assert(jstr_line_iter_next(&iter) == 1);
		assert(iter.len == 5);
		assert(strncmp(iter.line, "hello", 5) == 0);

		assert(jstr_line_iter_next(&iter) == 1);
		assert(iter.len == 5);
		assert(strncmp(iter.line, "world", 5) == 0);

		assert(jstr_line_iter_next(&iter) == 0);
	}

	// Test 7: Empty lines (NUL-terminated)
	{
		jstr_line_iter_ty iter;
		jstr_line_iter_init(&iter, "\n\n");
		assert(jstr_line_iter_next(&iter) == 1);
		assert(iter.len == 0);

		assert(jstr_line_iter_next(&iter) == 1);
		assert(iter.len == 0);

		assert(jstr_line_iter_next(&iter) == 0);
	}

	// Test 8: Empty lines with CRLF (bounded)
	{
		jstr_line_iter_ty iter;
		jstr_line_iter_init_len(&iter, "\r\n\r\n", 4);
		assert(jstr_line_iter_next(&iter) == 1);
		assert(iter.len == 0);

		assert(jstr_line_iter_next(&iter) == 1);
		assert(iter.len == 0);

		assert(jstr_line_iter_next(&iter) == 0);
	}

	// Test 9: Bounded slice without NUL-termination
	{
		jstr_line_iter_ty iter;
		// Iterate over "abc\nd" slice of "abc\ndef"
		jstr_line_iter_init_len(&iter, "abc\ndef", 5);
		assert(jstr_line_iter_next(&iter) == 1);
		assert(iter.len == 3);
		assert(strncmp(iter.line, "abc", 3) == 0);

		assert(jstr_line_iter_next(&iter) == 1);
		assert(iter.len == 1);
		assert(strncmp(iter.line, "d", 1) == 0);

		assert(jstr_line_iter_next(&iter) == 0);
	}

	SUCCESS();
	return EXIT_SUCCESS;
}
