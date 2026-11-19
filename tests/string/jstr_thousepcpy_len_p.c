/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	{
		char buf[32];
		char *end = jstr_thousepcpy_len_p(buf, "", 0, ',');
		assert(end == buf);
		assert(*buf == '\0');
	}
	{
		char buf[32];
		char *end = jstr_thousepcpy_len_p(buf, "-1234", 5, ',');
		assert(end == buf + 6);
		assert(!strcmp(buf, "-1,234"));
	}
	return 0;
}
