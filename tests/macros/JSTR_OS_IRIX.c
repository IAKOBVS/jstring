#include <assert.h>
#include "jstr.h"

int
main(void)
{
#if defined sgi || defined __sgi
	assert(JSTR_OS_IRIX == 1);
#else
	assert(JSTR_OS_IRIX == 0);
#endif
	return 0;
}
