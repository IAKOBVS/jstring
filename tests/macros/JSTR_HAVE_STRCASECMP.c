#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_STRCASECMP == 0) || (JSTR_HAVE_STRCASECMP == 1));
	return 0;
}
