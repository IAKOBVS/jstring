#include <assert.h>
#include "jstr.h"

static int value;
JSTR_FUNC_RET_NONNULL static int *helper(void) { return &value; }

int
main(void)
{
	assert(helper() == &value);
	return 0;
}
