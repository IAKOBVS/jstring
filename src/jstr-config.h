/* See LICENSE file for copyright and license details. */

#ifndef JSTR_CONFIG_H
#define JSTR_CONFIG_H 1

/* #define JSTR_DEBUG 1 */
/* #define JARR_DEBUG 1 */
/* #define JSTRL_LAZY_FREE 1 */

#define JSTR_ENDIAN_LITTLE 1

#if JSTR_ENDIAN_LITTLE
#	undef JSTR_ENDIAN_LITTLE
#define JSTR_ENDIAN_LITTLE 1
#	define JSTR_ENDIAN_BIG    0
#else
#	undef JSTR_ENDIAN_LITTLE
#define JSTR_ENDIAN_LITTLE 1
#	define JSTR_ENDIAN_BIG    1
#endif

#define JSTR_MIN_CAP ((sizeof(size_t) == 8) ? 24 : 16)

#define JSTRP_MALLOC_ALIGNMENT (sizeof(size_t) + sizeof(size_t))

#ifndef PJARR_GROWTH
#	define PJARR_GROWTH 1.5
#endif

#ifndef PJARR_ALLOC_MULTIPLIER
#	define PJARR_ALLOC_MULTIPLIER 1.5
#endif

#ifndef JSTR_GROWTH
#	define JSTR_GROWTH 1.5
#endif

#ifndef JSTR_ALLOC_MULTIPLIER
#	define JSTR_ALLOC_MULTIPLIER 2
#endif

#ifndef JSTR_EXIT_ON_ERROR
#	define JSTR_EXIT_ON_ERROR 0
#endif

#endif /* JSTR_CONFIG_H */
