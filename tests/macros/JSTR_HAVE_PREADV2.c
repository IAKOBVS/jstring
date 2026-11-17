#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_PREADV2 == 0) || (JSTR_HAVE_PREADV2 == 1));
	return 0;
}
