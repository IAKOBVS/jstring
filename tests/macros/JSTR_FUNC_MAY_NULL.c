#include <assert.h>
#include "jstr.h"

JSTR_FUNC_MAY_NULL static int helper(void) { return 7; }

int
main(void)
{
	assert(helper() == 7);
	return 0;
}
