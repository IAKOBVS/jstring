#include <assert.h>
#include "jstr.h"

#include <stdint.h>
#include "pointer-arith.h"

int
main(void)
{
	assert(JSTR_ALIGN_UP(100, 8) == 104);
	assert(JSTR_ALIGN_UP(0, 8) == 0);
	assert(JSTR_ALIGN_UP(8, 8) == 8);
	return 0;
}
