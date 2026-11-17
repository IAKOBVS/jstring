#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_BUILTIN_MEMCMP == 0) || (JSTR_HAVE_BUILTIN_MEMCMP == 1));
	return 0;
}
