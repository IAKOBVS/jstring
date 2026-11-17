/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
{
		size_t n = jstr_startscasenul("Hello World", "hello");
		assert(n == 5);
		n = jstr_startscasenul("Hello World", "world");
		assert(n == 0);
		n = jstr_startscasenul("", "");
		assert(n == 0);
	}
	return 0;
}
