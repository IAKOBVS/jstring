#include <assert.h>
#include "jstr.h"

#include <stddef.h>

int
main(void)
{
	assert(JSTR_IO_GIB == (size_t)1073741824);
	return 0;
}
