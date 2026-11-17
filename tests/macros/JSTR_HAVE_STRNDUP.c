#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_STRNDUP == 0) || (JSTR_HAVE_STRNDUP == 1));
	return 0;
}
