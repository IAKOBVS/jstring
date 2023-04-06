#include "jstr.h" // .c
#include "macros.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

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

deb_ d_jstr_delete(s_ s)
{
	jstr_delete(s);
	assert(!s->size);
	assert(!s->capacity);
	assert(!s->data);
	return 1;
}

const char *s_1 = "sample string";

deb_ d_jstr_new(s_ s)
{
	assert(jstr_new(s, s_1));
	assert(!strcmp(s->data, s_1));
	assert(s->size == strlen(s_1));
	return 1;
}

const char c_1 = 'e';

const char *s_1_c_1 = "sample stringe";

deb_ d_jstr_push_back(s_ s)
{
	const size_t old_size = s->size;
	assert(jstr_push_back(s, 'e'));
	assert(!strcmp(s->data, s_1_c_1));
	assert(s->size == old_size + 1);
	return 1;
}

deb_ d_jstr_reserve(s_ s)
{
	assert(jstr_reserve(s, 100));
	assert(s->capacity == JSTR_NEXT_POW2(100));
	return 1;
}

deb_ d_jstr_shrink_to_fit(s_ s)
{
	assert(jstr_shrink_to_fit(s));
	assert(s->capacity == s->size + 1);
	return 1;
}

const char *s_2 = "hello world";

deb_ d_jstr_assign(s_ s)
{
	assert(jstr_assign(s, s_2));
	assert(!strcmp(s->data, s_2));
	assert(s->size == strlen(s_2));
	return 1;
}

int debug()
{
	jstring_t s;
	assert(d_jstr_init(&s));
	assert(d_jstr_new(&s));
	assert(d_jstr_push_back(&s));
	assert(d_jstr_reserve(&s));
	assert(d_jstr_shrink_to_fit(&s));
	assert(d_jstr_assign(&s));
	assert(puts(s.data));
	assert(d_jstr_delete(&s));
	return 1;
}

int main()
{
	assert(debug());
}
