#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_CLEARERR_UNLOCKED == 0) || (JSTR_HAVE_CLEARERR_UNLOCKED == 1));
	return 0;
}
