#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_QECVT_R == 0) || (JSTR_HAVE_QECVT_R == 1));
	return 0;
}
