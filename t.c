#include "jstr.h" // .c
#include "macros.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

ALWAYS_INLINE static int debug()
{
	jstring_t s;
	jstr_init(&s);
		assert(!s.size);
		assert(!s.capacity);
		assert(!s.data);
	size_t old_size = s.size;
	assert(jstr_new(&s, "hello"));
		assert(!strcmp(s.data, "hello"));
		assert(s.size == strlen("hello"));
	old_size = s.size;
	assert(jstr_push_back(&s, 'e'));
		assert(!strcmp(s.data, "helloe"));
		assert(s.size == old_size + 1);
	assert(jstr_reserve(&s, 100));
		assert(s.capacity == JSTR_NEXT_POW2(100));
	assert(jstr_shrink_to_fit(&s));
		assert(s.capacity == s.size + 1);
	jstr_delete(&s);
		assert(!s.size);
		assert(!s.capacity);
		assert(!s.data);
	return 1;
}

int main()
{
	assert(debug());
}
