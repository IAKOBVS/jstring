#include <assert.h>
#include "jstr.h"

JSTR_ATTR_FORMAT(printf, 1, 2) static int helper(const char *fmt, ...) { return fmt != NULL; }

int
main(void)
{
	assert(helper("x"));
	return 0;
}
