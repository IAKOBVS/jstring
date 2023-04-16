#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "/home/james/c/jstring/jstr.h"
#include <stdlib.h>
#include <string.h>

#define deb_ static int

#define s_ jstring_t *

deb_ d_jstr_init(s_ s)
{
	jstr_init(s);
	assert(!s->size);
	assert(!s->capacity);
	assert(!s->data);
	return 1;
}

deb_ d_jstr_dealloc(s_ s)
{
	jstr_dealloc(s);
	assert(!s->size);
	assert(!s->capacity);
	assert(!s->data);
	return 1;
}

const char *s_1 = "sample string";

deb_ d_jstr_alloc(s_ s)
{
	jstr_alloc(s, s_1);
	assert(!strcmp(s->data, s_1));
	assert(s->size == strlen(s_1));
	return 1;
}

const char c_1 = 'e';

const char *s_1_c_1 = "sample stringe";

deb_ d_jstr_push_back(s_ s)
{
	const size_t old_size = s->size;
	jstr_push_back(s, 'e');
	assert(!strcmp(s->data, s_1_c_1));
	assert(s->size == old_size + 1);
	return 1;
}

deb_ d_jstr_reserve(s_ s)
{
	jstr_reserve(s, 100);
	assert(s->capacity == JSTR_NEXT_POW2(100));
	return 1;
}

deb_ d_jstr_shrink_to_fit(s_ s)
{
	jstr_shrink_to_fit(s);
	assert(s->capacity == s->size + 1);
	return 1;
}

const char *s_2 = "hello world";

deb_ d_jstr_assign(s_ s)
{
	jstr_assign(s, s_2);
	assert(!strcmp(s->data, s_2));
	assert(s->size == strlen(s_2));
	return 1;
}

int debug()
{
	jstring_t s;
	jstr_alloc(&s, 100);
	assert(d_jstr_init(&s));
	assert(d_jstr_alloc(&s));
	assert(d_jstr_push_back(&s));
	assert(d_jstr_reserve(&s));
	assert(d_jstr_shrink_to_fit(&s));
	assert(d_jstr_assign(&s));
	assert(puts(s.data));
	assert(d_jstr_dealloc(&s));
	return 1;
}

int main()
{
	jstring_t s;
}
