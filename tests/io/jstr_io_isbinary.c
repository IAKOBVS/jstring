#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <string.h>

int
main(void)
{
	assert(jstr_io_isbinary(jstr_literal("")) == 0);
	{
		const unsigned char b[] = { 0x7F, 'E', 'L', 'F', 0, 0, 0, 0 };
		assert(jstr_io_isbinary((const char *)b, 8) == 1);
	}
	{
		const unsigned char b[] = { 0xEF, 0xBB, 0xBF, 'a', 'b', 'c' };
		assert(jstr_io_isbinary((const char *)b, 6) == 0);
	}
	/* embedded NUL: strlen != sz */
	{
		const char b[] = "ab\0cd";
		assert(jstr_io_isbinary(b, 5) == 1);
	}
	/* text matches */
	{
		const char *s = "hello";
		assert(jstr_io_isbinary(s, 5) == 0);
	}
	return 0;
}
