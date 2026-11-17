#include <assert.h>
#include "jstr.h"

#include <stddef.h>

int
main(void)
{
	assert(JSTR_IO_KIB == (size_t)1024);
	return 0;
}
