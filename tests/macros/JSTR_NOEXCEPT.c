#include <assert.h>
#include "jstr.h"

static int
seven(void) JSTR_NOEXCEPT
{
	return 7;
}

int
main(void)
{
	assert(seven() == 7);
	return 0;
}
