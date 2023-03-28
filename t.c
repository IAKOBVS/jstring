#include "jstr.h" /* .c */
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

/* ALWAYS_INLINE static int debug() */
/* { */
/* 	jstring_t s; */
/* 	jstr_init(&s); */
/* 	assert(jstr_new(&s, 1, "hll")); */
/* 	assert(jstr_push_back(&s, 3)); */
/* 	assert(jstr_reserve(&s, 100)); */
/* 	assert(jstr_shrink_to_fit(&s)); */
/* 	jstr_delete(&s); */
/* 	return 1; */
/* } */

int main()
{
	/* jstr_new(&s, "hello"); */
	jstring_t s;
	/* jstr_add(&s, 10, 9999); */
	jstr_add(&s, 1);
}
