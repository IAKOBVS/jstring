#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{
	assert(freopen("/dev/null", "w", stdout) != NULL);
	assert(jstr_io_putchar('A') == 'A');
	return 0;
}
