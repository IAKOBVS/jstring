#include <assert.h>
#include "jstr.h"

#include <stddef.h>

int
main(void)
{
	assert(JSTR_IO_MIB == (size_t)1048576);
	return 0;
}
