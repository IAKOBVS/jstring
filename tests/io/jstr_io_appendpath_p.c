#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <string.h>

int
main(void)
{
	{
		char buf[64];
		memcpy(buf, jstr_literal("dir"));
		buf[3] = '\0';
		char *p = jstr_io_appendpath_p(buf, 3, "file");
		assert(strcmp(buf, "dir/file") == 0);
		assert(p == buf + 8);
	}
	{
		char buf[64] = "a";
		char *p = jstr_io_appendpath_p(buf, 1, "bc");
		assert(strcmp(buf, "a/bc") == 0);
		assert(p == buf + 4);
	}
	return 0;
}
