#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_FCHDIR == 0) || (JSTR_HAVE_FCHDIR == 1));
	return 0;
}
