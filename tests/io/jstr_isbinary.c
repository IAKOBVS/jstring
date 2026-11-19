#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <string.h>

int
main(void)
{
	assert(jstr_isbinary(NULL, 0, 0) == 0);
	assert(jstr_isbinary(jstr_literal(""), 5) == 0);
	{
		const char data[] = { 'a', 'b', '\0', 'c', 'd' };
		/* control? NUL is < ' ' but equals? NUL < ' ' and not tab/newline -> binary */
		assert(jstr_isbinary(data, 5, 5) == 1);
	}
	{
		const char printable[] = "hello\tworld\n";
		assert(jstr_isbinary(printable, 12, 12) == 0);
	}
	{
		const char b[] = { 'a', 127, 'b' };
		assert(jstr_isbinary(b, 3, 3) == 1);
	}
	/* n clamped to sz */
	{
		const char b[] = { 'a', 1, 'b' };
		assert(jstr_isbinary(b, 3, 1) == 0);
		assert(jstr_isbinary(b, 3, 2) == 1);
	}
	return 0;
}
