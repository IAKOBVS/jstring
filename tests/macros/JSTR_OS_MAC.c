#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined macintosh || defined Macintosh || (defined __APPLE__ && defined __MACH__)
	assert(JSTR_OS_MAC == 1);
#else
	assert(JSTR_OS_MAC == 0);
#endif
	return 0;
}
