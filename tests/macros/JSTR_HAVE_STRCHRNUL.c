#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_STRCHRNUL == 0) || (JSTR_HAVE_STRCHRNUL == 1));
	return 0;
}
