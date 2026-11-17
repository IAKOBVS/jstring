#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <string.h>

int
main(int argc, char **argv)
{
	START();
	/* empty */
	assert(jstr_io_isbinary_maybe(jstr_literal("")) == 0);
	/* NULL with sz 0 */
	assert(jstr_io_isbinary_maybe(NULL, 0) == 0);
	/* ELF signature */
	{
		const unsigned char b[] = { 0x7F, 'E', 'L', 'F', 0, 0, 0, 0 };
		assert(jstr_io_isbinary_maybe((const char *)b, sizeof(b)) == 1);
	}
	/* UTF-8 BOM */
	{
		const unsigned char b[] = { 0xEF, 0xBB, 0xBF, 'a', 'b', 'c' };
		assert(jstr_io_isbinary_maybe((const char *)b, sizeof(b)) == 0);
	}
	/* control char */
	{
		const char b[] = "ab\0cd";
		assert(jstr_io_isbinary_maybe(b, 5) == 1);
	}
	/* printable text */
	{
		const char *s = "hello\tworld\n";
		assert(jstr_io_isbinary_maybe(s, strlen(s)) == 0);
	}
	/* DEL */
	{
		const char b[] = { 'a', 127, 'b' };
		assert(jstr_io_isbinary_maybe(b, 3) == 1);
	}
	SUCCESS();
	return 0;
}
