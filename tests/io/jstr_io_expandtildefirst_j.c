/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static void
seed_j(jstr_ty *j, const char *s, size_t cap)
{
	j->data = malloc(cap);
	assert(j->data != NULL);
	memcpy(j->data, s, strlen(s) + 1);
	j->size = strlen(s);
	j->capacity = cap;
}

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

	setenv("HOME", "/home/tester", 1);

	/* Bare tilde expands to HOME and NUL-terminates the result. */
	{
		jstr_ty j = JSTR_INIT;
		seed_j(&j, "~", 8);
		assert(jstr_io_expandtildefirst_j(&j) == JSTR_RET_SUCC);
		assert(strcmp(j.data, "/home/tester") == 0);
		assert(j.data[strlen("/home/tester")] == '\0');
		free(j.data);
	}

	/* A tilde prefix expands only the first byte and keeps the remainder. */
	{
		jstr_ty j = JSTR_INIT;
		seed_j(&j, "~/sub/dir", 128);
		assert(jstr_io_expandtildefirst_j(&j) == JSTR_RET_SUCC);
		assert(strcmp(j.data, "/home/tester/sub/dir") == 0);
		assert(j.data[strlen("/home/tester/sub/dir")] == '\0');
		free(j.data);
	}

	/* "~user" expands the tilde and leaves "user" attached. */
	{
		jstr_ty j = JSTR_INIT;
		seed_j(&j, "~user", 64);
		assert(jstr_io_expandtildefirst_j(&j) == JSTR_RET_SUCC);
		assert(strcmp(j.data, "/home/testeruser") == 0);
		free(j.data);
	}

	/* No leading tilde is a no-op: same pointer, bytes, and size. */
	{
		jstr_ty j = JSTR_INIT;
		seed_j(&j, "foo~bar", 32);
		char *const data = j.data;
		const size_t cap = j.capacity;
		assert(jstr_io_expandtildefirst_j(&j) == JSTR_RET_SUCC);
		assert(j.data == data);
		assert(j.capacity == cap);
		assert(j.size == strlen("foo~bar"));
		assert(strcmp(j.data, "foo~bar") == 0);
		free(j.data);
	}

	/* An empty string is also a no-op. */
	{
		jstr_ty j = JSTR_INIT;
		seed_j(&j, "", 4);
		assert(jstr_io_expandtildefirst_j(&j) == JSTR_RET_SUCC);
		assert(j.size == 0);
		assert(j.data[0] == '\0');
		free(j.data);
	}

	/* Growth from a tiny capacity preserves the tail. */
	{
		jstr_ty j = JSTR_INIT;
		seed_j(&j, "~/x", 4);
		assert(jstr_io_expandtildefirst_j(&j) == JSTR_RET_SUCC);
		assert(strcmp(j.data, "/home/tester/x") == 0);
		assert(j.capacity > strlen("/home/tester/x"));
		assert(j.data[strlen("/home/tester/x")] == '\0');
		free(j.data);
	}

	/* Empty HOME: the tilde is removed and the result is empty. */
	{
		jstr_ty j = JSTR_INIT;
		setenv("HOME", "", 1);
		seed_j(&j, "~", 8);
		assert(jstr_io_expandtildefirst_j(&j) == JSTR_RET_SUCC);
		assert(j.data[0] == '\0');
		free(j.data);
		setenv("HOME", "/home/tester", 1);
	}

	/*
	 * Regression: the documented contract says *sz is updated when '~' is
	 * expanded.  The implementation currently does *sz += strlen(HOME)
	 * instead of *sz += strlen(HOME) - 1, so the reported size is one past
	 * the terminator.  Keep asserting the documented (correct) size.
	 */
	{
		jstr_ty j = JSTR_INIT;
		seed_j(&j, "~/x", 4);
		assert(jstr_io_expandtildefirst_j(&j) == JSTR_RET_SUCC);
		assert(strcmp(j.data, "/home/tester/x") == 0);
		assert(j.size == strlen("/home/tester/x"));
		assert(j.data[j.size] == '\0');
		free(j.data);
	}

	/* Missing HOME is an error and must not corrupt the input. */
	{
		jstr_ty j = JSTR_INIT;
		unsetenv("HOME");
		seed_j(&j, "~/keep", 32);
		assert(jstr_io_expandtildefirst_j(&j) != JSTR_RET_SUCC);
		assert(strcmp(j.data, "~/keep") == 0);
		assert(j.size == strlen("~/keep"));
		free(j.data);
		setenv("HOME", "/home/tester", 1);
	}

	SUCCESS();
	return 0;
}
