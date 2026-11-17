#include <assert.h>
#include "jstr.h"

JSTR_ATTR_ACCESS((__read_only__, 1, 2)) static int helper(const char *s, size_t n) { return s != NULL && n == 1; }

int
main(void)
{
	assert(helper("x", 1));
	return 0;
}
