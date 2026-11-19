#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	setenv("HOME", "/home/tester", 1);
	/* no tilde */
	{
		jstr_ty j = JSTR_INIT;
		jstr_ty jj = JSTR_INIT;
		/* build buffer */
		jj.data = malloc(8);
		strcpy(jj.data, "foo");
		jj.size = 3;
		jj.capacity = 8;
		int r = jstr_io_expandtildefirst(&jj.data, &jj.size, &jj.capacity);
		assert(r == JSTR_RET_SUCC);
		assert(strcmp(jj.data, "foo") == 0);
		assert(jj.size == 3);
		free(jj.data);
	}
	/* tilde */
	{
		jstr_ty jj = JSTR_INIT;
		jj.data = malloc(8);
		strcpy(jj.data, "~");
		jj.size = 1;
		jj.capacity = 8;
		int r = jstr_io_expandtildefirst(&jj.data, &jj.size, &jj.capacity);
		assert(r == JSTR_RET_SUCC);
		assert(strcmp(jj.data, "/home/tester") == 0);
		assert(jj.size > 0);
		free(jj.data);
	}
	/* missing HOME */
	{
		unsetenv("HOME");
		jstr_ty jj = JSTR_INIT;
		jj.data = malloc(8);
		strcpy(jj.data, "~");
		jj.size = 1;
		jj.capacity = 8;
		int r = jstr_io_expandtildefirst(&jj.data, &jj.size, &jj.capacity);
		assert(r != JSTR_RET_SUCC);
		free(jj.data);
		setenv("HOME", "/home/tester", 1);
	}
	return 0;
}
