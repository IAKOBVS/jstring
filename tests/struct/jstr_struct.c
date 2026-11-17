#include <assert.h>
#include "jstr.h"

#include <stddef.h>
#include <string.h>
#include "struct.h"

static int
struct_check(char **dp, size_t *sp, size_t *cp, jstr_ty *j)
{
	return dp == &j->data && sp == &j->size && cp == &j->capacity;
}

static void
struct_set(char **dp, size_t *sp, size_t *cp, char *d, size_t s, size_t c)
{
	*dp = d;
	*sp = s;
	*cp = c;
}

int
main(void)
{
	jstr_ty j = { 0 };
	j.data = (char *)"x";
	j.size = 1;
	j.capacity = 2;
	assert(struct_check(jstr_struct(&j), &j));

	/* The field addresses are writable through the expansion. */
	struct_set(jstr_struct(&j), (char *)"hello", 5, 8);
	assert(j.data == (char *)"hello");
	assert(j.size == 5);
	assert(j.capacity == 8);

	/* Two independent objects never alias. */
	{
		jstr_ty a = { 0 };
		jstr_ty b = { 0 };
		assert(struct_check(jstr_struct(&a), &a));
		assert(struct_check(jstr_struct(&b), &b));
		a.data = (char *)"a";
		b.data = (char *)"b";
		assert(a.data[0] == 'a');
		assert(b.data[0] == 'b');
		assert(a.data != b.data);
		assert(&a.data != &b.data);
	}

	/* Individual fields retain their values and widths. */
	{
		jstr_ty s = { 0 };
		s.data = (char *)"abc";
		s.size = 3;
		s.capacity = 16;
		assert(s.size == 3);
		assert(s.capacity == 16);
		assert(s.size <= s.capacity);
		assert(sizeof s.size == sizeof(size_t));
		assert(sizeof s.capacity == sizeof(size_t));
	}

	/* JSTR_INIT zero-initialises every field. */
	{
		jstr_ty z = JSTR_INIT;
		assert(z.data == NULL);
		assert(z.size == 0);
		assert(z.capacity == 0);
	}

	return 0;
}
