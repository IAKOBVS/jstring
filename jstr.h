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

int private_jstr_cat(jstring_t *RESTRICT dest, ...);
#define jstr_cat(JSTR, ...) private_jstr_cat(JSTR, __VA_ARGS__, NULL)

void jstr_init(jstring_t *RESTRICT dest);

void jstr_delete_nocheck(jstring_t *RESTRICT dest);
void jstr_delete(jstring_t *RESTRICT dest);

int jstr_new(jstring_t *RESTRICT dest, const char *RESTRICT src, const size_t src_size);
int jstr_new_alloc(jstring_t *RESTRICT dest, const size_t cap);
#define jstr_new_auto(dest, src) jstr_new(dest, src, strlen(src))

int jstr_push_back(jstring_t *dest, const char c);

void jstr_pop_back(jstring_t *RESTRICT dest);

int jstr_append(jstring_t *dest, const char *RESTRICT src, const size_t src_size);
#define jstr_append_auto(dest, src) jstr_append(dest, src, strlen(src))

/* swaps dest with src and updates the jstring_t struct members */
void jstr_swap(jstring_t *RESTRICT dest, jstring_t *RESTRICT src);
void jstr_swap_str(jstring_t *RESTRICT dest, char **RESTRICT src, size_t *src_size, size_t *src_capacity);

int jstr_shrink(jstring_t *RESTRICT dest);

int jstr_reserve(jstring_t *RESTRICT dest, size_t capacity);

/* replaces dest->data with dest and reallocs if needed */
int jstr_replace(jstring_t *RESTRICT dest, char *RESTRICT src, const size_t src_size);
#define jstr_replace_auto(dest, src) jstr_replace(dest, src, strlen(src))

void jstr_replace_noalloc(jstring_t *RESTRICT dest, char *RESTRICT src, const size_t src_size);
#define jstr_replace_noalloc_auto(dest, src) jstr_replace(dest, src, strlen(src))

/* compares two Jstr, and if equal, returns 0 */
int jstr_cmp(jstring_t *RESTRICT dest, jstring_t *RESTRICT src);

#define jstr_foreach(elem, jstr)                                                                \
	for (char RESTRICT *elem = jstr.data, *RESTRICT end = jstr.data + jstr.size; elem < end; ++elem)
#define jstr_foreach_index(elem, jstr)                    \
	for (size_t i = 0, end = jstr.size; i < end; ++i)

#endif
