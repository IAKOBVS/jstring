#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_QFCVT == 0) || (JSTR_HAVE_QFCVT == 1));
	return 0;
}
