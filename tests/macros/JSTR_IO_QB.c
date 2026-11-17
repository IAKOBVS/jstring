#include <assert.h>
#include "jstr.h"

#include <stddef.h>

int
main(void)
{
	assert(JSTR_IO_QB == (size_t)(JSTR_IO_RB * JSTR_IO_KB));
	return 0;
}
