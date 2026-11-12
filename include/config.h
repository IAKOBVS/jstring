
#ifndef JSTR_CONFIG_H
/** @brief JSTR_CONFIG_H macro. */
#	define JSTR_CONFIG_H 1

#	ifndef JSTR_PANIC
/** @brief JSTR_PANIC macro. */
#		define JSTR_PANIC 0
#	endif

#	ifndef JSTR_DEBUG
/** @brief JSTR_DEBUG macro. */
#		define JSTR_DEBUG 0
#	endif

#	ifndef JSTR_USE_UNLOCKED_IO
/** @brief JSTR_USE_UNLOCKED_IO macro. */
#		define JSTR_USE_UNLOCKED_IO 0
#	endif
#	ifndef JSTR_USE_UNLOCKED_IO_READ
/** @brief JSTR_USE_UNLOCKED_IO_READ macro. */
#		define JSTR_USE_UNLOCKED_IO_READ 0
#	endif
#	ifndef JSTR_USE_UNLOCKED_IO_WRITE
/** @brief JSTR_USE_UNLOCKED_IO_WRITE macro. */
#		define JSTR_USE_UNLOCKED_IO_WRITE 0
#	endif

#	ifndef JSTR_MIN_CAP
/** @brief JSTR_MIN_CAP macro. */
#		define JSTR_MIN_CAP ((sizeof(size_t) == 8) ? 24 : 12)
#	endif

#	ifndef JSTR_MALLOC_ALIGNMENT
/** @brief JSTR_MALLOC_ALIGNMENT macro. */
#		define JSTR_MALLOC_ALIGNMENT (sizeof(size_t) + sizeof(size_t))
#	endif

#endif /* JSTR_CONFIG_H */
