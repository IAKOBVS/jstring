#include "jstr.h" // .c
#include <assert.h>
#include <string.h>

ALWAYS_INLINE static int debug()
{
	jstring_t s;
	jstr_init(&s);
	assert(jstr_new(&s, 1, "hll"));
	assert(jstr_push_back(&s, 3));
	assert(jstr_reserve(&s, 100));
	assert(jstr_shrink_to_fit(&s));
	jstr_delete(&s);
	return 1;
}

int main()
{
	/* jstring_t s; */
	/* jstr_new(&s, "hello"); */
	void *p;
	jstr_new(p, "hell");
	return 0;
}
