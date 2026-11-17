#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_STRNLEN == 0) || (JSTR_HAVE_STRNLEN == 1));
	return 0;
}
