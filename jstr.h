#ifndef JSTR_H_DEF
#define JSTR_H_DEF

#include <stddef.h>

#include "/home/james/c/vargc.h"
#include "macros.h"

typedef struct jstring_t {
	char *data;
	size_t capacity;
	size_t size; 
} jstring_t;

int private_jstr_cat(jstring_t *RESTRICT dest, ...);
#define jstr_cat(JSTR, ...) private_jstr_cat(JSTR, __VA_ARGS__, NULL)

void jstr_init(jstring_t *RESTRICT dest);

void jstr_delete_fast(jstring_t *RESTRICT dest);
void jstr_delete(jstring_t *RESTRICT dest);

int jstr_new(jstring_t *RESTRICT dest, const char *RESTRICT src, const size_t src_size);
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

/* compares two Jstr, and if equal, returns 0 */
int jstr_cmp(jstring_t *RESTRICT dest, jstring_t *RESTRICT src);

#endif
