#include <assert.h>
#include "jstr.h"

static int value;
JSTR_ATTR_RETURNS_NONNULL static int *helper(void) { return &value; }

int
main(void)
{
	assert(helper() == &value);
	return 0;
}
