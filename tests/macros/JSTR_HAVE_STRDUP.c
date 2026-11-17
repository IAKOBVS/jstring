#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_STRDUP == 0) || (JSTR_HAVE_STRDUP == 1));
	return 0;
}
