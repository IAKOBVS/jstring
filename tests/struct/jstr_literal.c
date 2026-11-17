#include <assert.h>
#include "jstr.h"

#include <string.h>
#include "struct.h"

static size_t
literal_len(const char *s, size_t n)
{
	(void)s;
	return n;
}

static const char *
literal_ptr(const char *s, size_t n)
{
	(void)n;
	return s;
}

int
main(void)
{
	assert(literal_len(jstr_literal("hello")) == 5);
	assert(memcmp(literal_ptr(jstr_literal("hello")), "hello", 5) == 0);
	assert(literal_len(jstr_literal("")) == 0);
	return 0;
}
