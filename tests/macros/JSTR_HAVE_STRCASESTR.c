#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_STRCASESTR == 0) || (JSTR_HAVE_STRCASESTR == 1));
	return 0;
}
