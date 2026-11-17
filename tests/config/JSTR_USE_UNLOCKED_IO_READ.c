#include <assert.h>
#include "jstr.h"

#include "config.h"

int
main(void)
{
	assert(JSTR_USE_UNLOCKED_IO_READ == 0 || JSTR_USE_UNLOCKED_IO_READ == 1);
	return 0;
}
