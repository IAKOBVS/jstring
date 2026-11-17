#include <assert.h>
#include "jstr.h"

int
main(void)
{
	long x = JSTR_INTERNAL_CAST(long, 42);
	assert(x == 42);
	return 0;
}
