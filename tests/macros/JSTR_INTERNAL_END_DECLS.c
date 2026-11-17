#include <assert.h>
#include "jstr.h"

JSTR_INTERNAL_BEGIN_DECLS
int declared_inside;
JSTR_INTERNAL_END_DECLS

int
main(void)
{
	declared_inside = 1;
	assert(declared_inside == 1);
	return 0;
}
