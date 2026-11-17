#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_PCLOSE == 0) || (JSTR_HAVE_PCLOSE == 1));
	return 0;
}
