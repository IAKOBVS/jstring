#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_DIRENT_D_NAMLEN == 0) || (JSTR_HAVE_DIRENT_D_NAMLEN == 1));
	return 0;
}
