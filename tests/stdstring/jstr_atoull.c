/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(int argc, char **argv)
{
	(void)argc; (void)argv;
	assert(jstr_atoull("42") == 42);
	assert(jstr_atoull("0") == 0);
	assert(jstr_atoull("abc") == 0);
	return 0;
}
