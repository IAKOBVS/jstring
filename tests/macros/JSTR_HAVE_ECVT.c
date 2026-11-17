#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_ECVT == 0) || (JSTR_HAVE_ECVT == 1));
	return 0;
}
