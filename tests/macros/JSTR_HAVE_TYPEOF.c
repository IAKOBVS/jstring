#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_TYPEOF == 0) || (JSTR_HAVE_TYPEOF == 1));
	return 0;
}
