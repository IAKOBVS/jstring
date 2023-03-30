#ifndef JSTR_H_DEF__
#define JSTR_H_DEF__

#define JSTR_ALIGN_POWER_OF_TWO
#define JSTR_64_BIT

#ifdef __GNUC__
#	define _GNU_SOURCE
#endif

#include <sys/cdefs.h>
#include <stdlib.h>
#include <features.h>
#include "macros.h"
#include "/home/james/c/pp_macros/pp_va_args_macros.h"

typedef struct jstring_t {
	size_t size; 
	size_t capacity;
	char *data;
} jstring_t;

int private_jstr_cat(jstring_t *RESTRICT this_, const size_t len, ...) JSTR_WARN_UNUSED;
int private_jstr_cat_s(jstring_t *RESTRICT this_, const size_t len, ...) JSTR_WARN_UNUSED;

void jstr_init(jstring_t *RESTRICT this_);
void jstr_delete(jstring_t *RESTRICT this_);

int jstr_new_append(jstring_t *RESTRICT this_, const size_t srclen, const char *RESTRICT const src_, ...) JSTR_WARN_UNUSED;
int jstr_new_alloc(jstring_t *RESTRICT this_, const size_t size) JSTR_WARN_UNUSED;

int private_jstr_new_cat(jstring_t *RESTRICT this_, const size_t arglen, ...) JSTR_WARN_UNUSED;

void jstr_push_back_noalloc(jstring_t *this_, const char c);
int jstr_push_back_nocheck(jstring_t *this_, const char c) JSTR_WARN_UNUSED;
int jstr_push_back(jstring_t *this_, const char c) JSTR_WARN_UNUSED;
int jstr_push_back_s(jstring_t *this_, const char c) JSTR_WARN_UNUSED;

void jstr_push_front_noalloc(jstring_t *RESTRICT this_, const char c);
int jstr_push_front_nocheck(jstring_t *RESTRICT this_, const char c) JSTR_WARN_UNUSED;
int jstr_push_front(jstring_t *RESTRICT this_, const char c) JSTR_WARN_UNUSED;
int jstr_push_front_s(jstring_t *RESTRICT this_, const char c) JSTR_WARN_UNUSED;

void jstr_pop_back(jstring_t *RESTRICT this_);
void jstr_pop_back_s(jstring_t *RESTRICT this_);

void jstr_pop_front(jstring_t *RESTRICT this_);
void jstr_pop_front_s(jstring_t *RESTRICT this_);

void private_jstr_append_noalloc(jstring_t *dest, const char *RESTRICT src, const size_t srclen, ...);
int private_jstr_append(jstring_t *dest, const char *RESTRICT src, const size_t srclen, ...) JSTR_WARN_UNUSED;
int private_jstr_append_s(jstring_t *dest, const char *RESTRICT src, const size_t srclen, ...) JSTR_WARN_UNUSED;

void jstr_swap(jstring_t *RESTRICT this_, jstring_t *RESTRICT other_);
void jstr_swap_str(jstring_t *RESTRICT this_, char **RESTRICT other_, size_t *otherlen, size_t *other_cap);

int jstr_shrink_to_fit_nocheck(jstring_t *RESTRICT this_) JSTR_WARN_UNUSED;
int jstr_shrink_to_fit(jstring_t *RESTRICT this_) JSTR_WARN_UNUSED;

int jstr_shrink_to_nocheck(jstring_t *RESTRICT this_, const size_t cap) JSTR_WARN_UNUSED;
int jstr_shrink_to(jstring_t *RESTRICT this_, const size_t cap) JSTR_WARN_UNUSED;

void jstr_shrink_to_size_nocheck(jstring_t *RESTRICT this_, const size_t size);
void jstr_shrink_to_size(jstring_t *RESTRICT this_, const size_t size);

