#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(jstr_io_readsystem(&s, &sz, &cap, "echo hello") == JSTR_RET_SUCC);
		assert(sz > 0);
		assert(strstr(s, "hello") != NULL);
		free(s);
	}
	/* no output -> error */
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(jstr_io_readsystem(&s, &sz, &cap, "true") != JSTR_RET_SUCC);
		free(s);
	}
	/* bad command */
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		assert(jstr_io_readsystem(&s, &sz, &cap, "nonexistent_command_xyz") != JSTR_RET_SUCC);
		free(s);
	}
	return 0;
}
