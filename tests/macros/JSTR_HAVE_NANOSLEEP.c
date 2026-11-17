#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_NANOSLEEP == 0) || (JSTR_HAVE_NANOSLEEP == 1));
	return 0;
}
