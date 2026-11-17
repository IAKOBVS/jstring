#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_MEMPCPY == 0) || (JSTR_HAVE_MEMPCPY == 1));
	return 0;
}
