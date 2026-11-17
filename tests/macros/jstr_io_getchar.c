#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{
	assert(freopen("/dev/null", "r", stdin) != NULL);
	assert(jstr_io_getchar() == EOF);
	return 0;
}
