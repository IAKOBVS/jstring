#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_STRNCASECMP == 0) || (JSTR_HAVE_STRNCASECMP == 1));
	return 0;
}
