#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined AMIGA || defined __amigaos__
	assert(JSTR_OS_AMIGA == 1);
#else
	assert(JSTR_OS_AMIGA == 0);
#endif
	return 0;
}
