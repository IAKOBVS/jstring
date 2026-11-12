
#ifndef JSTR_STRUCT_H
/** @brief JSTR_STRUCT_H macro. */
#	define JSTR_STRUCT_H 1

#	include "macros.h"

JSTR_INTERNAL_BEGIN_DECLS
#	include <stddef.h>
JSTR_INTERNAL_END_DECLS

#	if defined(JSTR_IMPLEMENTATION)
#		undef JSTR_IMPLEMENTATION
#		include "internal/musl/twoway.h"
/** @brief JSTR_IMPLEMENTATION macro. */
#		define JSTR_IMPLEMENTATION 1
#	else
#		include "internal/musl/twoway.h"
#	endif

/** @brief jstr_struct macro. */
#	define jstr_struct(j)  &(j)->data, &(j)->size, &(j)->capacity
/** @brief jstr_literal macro. */
#	define jstr_literal(s) (s), (sizeof(s) - 1)
/** @brief jstr_literal_init macro. */
#	define jstr_literal_init(s)    \
		{                       \
			jstr_literal(s) \
		}

JSTR_INTERNAL_BEGIN_DECLS
/** @brief jstr_ty core string type. */
typedef struct jstr_ty {
	char *data;
	size_t size;
	size_t capacity;
} jstr_ty;

/** @brief jstr_literal_ty literal string type. */
typedef struct jstr_literal_ty {
	const char *data;
	const unsigned int size;
} jstr_literal_ty;

/** @brief jstr_cat_arg_ty concatenation argument type. */
typedef struct jstr_cat_arg_ty {
	const char *ptr;
	size_t size;
} jstr_cat_arg_ty;

/** @brief jstr_twoway_ty two-way matcher state. */
typedef struct jstr_twoway_ty {
	size_t _buf[4096 / sizeof(size_t)];
} jstr_twoway_ty;

/** @brief Set two-way needle length. */
static JSTR_ATTR_INLINE void
jstr_twoway_set_len(jstr_twoway_ty *t, size_t len)
{
	((jstr_internal_twoway_ty *)t)->needle_len = len;
}

/** @brief Get two-way needle length. */
static JSTR_ATTR_INLINE
size_t
jstr_twoway_get_len(const jstr_twoway_ty *t)
{
	return ((jstr_internal_twoway_ty *)t)->needle_len;
}

JSTR_INTERNAL_END_DECLS

#endif /* JSTR_STRUCT_H */
