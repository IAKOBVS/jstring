#include <assert.h>
#include <string.h>
#include "jstr.h"

int main(void) {
	/* jstr_debug with zero-initialized jstr_ty */
	{
		jstr_ty j = JSTR_INIT;
		jstr_ret_ty ret = jstr_debug(&j);
		assert(ret == JSTR_RET_SUCC);
	}
	/* jstr_debug with data */
	{
		jstr_ty j = JSTR_INIT;
		jstr_ret_ty ret = jstr_assign_len(&j.data, &j.size, &j.capacity, "hello", 5);
		assert(ret == JSTR_RET_SUCC);
		ret = jstr_debug(&j);
		assert(ret == JSTR_RET_SUCC);
		jstr_free(&j.data, &j.size, &j.capacity);
	}
	return 0;
}
