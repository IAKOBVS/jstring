#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_FCVT == 0) || (JSTR_HAVE_FCVT == 1));
	return 0;
}
