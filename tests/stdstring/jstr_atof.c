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
	assert(jstr_atof("2.5f") == 2.5f);
	assert(jstr_atof("0") == 0.0f);
	assert(jstr_atof("abc") == 0.0f);
	return 0;
}
