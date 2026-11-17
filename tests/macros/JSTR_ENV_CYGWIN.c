#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_ENV_CYGWIN == 0) || (JSTR_ENV_CYGWIN == 1));
	return 0;
}
