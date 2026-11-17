#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_STRNDUPA == 0) || (JSTR_HAVE_STRNDUPA == 1));
	return 0;
}
