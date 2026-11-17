#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_QECVT == 0) || (JSTR_HAVE_QECVT == 1));
	return 0;
}
