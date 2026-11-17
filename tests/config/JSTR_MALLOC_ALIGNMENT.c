#include <assert.h>
#include "jstr.h"

#include "config.h"

int
main(void)
{
	assert(JSTR_MALLOC_ALIGNMENT == sizeof(size_t) * 2);
	assert((JSTR_MALLOC_ALIGNMENT & (JSTR_MALLOC_ALIGNMENT - 1)) == 0);
	return 0;
}
