#include <assert.h>
#include "jstr.h"

static JSTR_INTERNAL_ATTR_INLINE int
seven(void)
{
	return 7;
}

int
main(void)
{
	assert(seven() == 7);
	return 0;
}
