#ifndef JSTR_H_DEF
#define JSTR_H_DEF

#define JSTR_RELEASE

#ifdef JSTR_RELEASE
	#undef JSTR_DEBUG
#else
	#define JSTR_DEBUG
#endif

#include <stddef.h>
#include "/home/james/c/macros/vargc.h"
#include "macros.h"

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) && !defined(__STDC_NO_STATIC_ASSERT__) && defined(_Static_assert)
#define JSTR_ASSERT(expr, msg) _Static_assert(expr, msg)
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) && !defined(__STDC_NO_STATIC_ASSERT__)
#define JSTR_ASSERT(expr, msg) static_assert(expr, msg)
#else
#endif

#define JARR_ALIGN_POWER_OF_TWO
#define JARR_64_BIT

#if defined(__GNUC__) || defined(__clang__)
	#define JARR_NEAR_POW2_32(x)                                          \
		((x) ? 1 : 1UL << (sizeof((x)) * 8 - __builtin_clz((x) - 1)))
	#define JARR_NEAR_POW2_64(x)                                             \
		((x) ? 1 : 1ULL << (sizeof((x)) * 8 - __builtin_clzll((x) - 1)))
#else
	#define JARR_NEAR_POW2_32(x) \
		(x--,                \
		x |= x >> 1,         \
		x |= x >> 2,         \
		x |= x >> 4,         \
		x |= x >> 8,         \
		x |= x >> 16,        \
		++x)
	#define JARR_NEAR_POW2_64(x) \
		(x--,                \
		x |= x >> 1,         \
		x |= x >> 2,         \
		x |= x >> 4,         \
		x |= x >> 8,         \
		x |= x >> 16,        \
		x |= x >> 32,        \
		++x)
#endif

#ifdef JARR_ALIGN_POWER_OF_TWO
	#ifdef JARR_64_BIT
		#define JARR_NEAR_POW2(x) JARR_NEAR_POW2_64(x)
	#elif JARR_32_BIT
		#define JARR_NEAR_POW2(x) JARR_NEAR_POW2_32(x)
	#else
		#define JARR_NEAR_POW2(x) (x)
	#endif
#endif

typedef struct jstring_t {
	char *data;
	size_t capacity;
	size_t size; 
} jstring_t;

int private_jstr_cat(jstring_t *RESTRICT this_jstr, ...);
#define jstr_cat(JSTR, ...) private_jstr_cat(JSTR, __VA_ARGS__, NULL)

void jstr_init(jstring_t *RESTRICT this_jstr);

void jstr_delete_nocheck(jstring_t *RESTRICT this_jstr);
void jstr_delete(jstring_t *RESTRICT this_jstr);

int jstr_new(jstring_t *RESTRICT this_jstr, const char *RESTRICT src, const size_t src_size);
#define jstr_new_auto(this_jstr, src) jstr_new(this_jstr, src, strlen(src))
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

#define jstr_foreach(elem, jstr)                                                                \
	for (char RESTRICT *elem = jstr.data, *RESTRICT end = jstr.data + jstr.size; elem < end; ++elem)
#define jstr_foreach_index(elem, jstr)                    \
	for (size_t i = 0, end = jstr.size; i < end; ++i)

#endif
