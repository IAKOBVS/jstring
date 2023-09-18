#ifndef JSTR_STRUCT_H
#define JSTR_STRUCT_H 1

#include "jstr-macros.h"

P_JSTR_BEGIN_DECLS
#include <stddef.h>

typedef struct jstr_ty {
	char *data;
	size_t size;
	size_t capacity;
} jstr_ty;

P_JSTR_END_DECLS

#endif /* JSTR_STRUCT_H */
