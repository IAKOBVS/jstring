#include <assert.h>
#include "jstr.h"

#include <stddef.h>

int
main(void)
{
	assert(JSTR_IO_EIB == (size_t)1152921504606846976);
	return 0;
}
