#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_PWRITEV2 == 0) || (JSTR_HAVE_PWRITEV2 == 1));
	return 0;
}
