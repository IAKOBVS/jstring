#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <string.h>

int
main(int argc, char **argv)
{
	START();
	{
		char buf[64];
		memcpy(buf, jstr_literal("dir"));
		buf[3] = '\0';
		char *p = jstr_io_appendpath_len_p(buf, 3, jstr_literal("file"));
		assert(p == buf + 8);
		assert(strcmp(buf, "dir/file") == 0);
		assert(*p == '\0');
	}
	{
		char buf[64] = "x";
		char *p = jstr_io_appendpath_len_p(buf, 1, jstr_literal("y"));
		assert(strcmp(buf, "x/y") == 0);
		assert(p == buf + 3);
	}
	/* empty fname */
	{
		char buf[64];
		memcpy(buf, jstr_literal("d"));
		buf[1] = '\0';
		char *p = jstr_io_appendpath_len_p(buf, 1, jstr_literal(""));
		assert(strcmp(buf, "d/") == 0);
		assert(p == buf + 2);
	}
	SUCCESS();
	return 0;
}
