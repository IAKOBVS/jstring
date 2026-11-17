#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_BZERO == 0) || (JSTR_HAVE_BZERO == 1));
	return 0;
}
