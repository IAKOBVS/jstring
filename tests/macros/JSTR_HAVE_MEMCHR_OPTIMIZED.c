#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_MEMCHR_OPTIMIZED == 0) || (JSTR_HAVE_MEMCHR_OPTIMIZED == 1));
	return 0;
}
