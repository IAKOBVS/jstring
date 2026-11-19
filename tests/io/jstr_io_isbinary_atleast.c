#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <string.h>

int
main(void)
{
	assert(jstr_io_isbinary_atleast(jstr_literal(""), 5) == 0);
	{
		const char *s = "hello world";
		size_t n = strlen(s);
		assert(jstr_io_isbinary_atleast(s, n, n) == 0);
		assert(jstr_io_isbinary_atleast(s, n, 4096) == 0);
	}
	{
		const char data[] = { 'a', 'b', '\0', 'c', 'd' };
		assert(jstr_io_isbinary_atleast(data, 5, 5) == 1);
		assert(jstr_io_isbinary_atleast(data, 5, 3) == 1);
	}
	{
		const char late[] = { 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', '\0', 'b', 'b', 'b' };
		assert(jstr_io_isbinary_atleast(late, 12, 8) == 0);
		assert(jstr_io_isbinary_atleast(late, 12, 9) == 1);
	}
	{
		const char data[] = { 'a', 'b', '\0', 'c', 'd' };
		assert(jstr_io_isbinary_atleast(data, 5, 1024) == 1);
	}
	{
		const char nul0[] = { '\0', 'a', 'b' };
		assert(jstr_io_isbinary_atleast(nul0, 3, 1) == 1);
	}
	assert(jstr_io_isbinary_atleast(jstr_literal(""), 0) == 0);
	/* n == 0 skips NUL scan but signature may apply */
	{
		const char *s = "hello";
		assert(jstr_io_isbinary_atleast(s, 5, 0) == 0);
	}
	return 0;
}
