#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_VASPRINTF == 0) || (JSTR_HAVE_VASPRINTF == 1));
	return 0;
}