int jstr_reserve(jstring_t *RESTRICT this_, const size_t cap) JSTR_WARN_UNUSED;
int jstr_reserve_s(jstring_t *RESTRICT this_, const size_t cap) JSTR_WARN_UNUSED;
int jstr_reserve_nocheck(jstring_t *RESTRICT this_, const size_t cap, ...) JSTR_WARN_UNUSED;

int private_jstr_replace(jstring_t *RESTRICT dest, char *RESTRICT src, const size_t srclen, ...) JSTR_WARN_UNUSED;
void jstr_replace_noalloc(jstring_t *RESTRICT dest, char *RESTRICT src, const size_t srclen);
int jstr_replace_nocheck(jstring_t *RESTRICT dest, char *RESTRICT src, const size_t srclen) JSTR_WARN_UNUSED;

void jstr_replace_jstr_noalloc(jstring_t *RESTRICT dest, jstring_t *RESTRICT src);
int jstr_replace_jstr_nocheck(jstring_t *RESTRICT dest, jstring_t *RESTRICT src) JSTR_WARN_UNUSED;
int jstr_replace_jstr(jstring_t *RESTRICT dest, jstring_t *RESTRICT src, ...) JSTR_WARN_UNUSED;

/* if equals returns 0 */
int jstr_cmp(jstring_t *RESTRICT this_, jstring_t *RESTRICT other_) JSTR_WARN_UNUSED;

#ifdef __USE_GNU
int jstr_case_cmp_nocheck(jstring_t *RESTRICT this_, jstring_t *RESTRICT other) JSTR_WARN_UNUSED;
int jstr_case_cmp(jstring_t *RESTRICT this_, jstring_t *RESTRICT other) JSTR_WARN_UNUSED;
#endif

/* finds first occurence of character from end of string */
char *jstr_rchr(jstring_t *RESTRICT this_, int c) JSTR_WARN_UNUSED;
/* memchr */
char *jstr_chr(jstring_t *RESTRICT this_, int c) JSTR_WARN_UNUSED;

#ifdef __USE_GNU
char *private_jstr_str(jstring_t *haystack, const char *RESTRICT const needle, size_t needlelen, ...) JSTR_WARN_UNUSED;

#	define jstr_str(this_, ...)                                                                        \
		(PP_NARG(__VA_ARGS__) == 1)                                                                \
			? private_jstr_str(this_, PP_FIRST_ARG(__VA_ARGS__), strlen(PP_NARG(__VA_ARGS__))) \
			: private_jstr_str(this_, __VA_ARGS__, 0)

#else

char *jstr_str(jstring_t *haystack, const char *RESTRICT needle) JSTR_WARN_UNUSED;

#endif

int jstr_dup(jstring_t *RESTRICT this_, jstring_t *RESTRICT other) JSTR_WARN_UNUSED;
int jstr_dup_s(jstring_t *RESTRICT this_, jstring_t *RESTRICT other) JSTR_WARN_UNUSED;

int jstr_ndup(jstring_t *RESTRICT this_, jstring_t *RESTRICT other, const size_t n) JSTR_WARN_UNUSED;
int jstr_ndup_s(jstring_t *RESTRICT this_, jstring_t *RESTRICT other, const size_t n) JSTR_WARN_UNUSED;

void jstr_rev(jstring_t *RESTRICT this_);

int jstr_rev_dup(jstring_t *RESTRICT this_) JSTR_WARN_UNUSED;
/* asserts that string fits in a stack array */
void jstr_rev_dupa(jstring_t *RESTRICT this_);
int jstr_rev_dup_s(jstring_t *RESTRICT this_) JSTR_WARN_UNUSED;

void jstr_rev_noalloc(jstring_t *RESTRICT this_, char *buf);

#ifdef JSTR_HAS_GENERIC
#	define jstr_replace(dest, ...) _Generic((PP_FIRST_ARG(__VA_ARGS__)),                                                       \
		jstring_t *: jstr_replace_jstr(dest, (jstring_t *)PP_FIRST_ARG(__VA_ARGS__)),                                      \
		char *: (PP_NARG(__VA_ARGS__) == 2)                                                                                \
			? private_jstr_replace(dest, (char *)__VA_ARGS__, 0)                                                       \
			: private_jstr_replace(dest, (char *)PP_FIRST_ARG(__VA_ARGS__), strlen((char *)PP_FIRST_ARG(__VA_ARGS__))) \
	)
