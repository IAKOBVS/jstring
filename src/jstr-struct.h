/* See LICENSE file for copyright and license details. */

#ifndef JSTR_STRUCT_H
#define JSTR_STRUCT_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stddef.h>
PJSTR_END_DECLS

#define JSTR_STRUCT(j) &(j)->data, &(j)->size, &(j)->capacity

PJSTR_BEGIN_DECLS

typedef struct jstr_ty {
	char *data;
	size_t size;
	size_t capacity;
} jstr_ty;

PJSTR_END_DECLS

#endif /* JSTR_STRUCT_H */
