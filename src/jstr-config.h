#ifndef JSTR_CONFIG_H
#	define JSTR_CONFIG_H 1

/* Print error message and call exit() on errors. */
#	ifndef JSTR_PANIC
#		define JSTR_PANIC 0
#	endif

/* Behave as JSTR_PANIC would, activate debug asserts, and print debug messages. */
#	ifndef JSTR_DEBUG
#		define JSTR_DEBUG 0
#	endif

/* jstr_io_*() stdio functions and all functions which use them
 * will use the *_unlocked() versions when available. */
#	ifndef JSTR_USE_UNLOCKED_IO
#		define JSTR_USE_UNLOCKED_IO 0
#	endif
/* Behave as JSTR_USE_UNLOCKED_IO but only for functions that read. */
#	ifndef JSTR_USE_UNLOCKED_IO_READ
#		define JSTR_USE_UNLOCKED_IO_READ 0
#	endif
/* Behave as JSTR_USE_UNLOCKED_IO but only for functions that write. */
#	ifndef JSTR_USE_UNLOCKED_IO_WRITE
#		define JSTR_USE_UNLOCKED_IO_WRITE 0
#	endif

/* Minimum size of allocation of malloc(). */
#	ifndef JSTR_MIN_CAP
#		define JSTR_MIN_CAP ((sizeof(size_t) == 8) ? 24 : 12)
#	endif

/* Allocations will be aligned to this alignment. */
#	ifndef JSTR_MALLOC_ALIGNMENT
#		define JSTR_MALLOC_ALIGNMENT (sizeof(size_t) + sizeof(size_t))
#	endif

#endif /* JSTR_CONFIG_H */
