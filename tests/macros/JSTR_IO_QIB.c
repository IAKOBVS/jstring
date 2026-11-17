#include <assert.h>
#include "jstr.h"

#include <stddef.h>

int
main(void)
{
	assert(JSTR_IO_QIB == (size_t)(JSTR_IO_RIB * JSTR_IO_KIB));
	return 0;
}
