#include "jstr.h" /* .c */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/cdefs.h>
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

#define ADD(a, b) __attribute__((warn_unused_result)) (a + b)


int main()
{
}
