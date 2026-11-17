#include <assert.h>
#include "jstr.h"

#include <stddef.h>

int
main(void)
{
	assert(JSTR_IO_YB == (size_t)(JSTR_IO_ZB * JSTR_IO_KB));
	return 0;
}
