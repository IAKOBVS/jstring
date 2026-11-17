#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_MEMRCHR == 0) || (JSTR_HAVE_MEMRCHR == 1));
	return 0;
}
