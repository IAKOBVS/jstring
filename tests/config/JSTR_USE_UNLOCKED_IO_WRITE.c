#include <assert.h>
#include "jstr.h"

#include "config.h"

int
main(void)
{
	assert(JSTR_USE_UNLOCKED_IO_WRITE == 0 || JSTR_USE_UNLOCKED_IO_WRITE == 1);
	return 0;
}
