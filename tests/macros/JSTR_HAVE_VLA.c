#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_VLA == 0) || (JSTR_HAVE_VLA == 1));
	return 0;
}
