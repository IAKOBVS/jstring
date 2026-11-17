#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_PREADV == 0) || (JSTR_HAVE_PREADV == 1));
	return 0;
}