#else
#	define jstr_replace(dest, src, srclen)          \
		private_jstr_replace(dest, src, srclen)
#endif // JSTR_HAS_GENERIC

#ifdef JSTR_HAS_GENERIC
#	define jstr_cat(this_jstr, ...)                                                          \
		generic_jstr_cat(this_jstr, PP_STRLEN_VA_ARGS(__VA_ARGS__), __VA_ARGS__, NULL)
#	define generic_jstr_cat(this_jstr, len, arg1, ...) _Generic((PP_FIRST_ARG(__VA_ARGS__)), \
		void *: jstr_append(this_jstr, arg1, len),                                       \
		char *: private_jstr_cat(this_jstr, len, arg1, __VA_ARGS__)                      \
	)
#	define jstr_cat_s(this_jstr, ...)                                                          \
		generic_jstr_cat_s(this_jstr, PP_STRLEN_VA_ARGS(__VA_ARGS__), __VA_ARGS__, NULL)
#	define generic_jstr_cat_s(this_jstr, len, arg1, ...) _Generic((PP_FIRST_ARG(__VA_ARGS__)), \
		void *: jstr_append_s(this_jstr, arg1, len),                                       \
		char *: private_jstr_cat_s(this_jstr, len, arg1, __VA_ARGS__)                      \
	)
#else
#	define jstr_cat(this_jstr, ...)                                                          \
		private_jstr_cat(this_jstr, PP_STRLEN_VA_ARGS(__VA_ARGS__), __VA_ARGS__, NULL)
#	define jstr_cat_s(this_jstr, ...)                                                          \
		private_jstr_cat_s(this_jstr, PP_STRLEN_VA_ARGS(__VA_ARGS__), __VA_ARGS__, NULL)
#endif // JSTR_HAS_GENERIC

#define jstr_new_cat(this_jstr, ...) private_jstr_new_cat(this_jstr, PP_STRLEN_VA_ARGS(__VA_ARGS__), __VA_ARGS__, NULL)

#define PRIVATE_JSTR_NEW_FIRST_INT(this_jstr, ...)                         \
	(PP_NARG(__VA_ARGS__) == 1)                                        \
		? PRIVATE_JSTR_NEW_ALLOC(this_jstr, __VA_ARGS__)           \
		: PRIVATE_JSTR_NEW_APPEND_WITH_LEN(this_jstr, __VA_ARGS__)

#define PRIVATE_JSTR_NEW_ALLOC(this_jstr, ...)                       \
	jstr_new_alloc(this_jstr, (size_t)PP_FIRST_ARG(__VA_ARGS__))

#define PRIVATE_JSTR_NEW_APPEND(this_jstr, ...)                                                                  \
	jstr_new_append(this_jstr, strlen((char *)PP_FIRST_ARG(__VA_ARGS__)), (char *)PP_FIRST_ARG(__VA_ARGS__))

#define PRIVATE_JSTR_NEW_APPEND_WITH_LEN(this_jstr, ...)      \
	jstr_new_append(this_jstr, (size_t)__VA_ARGS__, NULL)

#define PRIVATE_JSTR_NEW_CAT(this_jstr, ...)                                                       \
	private_jstr_new_cat(this_jstr, PP_STRLEN_VA_ARGS((char *)__VA_ARGS__), __VA_ARGS__, NULL)

#define PRIVATE_JSTR_NEW_ADD_STR(this_jstr, ...)                  \
(                                                                 \
	(PP_NARG(__VA_ARGS__) == 1)                               \
		? PRIVATE_JSTR_NEW_APPEND(this_jstr, __VA_ARGS__) \
		: PRIVATE_JSTR_NEW_CAT(this_jstr, __VA_ARGS__)    \
)

