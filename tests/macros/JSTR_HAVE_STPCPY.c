#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_STPCPY == 0) || (JSTR_HAVE_STPCPY == 1));
	return 0;
}
