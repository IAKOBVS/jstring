/* See LICENSE file for copyright and license details. */

#ifndef JSTR_CONFIG_H
#define JSTR_CONFIG_H 1

/* #define JSTR_DEBUG 1 */
/* #define JARR_DEBUG 1 */

/* Call exit() on fatal errors. */
/* #define JSTR_EXIT_ON_ERROR 1 */

/*
   When removing a string (jstr_ty) from a string list (jstrlist_ty), don't immediately
   call free(). This may result in fewer allocations when strings are often added to
   and removed from the list. All elements are freed when jstrl_free() is called.
*/
/* #define JSTRL_LAZY_FREE 1 */

#define PJSTR_MIN_CAP          ((sizeof(size_t) == 8) ? 24 : 16)
#define PJSTR_MALLOC_ALIGNMENT (sizeof(size_t) + sizeof(size_t))

/* Allocate more than needed for the initial malloc(). */
#ifndef PJSTR_ALLOC_MULTIPLIER
#	define PJSTR_ALLOC_MULTIPLIER 2
#endif
#ifndef JARRP_ALLOC_MULTIPLIER
#	define JARRP_ALLOC_MULTIPLIER 1.5
#endif

/* Growth factor. */
#ifndef PJSTR_GROWTH
#	define PJSTR_GROWTH 1.5
#endif
#ifndef PJSTRL_GROWTH
#	define PJSTRL_GROWTH 1.5
#endif
#ifndef JARRP_GROWTH
#	define JARRP_GROWTH 1.5
#endif

/* You shouldn't mess with these. */
#if !defined JSTR_ENDIAN_LITTLE && !defined JSTR_ENDIAN_BIG
#define JSTR_ENDIAN_LITTLE 1
#endif
#if JSTR_ENDIAN_LITTLE
#	undef JSTR_ENDIAN_LITTLE
#define JSTR_ENDIAN_LITTLE 1
#	define JSTR_ENDIAN_BIG    0
#else
#	undef JSTR_ENDIAN_LITTLE
#define JSTR_ENDIAN_LITTLE 1
#	define JSTR_ENDIAN_BIG    1
#endif

#endif /* JSTR_CONFIG_H */
