#include <assert.h>
#include <string.h>
#include "stdstring.h"

int main(void) {
	/* jstr_strcasecmp_len: equal strings */
	{
		int r = jstr_strcasecmp_len("hello", "hello", 5);
		assert(r == 0);
	}
	/* jstr_strcasecmp_len: case-different */
	{
		int r = jstr_strcasecmp_len("Hello", "hElLo", 5);
		assert(r == 0);
	}
	/* jstr_strcasecmp_len: different */
	{
		int r = jstr_strcasecmp_len("abc", "xyz", 3);
		assert(r != 0);
	}
	/* jstr_strcasecmp_len: n == 0 */
	{
		int r = jstr_strcasecmp_len("abc", "xyz", 0);
		assert(r == 0);
	}
	/* jstr_strcasecmpeq_loop: equal */
	{
		int r = jstr_strcasecmpeq_loop("hello", "hello");
		assert(r == 0);
	}
	/* jstr_strcasecmpeq_loop: case-different */
	{
		int r = jstr_strcasecmpeq_loop("Hello", "hELLo");
		assert(r == 0);
	}
	/* jstr_strcasecmpeq_loop: different */
	{
		int r = jstr_strcasecmpeq_loop("abc", "xyz");
		assert(r != 0);
	}
	/* jstr_strcmpeq_loop: equal */
	{
		int r = jstr_strcmpeq_loop("hello", "hello");
		assert(r == 0);
	}
	/* jstr_strcmpeq_loop: different */
	{
		int r = jstr_strcmpeq_loop("abc", "xyz");
		assert(r != 0);
	}
	/* jstr_strzero */
	{
		char buf[] = "hello";
		jstr_strzero(buf);
		assert(buf[0] == '\0');
		assert(buf[1] == '\0');
		assert(buf[2] == '\0');
		assert(buf[3] == '\0');
		assert(buf[4] == '\0');
	}
	/* jstr_stpzero */
	{
		char buf[] = "abc";
		char *p = jstr_stpzero(buf);
		assert(buf[0] == '\0');
		assert(buf[1] == '\0');
		assert(buf[2] == '\0');
		assert(p == buf + 3);
	}
	return 0;
}
