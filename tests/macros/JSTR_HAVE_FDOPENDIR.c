#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_FDOPENDIR == 0) || (JSTR_HAVE_FDOPENDIR == 1));
	return 0;
}
