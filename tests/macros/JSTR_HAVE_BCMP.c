#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_BCMP == 0) || (JSTR_HAVE_BCMP == 1));
	return 0;
}
