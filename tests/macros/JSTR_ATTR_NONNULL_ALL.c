#include <assert.h>
#include "jstr.h"

JSTR_ATTR_NONNULL_ALL static int helper(const void *p) { return p != NULL; }

int
main(void)
{
	int x = 0;
	assert(helper(&x));
	return 0;
}