#ifdef JSTR_HAS_GENERIC
#	define jstr_new(this_jstr, ...) _Generic((PP_FIRST_ARG(__VA_ARGS__)),               \
		JSTR_GENERIC_CASE_SIZE(PRIVATE_JSTR_NEW_FIRST_INT(this_jstr, __VA_ARGS__)), \
		JSTR_GENERIC_CASE_STR(PRIVATE_JSTR_NEW_ADD_STR(this_jstr, __VA_ARGS__))     \
	)
#else
#	define jstr_new(this_jstr, size) jstr_new_alloc(this_jstr, size)
#endif // JSTR_HAS_GENERIC

#ifdef JSTR_HAS_GENERIC
#	define jstr_add(this_jstr, ...) _Generic((PP_FIRST_ARG(__VA_ARGS__)),                                                         \
		JSTR_GENERIC_CASE_SIZE(jstr_reserve_nocheck(this_jstr, ((this_jstr)->capacity) + (size_t)PP_FIRST_ARG(__VA_ARGS__))), \
		JSTR_GENERIC_CASE_STR(jstr_append(this_jstr, (char *)__VA_ARGS__))                                                    \
	)
#endif // JSTR_HAS_GENERIC

#define jstr_append(this_jstr, ...)                                                                            \
(                                                                                                              \
	PP_NARG(__VA_ARGS__) == 2                                                                              \
		? private_jstr_append(this_jstr, __VA_ARGS__, 0)                                               \
		: private_jstr_append(this_jstr, PP_FIRST_ARG(__VA_ARGS__), strlen(PP_FIRST_ARG(__VA_ARGS__))) \
)

#define private_jstr_reserve_x(this_jstr, multiplier)                                   \
	jstr_reserve_nocheck_exact(this_jstr, ((multiplier) * ((this_jstr)->capacity)))

#define jstr_reserve_2x(this_jstr) private_jstr_reserve_x(this_jstr, 2)
#define jstr_reserve_4x(this_jstr) private_jstr_reserve_x(this_jstr, 4)
#define jstr_reserve_8x(this_jstr) private_jstr_reserve_x(this_jstr, 8)
#define jstr_reserve_16x(this_jstr) private_jstr_reserve_x(this_jstr, 16)
#define jstr_reserve_32x(this_jstr) private_jstr_reserve_x(this_jstr, 32)
#define jstr_reserve_64x(this_jstr) private_jstr_reserve_x(this_jstr, 64)

#define private_jstr_reserve_s_x(this_jstr, multiplier)                                   \
	jstr_reserve_s_nocheck_exact(this_jstr, ((multiplier) * ((this_jstr)->capacity)))

#define jstr_reserve_s_2x(this_jstr) private_jstr_reserve_s_x(this_jstr, 2)
#define jstr_reserve_s_4x(this_jstr) private_jstr_reserve_s_x(this_jstr, 4)
#define jstr_reserve_s_8x(this_jstr) private_jstr_reserve_s_x(this_jstr, 8)
#define jstr_reserve_s_16x(this_jstr) private_jstr_reserve_s_x(this_jstr, 16)
#define jstr_reserve_s_32x(this_jstr) private_jstr_reserve_s_x(this_jstr, 32)
#define jstr_reserve_s_64x(this_jstr) private_jstr_reserve_s_x(this_jstr, 64)

#define jstr_foreach(elem, jstr)                   \
	for (char *elem = jstr.data; elem; ++elem)

#define jstr_begin(this_jstr) ((this_jstr)->data)
#define jstr_end(this_jstr) (((this_jstr)->data) + ((this_jstr)->size))

#define jstr_cbegin(this_jstr) ((const char *)((this_jstr)->data))
#define jstr_cend(this_jstr) ((const char *)(((this_jstr)->data) + ((this_jstr)->size)))

#define jstr_typeof_data char *

#define jstr_foreach_index(elem, jstr)                        \
	for (size_t i = 0, end__ = jstr.size; i < end__; ++i)

#endif // JSTR_H_DEF__
