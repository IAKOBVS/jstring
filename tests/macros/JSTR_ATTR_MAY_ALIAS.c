#include <assert.h>
#include "jstr.h"

typedef int JSTR_ATTR_MAY_ALIAS aliased_int;
static int helper(void) { aliased_int x = 7; return x; }

int
main(void)
{
	assert(helper() == 7);
	return 0;
}
