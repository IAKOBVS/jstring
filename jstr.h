#ifndef JSTR_H_DEF
#define JSTR_H_DEF

#define JSTR_RELEASE

#ifdef JSTR_RELEASE
	#undef JSTR_DEBUG
#else
	#define JSTR_DEBUG
#endif // JSTR_RELEASE

#include <stddef.h>
#include "macros.h"
#include "/home/james/c/macros/vargc.h"

#define JSTR_ALIGN_POWER_OF_TWO
#define JSTR_64_BIT

#ifdef JSTR_ALIGN_POWER_OF_TWO
	#ifdef JSTR_64_BIT
		#define JSTR_NEAR_POW2(x) private_jstr_next_pow2_64(x)
	#elif JSTR_32_BIT
		#define JSTR_NEAR_POW2(x) private_jstr_next_pow2_32(x)
	#else
		#define JSTR_NEAR_POW2(x) (x)
	#endif
#endif // JSTR_ALIGN_POWER_OF_TWO

typedef struct jstring_t {
	char *data;
	size_t capacity;
	size_t size; 
} jstring_t;

int private_jstr_cat(jstring_t *RESTRICT this_jstr, const size_t len, ...);
#define jstr_cat(this_jstr, ...) private_jstr_cat(this_jstr, PP_STRLEN_VA_ARGS(__VA_ARGS__), __VA_ARGS__, NULL)

void jstr_init(jstring_t *RESTRICT this_jstr);

void jstr_delete(jstring_t *RESTRICT this_jstr);

int jstr_new_wsize(jstring_t *RESTRICT this_jstr, const char *RESTRICT src, const size_t src_size);
#define jstr_new(this_jstr, src) jstr_new_wsize(this_jstr, src, strlen(src))
int jstr_new_alloc(jstring_t *RESTRICT this_jstr, const size_t cap);

int jstr_push_back(jstring_t *this_jstr, const char c);
int jstr_push_back_nocheck(jstring_t *this_jstr, const char c);
void jstr_push_back_noalloc(jstring_t *this_jstr, const char c);

void jstr_push_front_noalloc(jstring_t *RESTRICT this_jstr, const char c);
int jstr_push_front_nocheck(jstring_t *RESTRICT this_jstr, const char c);
int jstr_push_front(jstring_t *RESTRICT this_jstr, const char c);

void jstr_pop_back(jstring_t *RESTRICT this_jstr);
void jstr_pop_front(jstring_t *RESTRICT this_jstr, const char c);

int jstr_append(jstring_t *this_jstr, const char *RESTRICT src, const size_t src_size);
void jstr_append_noalloc(jstring_t *this_jstr, const char *RESTRICT src, const size_t src_size);
#define jstr_append_auto(this_jstr, src) jstr_append(this_jstr, src, strlen(src))

/* swaps this_jstr with src and updates the jstring_t struct members */
void jstr_swap(jstring_t *RESTRICT this_jstr, jstring_t *RESTRICT src);
void jstr_swap_str(jstring_t *RESTRICT this_jstr, char **RESTRICT src, size_t *src_size, size_t *src_capacity);

int jstr_shrink_to_fit(jstring_t *RESTRICT this_jstr);

int jstr_reserve(jstring_t *RESTRICT this_jstr, const size_t capacity);
int jstr_reserve_nocheck(jstring_t *RESTRICT this_jstr, const size_t capacity);

/* replaces this_jstr->data with this_jstr and reallocs if needed */
int jstr_replace(jstring_t *RESTRICT this_jstr, char *RESTRICT src, const size_t src_size);
#define jstr_replace_auto(this_jstr, src) jstr_replace(this_jstr, src, strlen(src))

void jstr_replace_noalloc(jstring_t *RESTRICT this_jstr, char *RESTRICT src, const size_t src_size);
#define jstr_replace_noalloc_auto(this_jstr, src) jstr_replace(this_jstr, src, strlen(src))

/* compares two Jstr, and if equal, returns 0 */
int jstr_cmp(jstring_t *RESTRICT this_jstr, jstring_t *RESTRICT src);

#define jstr_foreach(elem, jstr)                   \
	for (char *elem = jstr.data; elem; ++elem)

#define jstr_foreach_index(elem, jstr)                        \
	for (size_t i = 0, end__ = jstr.size; i < end__; ++i)

#endif // JSTR_H_DEF
