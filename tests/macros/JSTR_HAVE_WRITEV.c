#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_WRITEV == 0) || (JSTR_HAVE_WRITEV == 1));
	return 0;
}
