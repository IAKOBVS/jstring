#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_ATFILE == 0) || (JSTR_HAVE_ATFILE == 1));
	return 0;
}
