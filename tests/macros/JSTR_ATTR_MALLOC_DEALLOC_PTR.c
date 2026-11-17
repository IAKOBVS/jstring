#include <assert.h>
#include "jstr.h"

#include <stdlib.h>
JSTR_ATTR_MALLOC_DEALLOC_PTR(free, 1) static void *helper(void) { return malloc(1); }

int
main(void)
{
	void *p = helper();
	assert(p != NULL);
	free(p);
	return 0;
}
