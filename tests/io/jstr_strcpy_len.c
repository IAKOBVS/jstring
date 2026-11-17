#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <string.h>

int
main(int argc, char **argv)
{
	START();
	{
		char dst[64];
		jstr_strcpy_len(dst, jstr_literal("hello"));
		assert(strcmp(dst, "hello") == 0);
		assert(dst[5] == '\0');
	}
	{
		char dst[64];
		jstr_strcpy_len(dst, jstr_literal("ab\0cd"));
		assert(memcmp(dst, "ab\0cd", 5) == 0);
		assert(dst[5] == '\0');
	}
	{
		char dst[64];
		jstr_strcpy_len(dst, "x", 0);
		assert(dst[0] == '\0');
	}
	SUCCESS();
	return 0;
}
