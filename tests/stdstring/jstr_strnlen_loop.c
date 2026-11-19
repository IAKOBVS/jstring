/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	char buf[] = "hello";
	assert(jstr_strnlen_loop(buf, 10) == 5);
	assert(jstr_strnlen_loop(buf, 3) == 3);
	assert(jstr_strnlen_loop("", 5) == 0);
	return 0;
}
