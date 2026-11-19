/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	char a[] = "abc", b[] = "abc";
	assert(jstr_memcmpeq_loop(a, b, 3) == 0);
	char c[] = "abd";
	assert(jstr_memcmpeq_loop(a, c, 3) == 1);
	assert(jstr_memcmpeq_loop(a, b, 0) == 0);
	return 0;
}
