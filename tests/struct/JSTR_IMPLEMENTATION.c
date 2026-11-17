#include <assert.h>
#include "jstr.h"

#include <stddef.h>
#include <string.h>
#include "struct.h"
#include "struct.h" /* including struct.h again must be a no-op */

/* If the implementation macro is present after the headers, it must carry the
 * documented value 1. */
#ifdef JSTR_IMPLEMENTATION
#	if JSTR_IMPLEMENTATION != 1
#		error "JSTR_IMPLEMENTATION must be 1 when defined"
#	endif
#endif

int
main(void)
{
	/* The include-once guard is honoured after a second include. */
	assert(JSTR_STRUCT_H == 1);

	/* Public structs remain usable exactly once. */
	jstr_ty j = JSTR_INIT;
	jstr_literal_ty lit = jstr_literal_init("impl");
	jstr_cat_arg_ty ca = { "ab", 2 };
	jstr_twoway_ty tw;

	assert(j.data == NULL && j.size == 0 && j.capacity == 0);
	assert(lit.size == 4);
	assert(memcmp(lit.data, "impl", 4) == 0);
	assert(ca.size == 2 && memcmp(ca.ptr, "ab", 2) == 0);
	assert(sizeof tw >= 4096);

	jstr_twoway_set_len(&tw, 7);
	assert(jstr_twoway_get_len(&tw) == 7);

#ifdef JSTR_IMPLEMENTATION
	assert(JSTR_IMPLEMENTATION == 1);
#endif

	return 0;
}
