#define JSTR_DEBUG 1

#define JSTR_BUILT 1
#include "../include/jstr.h"

#define R restrict

int
main()
{
	jstr_ty s = JSTR_INIT;
	jstr_append_len(&s.data, &s.size, &s.capacity, jstr_literal("hello"));
	size_t i = jstr_count_len(s.data, s.size, "e", strlen("e"));
	printf("%zu\n", i);
	jstr_free_j(&s);
	return 0;
}
