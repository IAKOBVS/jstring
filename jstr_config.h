#ifndef JSTR_CONFIG_DEF_H
#define JSTR_CONFIG_DEF_H

#define JSTR_GROWTH_MULTIPLIER 2

#define JSTR_EXIT_ON_MALLOC_ERROR	   1
#define JSTR_PRINT_ERR_MSG_ON_MALLOC_ERROR 1

#define JSTR_FREE_ON_DESTRUCTOR_CPP	   1
#define JSTR_NULLIFY_PTR_ON_DESTRUCTOR_CPP 1
#define JSTR_NULLIFY_PTR_ON_DELETE	   1

#if JSTR_FREE_ON_DESTRUCTOR_CPP
#	undef JSTR_NULLIFY_PTR_ON_DELETE
#	define JSTR_NULLIFY_PTR_ON_DELETE 1
#endif /* JSTR_FREE_ON_DESTRUCTOR_CPP */

#if !JSTR_GROWTH_MULTIPLIER
#	define JSTR_GROWTH_MULTIPLIER 2
#endif /* JSTR_GROWTH_MULTIPLIER */

#endif /* JSTR_CONFIG_DEF_H */
