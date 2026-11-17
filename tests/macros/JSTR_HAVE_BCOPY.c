#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_BCOPY == 0) || (JSTR_HAVE_BCOPY == 1));
	return 0;
}
