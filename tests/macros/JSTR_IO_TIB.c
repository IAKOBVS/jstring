#include <assert.h>
#include "jstr.h"

#include <stddef.h>

int
main(void)
{
	assert(JSTR_IO_TIB == (size_t)1099511627776);
	return 0;
}
