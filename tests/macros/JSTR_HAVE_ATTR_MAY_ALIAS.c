#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_ATTR_MAY_ALIAS == 0) || (JSTR_HAVE_ATTR_MAY_ALIAS == 1));
	return 0;
}
