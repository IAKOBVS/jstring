#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <string.h>

int
main(int argc, char **argv)
{
	START();
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		/* seed */
		const char *init = "dir";
		size_t initsz = 3;
		s = malloc(initsz + 1);
		memcpy(s, init, initsz);
		s[initsz] = '\0';
		sz = initsz;
		cap = initsz + 1;
		int r = jstr_io_appendpath_len(&s, &sz, &cap, jstr_literal("file"));
		assert(r == JSTR_RET_SUCC);
		assert(sz == 8);
		assert(strcmp(s, "dir/file") == 0);
		free(s);
	}
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		s = malloc(2);
		s[0] = 'a'; s[1] = '\0';
		sz = 1; cap = 2;
		int r = jstr_io_appendpath_len(&s, &sz, &cap, jstr_literal(""));
		assert(r == JSTR_RET_SUCC);
		assert(strcmp(s, "a/") == 0);
		free(s);
	}
	SUCCESS();
	return 0;
}
