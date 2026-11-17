#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_ENV_INTERIX == 0) || (JSTR_ENV_INTERIX == 1));
	return 0;
}
