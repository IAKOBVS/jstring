#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_READV == 0) || (JSTR_HAVE_READV == 1));
	return 0;
}
