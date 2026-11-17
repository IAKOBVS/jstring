#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_DIRENT_D_RECLEN == 0) || (JSTR_HAVE_DIRENT_D_RECLEN == 1));
	return 0;
}
