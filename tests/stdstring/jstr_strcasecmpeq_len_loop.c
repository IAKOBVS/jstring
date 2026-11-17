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
	char a[]="abc", b[]="ABC";
	assert(jstr_strcasecmpeq_len_loop(a, b, 3) == 0);
	char c[]="abd";
	assert(jstr_strcasecmpeq_len_loop(a, c, 3) == 1);
	assert(jstr_strcasecmpeq_len_loop(a, b, 0) == 0);
	return 0;
}
