/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{

	/* Exact small output. */
	{
		jstr_ty j = JSTR_INIT;
		assert(jstr_io_readsystem_j(&j, "printf '%s' 'hi'") == JSTR_RET_SUCC);
		assert(j.size == 2);
		assert(strcmp(j.data, "hi") == 0);
		assert(j.data[j.size] == '\0');
		free(j.data);
	}

	/* Embedded newlines and exact byte count. */
	{
		jstr_ty j = JSTR_INIT;
		assert(jstr_io_readsystem_j(&j, "printf 'a\\nb\\n'") == JSTR_RET_SUCC);
		assert(j.size == 4);
		assert(memcmp(j.data, "a\nb\n", 5) == 0);
		assert(j.data[j.size] == '\0');
		free(j.data);
	}

	/* A command with no stdout output is an error. */
	{
		jstr_ty j = JSTR_INIT;
		assert(jstr_io_readsystem_j(&j, "true") != JSTR_RET_SUCC);
		assert(j.data == NULL);
		assert(j.size == 0);
		free(j.data);
	}

	/* A nonexistent command produces no stdout and is an error. */
	{
		jstr_ty j = JSTR_INIT;
		assert(jstr_io_readsystem_j(&j, "nonexistent_command_xyz 2>/dev/null") != JSTR_RET_SUCC);
		free(j.data);
	}

	/* More than one page of output forces the growth loop. */
	{
		const size_t out_len = 10000;
		char *cmd = malloc(out_len + 32);
		char *expect = malloc(out_len);
		jstr_ty j = JSTR_INIT;
		assert(cmd != NULL && expect != NULL);
		memset(expect, 'x', out_len);
		snprintf(cmd, out_len + 32, "printf '%%s' '");
		memcpy(cmd + strlen(cmd), expect, out_len);
		strcat(cmd, "'");
		assert(jstr_io_readsystem_j(&j, cmd) == JSTR_RET_SUCC);
		assert(j.size == out_len);
		assert(j.capacity > j.size);
		assert(memcmp(j.data, expect, out_len) == 0);
		assert(j.data[j.size] == '\0');
		free(cmd);
		free(expect);
		free(j.data);
	}

	/* A later, shorter command replaces rather than appends. */
	{
		jstr_ty j = JSTR_INIT;
		assert(jstr_io_readsystem_j(&j, "printf '%s' 'longer output'") == JSTR_RET_SUCC);
		assert(j.size == strlen("longer output"));
		assert(strcmp(j.data, "longer output") == 0);
		assert(jstr_io_readsystem_j(&j, "printf '%s' 'x'") == JSTR_RET_SUCC);
		assert(j.size == 1);
		assert(strcmp(j.data, "x") == 0);
		assert(j.data[j.size] == '\0');
		free(j.data);
	}

	/* An empty shell format string also yields no output -> error. */
	{
		jstr_ty j = JSTR_INIT;
		assert(jstr_io_readsystem_j(&j, "printf ''") != JSTR_RET_SUCC);
		free(j.data);
	}

	return 0;
}
