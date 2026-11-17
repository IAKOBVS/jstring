#include <assert.h>
#include "jstr.h"

#include <stddef.h>

int
main(void)
{
	assert(JSTR_IO_ZB == (size_t)(JSTR_IO_EB * JSTR_IO_KB));
	return 0;
}
