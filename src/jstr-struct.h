/* See LICENSE file for copyright and license details. */

#ifndef JSTR_STRUCT_H
#define JSTR_STRUCT_H 1

#include "jstr-macros.h"

JSTRP__BEGIN_DECLS
#include <stddef.h>
JSTRP__END_DECLS

JSTRP__BEGIN_DECLS

typedef struct jstr_ty {
	char *data;
	size_t size;
	size_t capacity;
} jstr_ty;

JSTRP__END_DECLS

#endif /* JSTR_STRUCT_H */
