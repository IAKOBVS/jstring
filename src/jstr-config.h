/* See LICENSE file for copyright and license details. */

#ifndef JSTR_CONFIG_H
#define JSTR_CONFIG_H 1

#ifndef JSTR_DEBUG
#	define JSTR_DEBUG 0
#endif

/* Print error message and call exit() on errors. */
#ifndef JSTR_PANIC
#	define JSTR_PANIC 0
#endif

/*
   When removing a string (jstr_ty) from a string list (jstrlist_ty), don't immediately
   call free(). This may result in fewer allocations when strings are often added to
   and removed from the list. All elements are freed when jstrl_free() is called.
*/
#ifndef JSTRL_LAZY_FREE
#	define JSTRL_LAZY_FREE 0
#endif

#define PJSTR_MIN_CAP          ((sizeof(size_t) == 8) ? 24 : 16)
#define PJSTR_ATTR_MALLOC_ALIGNMENT (sizeof(size_t) + sizeof(size_t))

/* Allocate more than needed for the initial malloc(). */
#ifndef PJSTR_ALLOC_MULTIPLIER
#	define PJSTR_ALLOC_MULTIPLIER 2
#endif
#ifndef PJARR_ALLOC_MULTIPLIER
#	define PJARR_ALLOC_MULTIPLIER 1.5
#endif

/* Growth factor. */
#ifndef PJSTR_GROWTH
#	define PJSTR_GROWTH 1.5
#endif
#ifndef PJSTRL_GROWTH
#	define PJSTRL_GROWTH 1.5
#endif
#ifndef PJARR_GROWTH
#	define PJARR_GROWTH 1.5
#endif

/* File extensions interpreted as text files. */
#ifndef PJSTRIO_EXT_ARRAY_FT_TEXT
#	define PJSTRIO_EXT_ARRAY_FT_TEXT "C", "S", "c", "cc", "cs", "cpp", "h", "hh", "hpp", "html", "js", "json", "md", "pl", "pm", "py", "pyi", "rs", "s", "sh", "ts", "txt"
#endif
/* File extensions interpreted as binary files. */
#ifndef PJSTRIO_EXT_ARRAY_FT_BINARY
#	define PJSTRIO_EXT_ARRAY_FT_BINARY "a", "bin", "gz", "jpg", "jpeg", "mp4", "mp3", "mkv", "o", "pdf", "png", "pyc", "rar", "so", "wav", "zip"
#endif

/* You shouldn't mess with these. */
#define JSTR_USE_LGPL 1

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
