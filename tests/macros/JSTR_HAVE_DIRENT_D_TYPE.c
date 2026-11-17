#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_DIRENT_D_TYPE == 0) || (JSTR_HAVE_DIRENT_D_TYPE == 1));
	return 0;
}
