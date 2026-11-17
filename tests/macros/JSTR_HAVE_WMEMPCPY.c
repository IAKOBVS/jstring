#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_WMEMPCPY == 0) || (JSTR_HAVE_WMEMPCPY == 1));
	return 0;
}
