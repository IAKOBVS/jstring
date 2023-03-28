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

#		define JSTR_GENERIC_CASE_SIZE1(...)            \
			int: __VA_ARGS__,                      \
			unsigned int: __VA_ARGS__,             \
			size_t: __VA_ARGS__,                   \
			long: __VA_ARGS__,                     \
			long long: __VA_ARGS__,                \
			unsigned long long: __VA_ARGS__,       \
						               \
			const int: __VA_ARGS__,                \
			const unsigned int: __VA_ARGS__,       \
			const size_t: __VA_ARGS__,             \
			const long: __VA_ARGS__,               \
			const long long: __VA_ARGS__,          \
			const unsigned long long: __VA_ARGS__

#		define JSTR_GENERIC_CASE_SIZE2(expr)            \
			int: expr,                     \
			unsigned int: expr,            \
			size_t: expr,                  \
			long: expr,                    \
			long long: expr,               \
			unsigned long long: expr,      \
						              \
			const int: expr,               \
			const unsigned int: expr,      \
			const size_t: expr,            \
			const long: expr,              \
			const long long: expr,         \
			const unsigned long long: expr


int main()
{
	/* jstr_new(&s, "hello"); */
	jstring_t s;
	/* jstr_add(&s, 10, 9999); */
	JSTR_GENERIC_CASE_SIZE1(jstr_append(1, 3));
}
