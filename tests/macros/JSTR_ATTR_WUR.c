#include <assert.h>
#include "jstr.h"

JSTR_ATTR_WUR static int helper(void) { return 7; }

int
main(void)
{
	assert(helper() == 7);
	return 0;
}
