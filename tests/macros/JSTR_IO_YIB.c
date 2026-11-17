#include <assert.h>
#include "jstr.h"

#include <stddef.h>

int
main(void)
{
	assert(JSTR_IO_YIB == (size_t)(JSTR_IO_ZIB * JSTR_IO_KIB));
	return 0;
}
