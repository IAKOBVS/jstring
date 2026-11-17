#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_STATIC_ASSERT == 0) || (JSTR_HAVE_STATIC_ASSERT == 1));
	return 0;
}
