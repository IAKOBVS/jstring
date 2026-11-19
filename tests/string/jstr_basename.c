/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	char *p = jstr_basename("/usr/local/bin");
	assert(p && !strcmp(p, "bin"));
	p = jstr_basename("foo");
	assert(p && !strcmp(p, "foo"));
	p = jstr_basename("");
	assert(p && !strcmp(p, ""));
	return 0;
}
