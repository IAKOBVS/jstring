#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_SNPRINTF_STRLEN == 0) || (JSTR_HAVE_SNPRINTF_STRLEN == 1));
	return 0;
}
