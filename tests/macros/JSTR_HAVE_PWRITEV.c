#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_PWRITEV == 0) || (JSTR_HAVE_PWRITEV == 1));
	return 0;
}
