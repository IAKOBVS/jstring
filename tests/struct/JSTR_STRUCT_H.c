#include <assert.h>
#include "jstr.h"

#include <stddef.h>
#include <string.h>
#include "struct.h"
#include "struct.h" /* include-once guard must accept a second include */

static int
field_check(char **dp, size_t *sp, size_t *cp, jstr_ty *j)
{
	return dp == &j->data && sp == &j->size && cp == &j->capacity;
}

int
main(void)
{
	assert(JSTR_STRUCT_H == 1);

	/* Compile-time assertion and typecheck helpers.  They expand to nothing
	 * in plain C99 but must still parse cleanly; in C11 they are enforced. */
	JSTR_STATIC_ASSERT(1, "true");
	JSTR_STATIC_ASSERT(sizeof(jstr_ty) >= sizeof(char *) + 2 * sizeof(size_t),
	                   "jstr_ty too small");
	JSTR_STATIC_ASSERT_IS_SIZE((size_t)0);
	JSTR_STATIC_ASSERT_IS_STR((char *)0);
	JSTR_STATIC_ASSERT_IS_CHAR((char)0);
	JSTR_STATIC_ASSERT_TYPECHECK((size_t)0, (size_t)0);

	/* Core types are visible and have the expected shape. */
	{
		jstr_ty j = JSTR_INIT;
		assert(j.data == NULL && j.size == 0 && j.capacity == 0);

		jstr_literal_ty lit = jstr_literal_init("abc");
		assert(lit.size == 3);
		assert(memcmp(lit.data, "abc", 3) == 0);
		assert(sizeof lit.size == sizeof(unsigned int));

		jstr_cat_arg_ty ca;
		ca.ptr = "xyz";
		ca.size = 3;
		assert(ca.size == 3 && memcmp(ca.ptr, "xyz", 3) == 0);
	}

	/* jstr_struct expands to the three field addresses. */
	{
		jstr_ty j = JSTR_INIT;
		assert(field_check(jstr_struct(&j), &j));
		assert(offsetof(jstr_ty, data) == 0);
		assert(offsetof(jstr_ty, size) >= sizeof(char *));
		assert(offsetof(jstr_ty, capacity) > offsetof(jstr_ty, size));
	}

	/* Two-way state is large enough for the vendored automaton and exposes
	 * its needle-length accessors. */
	{
		assert(sizeof(jstr_twoway_ty) >= 4096);
		jstr_twoway_ty t;
		memset(&t, 0, sizeof t);
		jstr_twoway_set_len(&t, 0);
		assert(jstr_twoway_get_len(&t) == 0);
		jstr_twoway_set_len(&t, 12345);
		assert(jstr_twoway_get_len(&t) == 12345);
	}

	return 0;
}
