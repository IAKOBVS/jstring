#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_UNALIGNED_ACCESS == 0) || (JSTR_HAVE_UNALIGNED_ACCESS == 1));
	return 0;
}
