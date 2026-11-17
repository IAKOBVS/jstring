#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_QGCVT == 0) || (JSTR_HAVE_QGCVT == 1));
	return 0;
}
