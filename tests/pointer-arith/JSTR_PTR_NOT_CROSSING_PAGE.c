#include <assert.h>
#include "jstr.h"

#include <stdint.h>
#include "pointer-arith.h"

int
main(void)
{
	assert(JSTR_PTR_NOT_CROSSING_PAGE((uintptr_t)0xf9c, 100, JSTR_PAGE_SIZE));
	assert(!JSTR_PTR_NOT_CROSSING_PAGE((uintptr_t)0xf9c, 101, JSTR_PAGE_SIZE));
	assert(!JSTR_PTR_NOT_CROSSING_PAGE((uintptr_t)0x1000, 1, JSTR_PAGE_SIZE));
	assert(JSTR_PTR_NOT_CROSSING_PAGE((uintptr_t)0x1000, 0, JSTR_PAGE_SIZE));
	return 0;
}
