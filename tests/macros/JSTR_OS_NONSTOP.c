#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined __TANDEM
	assert(JSTR_OS_NONSTOP == 1);
#else
	assert(JSTR_OS_NONSTOP == 0);
#endif
	return 0;
}
