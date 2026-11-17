#include <assert.h>
#include "jstr.h"

JSTR_ATTR_SENTINEL static int helper(const char *s, ...) { return s != NULL; }

int
main(void)
{
	assert(helper("x", NULL));
	return 0;
}
