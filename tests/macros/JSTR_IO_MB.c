#include <assert.h>
#include "jstr.h"

#include <stddef.h>

int
main(void)
{
	assert(JSTR_IO_MB == (size_t)1000000);
	return 0;
}
