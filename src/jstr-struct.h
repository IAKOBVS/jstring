/* See LICENSE file for copyright and license details. */

#ifndef JSTR_STRUCT_H
#define JSTR_STRUCT_H 1

#include "jstr-macros.h"

JSTRP_BEGIN_DECLS
#include <stddef.h>
JSTRP_END_DECLS

JSTRP_BEGIN_DECLS

typedef struct jstr_ty {
	char *data;
	size_t size;
	size_t capacity;
} jstr_ty;

JSTRP_END_DECLS

#endif /* JSTR_STRUCT_H */
