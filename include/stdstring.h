
#ifndef JSTR_STDSTRING_H
#	define JSTR_STDSTRING_H 1

#	include "macros.h"
#	include "struct.h"

JSTR_INTERNAL_BEGIN_DECLS
#if JSTR_HAVE_STRCASECMP && JSTR_HAVE_STRNCASECMP
#	include <strings.h>
#endif
#	include <stdlib.h>
#	include <string.h>
#	include <limits.h>
JSTR_INTERNAL_END_DECLS

#ifdef JSTR_IMPLEMENTATION
#undef JSTR_IMPLEMENTATION
#	include "ctype.h"
#define JSTR_IMPLEMENTATION 1
#endif

#	define R JSTR_RESTRICT

JSTR_INTERNAL_BEGIN_DECLS

/**
 * @brief Compare at most @p n bytes of @p s1 and @p s2 case-insensitively.
 *
 * @param s1 First string; must be non-null when @p n > 0.
 * @param s2 Second string; must be non-null when @p n > 0.
 * @param n  Maximum number of bytes to compare.
 * @return 0 if the compared portions are equal; otherwise the difference of
 *         the lowercased bytes at the first differing position.
 * @note Stops early at a NUL byte in either string. Behavior is undefined
 *       for NULL pointers with @p n > 0.
 */
JSTR_FUNC_PURE
int
jstr_strncasecmp(const char *s1, const char *s2, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_STRNCASECMP && !JSTR_TEST
	return strncasecmp(s1, s2, n);
#	else
	int ret = 0;
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	while (n-- && !(ret = jstr_tolower(*p1) - jstr_tolower(*p2++)) && *p1++) {}
	return ret;
#	endif
}
#else
;
#endif

/**
 * @brief Test whether the first @p n bytes of @p s1 and @p s2 are equal
 *        case-insensitively.
 *
 * @param s1 First string; must be non-null when @p n > 0.
 * @param s2 Second string; must be non-null when @p n > 0.
 * @param n  Number of bytes to compare.
 * @return 0 if all compared bytes match (or @p n is 0); non-zero otherwise.
 * @note Stops early at a NUL byte in either string.
 */
JSTR_FUNC_PURE
int
jstr_strncasecmpeq(const char *s1, const char *s2, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_STRNCASECMP && !JSTR_TEST
	return strncasecmp(s1, s2, n);
#	else
	if (jstr_unlikely(n == 0))
		return 0;
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	for (; jstr_tolower(*p1) == jstr_tolower(*p2++) && *p1 && n; ++p1, --n) {}
	return *p1 && n;
#	endif
}
#else
;
#endif

/**
 * @brief Compare exactly @p n bytes of @p s1 and @p s2 case-insensitively.
 *
 * Unlike jstr_strncasecmp(), this does not stop at NUL bytes and always
 * inspects all @p n bytes.
 *
 * @param s1 First buffer; must be non-null when @p n > 0.
 * @param s2 Second buffer; must be non-null when @p n > 0.
 * @param n  Number of bytes to compare; must not exceed either buffer size.
 * @return 0 if all @p n bytes match; otherwise the difference at the first
 *         differing byte.
 */
JSTR_FUNC_PURE
int
jstr_strcasecmp_len(const char *s1, const char *s2, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_STRNCASECMP && !JSTR_TEST
	return strncasecmp(s1, s2, n);
#	else
	int ret = 0;
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	while (n-- && !(ret = jstr_tolower(*p1++) - jstr_tolower(*p2++))) {}
	return ret;
#	endif
}
#else
;
#endif

/**
 * @brief Test whether exactly @p n bytes of @p s1 and @p s2 are equal
 *        case-insensitively.
 *
 * @param s1 First buffer; must be non-null when @p n > 0.
 * @param s2 Second buffer; must be non-null when @p n > 0.
 * @param n  Number of bytes to compare; must not exceed either buffer size.
 * @return 0 if all @p n bytes match; 1 otherwise.
 */
JSTR_FUNC_PURE
int
jstr_memcasecmpeq(const char *s1, const char *s2, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	for (; n && jstr_tolower(*p1++) == jstr_tolower(*p2++); --n) {}
	return n ? 1 : 0;
}
#else
;
#endif

/**
 * @brief Test whether the first @p n bytes of @p s1 and @p s2 are equal
 *        case-insensitively.
 *
 * @param s1 First string; must be non-null when @p n > 0.
 * @param s2 Second string; must be non-null when @p n > 0.
 * @param n  Number of bytes to compare.
 * @return 0 if all compared bytes match (or @p n is 0); non-zero otherwise.
 * @note May stop at an embedded NUL when libc strncasecmp is available;
 *       otherwise all @p n bytes are inspected.
 */
JSTR_FUNC_PURE
int
jstr_strcasecmpeq_len(const char *s1, const char *s2, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_STRNCASECMP && !JSTR_TEST
	return strncasecmp(s1, s2, n);
#	else
	return jstr_memcasecmpeq(s1, s2, n);
#	endif
}
#else
;
#endif

/**
 * @brief Compare two NUL-terminated strings @p s1 and @p s2
 *        case-insensitively.
 *
 * @param s1 First string; must be non-null.
 * @param s2 Second string; must be non-null.
 * @return 0 if the strings are equal; otherwise the difference of the
 *         lowercased bytes at the first differing position.
 */
JSTR_FUNC_PURE
int
jstr_strcasecmp(const char *s1, const char *s2) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_STRCASECMP && !JSTR_TEST
	return strcasecmp(s1, s2);
#	else
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	int ret;
	while (!(ret = jstr_tolower(*p1) - jstr_tolower(*p2++)) && *p1++) {}
	return ret;
#	endif
}
#else
;
#endif

/**
 * @brief Compare two NUL-terminated strings @p s1 and @p s2
 *        case-insensitively and return the trailing byte.
 *
 * @param s1 First string; must be non-null.
 * @param s2 Second string; must be non-null.
 * @return The byte value at the first differing (or terminating) position
 *         of @p s2; 0 if the strings are fully equal.
 */
JSTR_FUNC_PURE
int
jstr_strcasecmpeq_loop(const char *s1, const char *s2) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	while (jstr_tolower(*p1) == jstr_tolower(*p2++) && *p1++) {}
	return *(p2 - 1);
}
#else
;
#endif

/**
 * @brief Test whether two NUL-terminated strings @p s1 and @p s2 are equal
 *        case-insensitively.
 *
 * @param s1 First string; must be non-null.
 * @param s2 Second string; must be non-null.
 * @return 0 if the strings are fully equal; otherwise the byte value at the
 *         first differing (or terminating) position of @p s2.
 */
JSTR_FUNC_PURE
int
jstr_strcasecmpeq(const char *s1, const char *s2) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_STRCASECMP && !JSTR_TEST
	return strcasecmp(s1, s2);
#	else
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	while (jstr_tolower(*p1) == jstr_tolower(*p2++) && *p1++) {}
	return *(p2 - 1);
#	endif
}
#else
;
#endif

/**
 * @brief Compare two NUL-terminated strings @p s1 and @p s2 and return the
 *        trailing byte at the first difference.
 *
 * @param s1 First string; must be non-null.
 * @param s2 Second string; must be non-null.
 * @return The byte value at the first differing (or terminating) position
 *         of @p s2; 0 if the strings are fully equal.
 */
JSTR_FUNC_PURE
int
jstr_strcmpeq_loop(const char *s1, const char *s2) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	while ((*s1 == *s2++) && *s1++) {}
	return *(s2 - 1);
}
#else
;
#endif

/**
 * @brief Test whether exactly @p n bytes of @p s1 and @p s2 are equal.
 *
 * @param s1 First buffer; must be non-null when @p n > 0.
 * @param s2 Second buffer; must be non-null when @p n > 0.
 * @param n  Number of bytes to compare.
 * @return 0 if all @p n bytes match; non-zero (1) otherwise.
 */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_ATTR_ACCESS((__read_only__, 2, 3))
JSTR_FUNC_PURE
int
jstr_memcmpeq_loop(const void *s1, const void *s2, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	for (; n && (*p1++ == *p2++); --n) {}
	return n ? 1 : 0;
}
#else
;
#endif

/**
 * @brief Test whether exactly @p n bytes of @p s1 and @p s2 are equal
 *        case-insensitively.
 *
 * @param s1 First buffer; must be non-null when @p n > 0.
 * @param s2 Second buffer; must be non-null when @p n > 0.
 * @param n  Number of bytes to compare.
 * @return 0 if all @p n bytes match; non-zero (1) otherwise.
 */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
int
jstr_strcasecmpeq_len_loop(const void *s1, const void *s2, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	for (; n && (jstr_tolower(*p1++) == jstr_tolower(*p2++)); --n) {}
	return n ? 1 : 0;
}
#else
;
#endif

/**
 * @brief Compute the length of @p s, capped at @p maxlen.
 *
 * @param s       NUL-terminated string; must be non-null.
 * @param maxlen  Upper bound on the returned length.
 * @return The number of bytes before the first NUL, or @p maxlen if no NUL
 *         is found within the first @p maxlen bytes.
 */
JSTR_FUNC_PURE
size_t
jstr_strnlen(const char *s, size_t maxlen) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_STRNLEN
	return strnlen(s, maxlen);
#	else
	const char *p = (char *)memchr(s, '\0', maxlen);
	return p ? (size_t)(p - s) : maxlen;
#	endif
}
#else
;
#endif

/**
 * @brief Compute the length of @p s, capped at @p maxlen, without libc
 *        helpers.
 *
 * @param s       NUL-terminated string; must be non-null.
 * @param maxlen  Upper bound on the returned length.
 * @return The number of bytes before the first NUL, or @p maxlen if no NUL
 *         is found within the first @p maxlen bytes.
 */
JSTR_FUNC_PURE
size_t
jstr_strnlen_loop(const char *s, size_t maxlen) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	size_t n = maxlen;
	for (; n && *s; --n, ++s) {}
	return maxlen - n;
}
#else
;
#endif

/**
 * @brief Fill @p n bytes of @p s with byte @p c.
 *
 * @param s  Destination buffer; must be non-null when @p n > 0.
 * @param c  Byte value to store (converted to unsigned char).
 * @param n  Number of bytes to fill.
 * @return Pointer to one past the last byte written (@p s + @p n).
 */
JSTR_ATTR_ACCESS((__read_write__, 1, 3))
JSTR_FUNC
static JSTR_ATTR_INLINE
void *
jstr_mempset(void *s, int c, size_t n) JSTR_NOEXCEPT
{
	return (char *)memset(s, c, n) + n;
}

/**
 * @brief Fill @p n bytes of @p s with byte @p c.
 *
 * @param s  Destination buffer; must be non-null when @p n > 0.
 * @param c  Byte value to store (converted to unsigned char).
 * @param n  Number of bytes to fill.
 * @return Pointer to one past the last byte written (@p s + @p n).
 */
JSTR_ATTR_ACCESS((__read_write__, 1, 3))
JSTR_FUNC
static JSTR_ATTR_INLINE
char *
jstr_stpset_len(char *s, int c, size_t n) JSTR_NOEXCEPT
{
	return (char *)memset(s, c, n) + n;
}

/**
 * @brief Fill the NUL-terminated string @p s with byte @p c.
 *
 * Only the bytes before the terminating NUL are overwritten; the NUL is
 * left in place.
 *
 * @param s  Destination string; must be non-null.
 * @param c  Byte value to store (converted to unsigned char).
 * @return Pointer to the terminating NUL of @p s.
 */
JSTR_ATTR_ACCESS((__read_write__, 1, 2))
JSTR_FUNC_VOID
static JSTR_ATTR_INLINE
char *
jstr_stpset(char *s, int c) JSTR_NOEXCEPT
{
	return jstr_stpset_len(s, c, strlen(s));
}

/**
 * @brief Zero @p n bytes of @p s.
 *
 * @param s  Destination buffer; must be non-null when @p n > 0.
 * @param n  Number of bytes to zero.
 */
JSTR_ATTR_ACCESS((__read_write__, 1, 2))
JSTR_FUNC_VOID
static JSTR_ATTR_INLINE
void
jstr_bzero(void *s, size_t n) JSTR_NOEXCEPT
{
	memset(s, 0, n);
}

/**
 * @brief Zero the contents of the NUL-terminated string @p s, keeping its
 *        terminating NUL.
 *
 * @param s  Destination string; must be non-null.
 */
JSTR_FUNC_VOID
static JSTR_ATTR_INLINE
void
jstr_strzero(char *s) JSTR_NOEXCEPT
{
	memset(s, 0, strlen(s));
}

/**
 * @brief Zero the contents of @p s and return a pointer past the end.
 *
 * Only the bytes before the terminating NUL are zeroed; the NUL is left in
 * place.
 *
 * @param s  Destination string; must be non-null.
 * @return Pointer to the terminating NUL of @p s.
 */
JSTR_FUNC_VOID
char *
jstr_stpzero(char *s) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const size_t sz = strlen(s);
	return (char *)memset(s, 0, sz) + sz;
}
#else
;
#endif

/**
 * @brief Search for byte @p c within the first min(@p sz, @p n) bytes of
 *        @p s.
 *
 * @param s   Buffer to search; must be non-null when the search length > 0.
 * @param c   Byte value to find (converted to unsigned char).
 * @param sz  Size of the buffer @p s.
 * @param n   Maximum number of bytes to search.
 * @return Pointer to the first matching byte, or NULL if not found.
 */
JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static JSTR_ATTR_INLINE
void *
jstr_memnchr(const void *s, int c, size_t sz, size_t n) JSTR_NOEXCEPT
{
	return (void *)memchr(s, c, JSTR_MIN(n, sz));
}

/**
 * @brief Search for byte @p c within the first min(@p sz, @p n) bytes of
 *        the string @p s.
 *
 * @param s   String to search; must be non-null when the search length > 0.
 * @param c   Byte value to find (converted to unsigned char).
 * @param sz  Size of the buffer @p s.
 * @param n   Maximum number of bytes to search.
 * @return Pointer to the first matching byte, or NULL if not found.
 */
JSTR_FUNC_PURE
static JSTR_ATTR_INLINE
char *
jstr_strnchr_len(const char *s, int c, size_t sz, size_t n) JSTR_NOEXCEPT
{
	return (char *)jstr_memnchr(s, c, sz, n);
}

/**
 * @brief Copy @p sz bytes from @p src to @p dst.
 *
 * @param dst  Destination buffer; must be non-null when @p sz > 0 and must
 *             not overlap @p src.
 * @param src  Source buffer; must be non-null when @p sz > 0.
 * @param sz   Number of bytes to copy.
 * @return Pointer to one past the last byte written (@p dst + @p sz).
 */
JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC_RET_NONNULL
void *
jstr_mempcpy(void *R dst, const void *R src, size_t sz) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_MEMPCPY
	return mempcpy(dst, src, sz);
#	else
	return (char *)memcpy(dst, src, sz) + sz;
#	endif
}
#else
;
#endif

/**
 * @brief Copy @p n bytes from @p src to @p dst, returning a pointer past
 *        the end.
 *
 * @param dst  Destination buffer; must be non-null when @p n > 0 and must
 *             not overlap @p src.
 * @param src  Source buffer; must be non-null when @p n > 0.
 * @param n    Number of bytes to copy.
 * @return Pointer to one past the last byte written (@p dst + @p n).
 */
JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC
static JSTR_ATTR_INLINE
void *
jstr_mempmove(void *dst, const void *src, size_t n) JSTR_NOEXCEPT
{
	return (char *)memmove(dst, src, n) + n;
}

/**
 * @brief Copy @p n bytes from @p src to @p dst and append a NUL terminator.
 *
 * The destination must have room for at least @p n + 1 bytes. Overlapping
 * ranges are handled safely (uses memmove).
 *
 * @param dst  Destination buffer; must be non-null when @p n > 0.
 * @param src  Source buffer; must be non-null when @p n > 0.
 * @param n    Number of bytes to copy.
 */
JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC_VOID
static JSTR_ATTR_INLINE
void
jstr_strmove_len(char *dst, const char *src, size_t n) JSTR_NOEXCEPT
{
	*(char *)jstr_mempmove(dst, src, n) = '\0';
}

/**
 * @brief Copy @p n bytes from @p src to @p dst, append a NUL terminator,
 *        and return a pointer past the end.
 *
 * The destination must have room for at least @p n + 1 bytes.
 *
 * @param dst  Destination buffer; must be non-null when @p n > 0.
 * @param src  Source buffer; must be non-null when @p n > 0.
 * @param n    Number of bytes to copy.
 * @return Pointer to the terminating NUL byte written (@p dst + @p n).
 */
JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC
char *
jstr_stpmove_len(char *dst, const char *src, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	jstr_strmove_len(dst, src, n);
	return (char *)dst + n;
}
#else
;
#endif

/**
 * @brief Copy the NUL-terminated string @p src to @p dst (using strlen)
 *        and return a pointer past the end.
 *
 * @param dst  Destination buffer; must be non-null.
 * @param src  Source string; must be non-null.
 * @return Pointer to the terminating NUL byte written.
 */
JSTR_FUNC
char *
jstr_stpmove(char *dst, const char *src) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_stpmove_len(dst, src, strlen((const char *)src));
}
#else
;
#endif

/**
 * @brief Copy @p n bytes from @p src to @p dst and append a NUL terminator.
 *
 * The destination must have room for at least @p n + 1 bytes.
 *
 * @param dst  Destination buffer; must be non-null when @p n > 0.
 * @param src  Source buffer; must be non-null when @p n > 0.
 * @param n    Number of bytes to copy.
 */
JSTR_FUNC_VOID
void
jstr_strcpy_len(char *R dst, const char *R src, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	*(char *)jstr_mempcpy(dst, src, n) = '\0';
}
#else
;
#endif

/**
 * @brief Copy @p n bytes from @p src to @p dst, append a NUL terminator,
 *        and return a pointer past the end.
 *
 * The destination must have room for at least @p n + 1 bytes.
 *
 * @param dst  Destination buffer; must be non-null when @p n > 0.
 * @param src  Source buffer; must be non-null when @p n > 0.
 * @param n    Number of bytes to copy.
 * @return Pointer to the terminating NUL byte written (@p dst + @p n).
 */
JSTR_ATTR_ACCESS((__write_only__, 1, 3))
JSTR_FUNC_VOID
char *
jstr_stpcpy_len(char *R dst, const char *R src, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	jstr_strcpy_len(dst, src, n);
	return (char *)dst + n;
}
#else
;
#endif

/**
 * @brief Copy bytes from @p src to @p dst until byte @p c is found or
 *        @p n bytes have been copied.
 *
 * If @p c is found, it is copied and a pointer past it is returned.
 *
 * @param dst  Destination buffer; must be non-null when @p n > 0.
 * @param src  Source buffer; must be non-null when @p n > 0.
 * @param c    Stop byte (converted to unsigned char).
 * @param n    Maximum number of bytes to copy.
 * @return Pointer past the copied stop byte if found; NULL otherwise.
 */
JSTR_ATTR_ACCESS((__write_only__, 1, 4))
JSTR_FUNC
void *
jstr_memccpy(void *R dst, const void *R src, int c, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_MEMCCPY
	return memccpy(dst, src, c, n);
#	else
	const char *p = (char *)memchr(src, c, n);
	if (p != NULL)
		return jstr_stpcpy_len(dst, src, JSTR_DIFF(p, (const char *)src) + 1);
	memcpy(dst, src, n);
	return NULL;
#	endif /* HAVE_MEMCPY */
}
#else
;
#endif

/**
 * @brief Allocate and copy @p n bytes from @p s.
 *
 * @param s  Source buffer; must be non-null when @p n > 0.
 * @param n  Number of bytes to copy.
 * @return Newly allocated buffer (caller frees), or NULL on allocation
 *         failure.
 */
JSTR_ATTR_MALLOC
JSTR_FUNC
void *
jstr_memdup(const void *R s, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	void *p = malloc(n);
	return (jstr_likely(p != NULL)) ? memcpy(p, s, n) : NULL;
}
#else
;
#endif

/**
 * @brief Allocate a copy of the first @p n bytes of @p s with a NUL
 *        terminator.
 *
 * @param s  Source string; must be non-null when @p n > 0.
 * @param n  Number of bytes to copy.
 * @return Newly allocated NUL-terminated string (caller frees), or NULL on
 *         allocation failure.
 */
JSTR_ATTR_MALLOC
JSTR_FUNC
char *
jstr_strdup_len(const char *R s, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	char *p = (char *)malloc(n + 1);
	if (jstr_likely(p != NULL)) {
		jstr_stpcpy_len(p, s, n);
		return p;
	}
	JSTR_RETURN_ERR_P(NULL);
}
#else
;
#endif

/**
 * @brief Allocate a copy of the NUL-terminated string @p s.
 *
 * @param s  Source string; must be non-null.
 * @return Newly allocated NUL-terminated string (caller frees), or NULL on
 *         allocation failure.
 */
JSTR_ATTR_MALLOC
JSTR_FUNC
char *
jstr_strdup(const char *R s) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_strdup_len(s, strlen(s));
}
#else
;
#endif

#ifdef JSTR_IMPLEMENTATION
#	define JSTR_INTERNAL_DEFINE_ATOI(T, name, func) \
		JSTR_FUNC_PURE                   \
		T jstr_##name(const char *s) JSTR_NOEXCEPT { return func; }
#else
#	define JSTR_INTERNAL_DEFINE_ATOI(T, name, func) \
		JSTR_FUNC_PURE                   \
		T jstr_##name(const char *s) JSTR_NOEXCEPT;
#endif

/**
 * @brief Parse an unsigned int from @p s using base 0 (strtoul).
 * @param s NUL-terminated string; must be non-null.
 * @return Converted value, or 0 if parsing fails.
 */
JSTR_INTERNAL_DEFINE_ATOI(unsigned int, atou, strtoul(s, NULL, 0))
/**
 * @brief Parse an unsigned long from @p s using base 0 (strtoul).
 * @param s NUL-terminated string; must be non-null.
 * @return Converted value, or 0 if parsing fails.
 */
JSTR_INTERNAL_DEFINE_ATOI(unsigned long, atoul, strtoul(s, NULL, 0))
/**
 * @brief Parse an unsigned long long from @p s using base 0 (strtoull).
 * @param s NUL-terminated string; must be non-null.
 * @return Converted value, or 0 if parsing fails.
 */
JSTR_INTERNAL_DEFINE_ATOI(unsigned long long, atoull, strtoull(s, NULL, 0))
/**
 * @brief Parse an int from @p s using base 0 (strtol).
 * @param s NUL-terminated string; must be non-null.
 * @return Converted value, or 0 if parsing fails.
 */
JSTR_INTERNAL_DEFINE_ATOI(int, atoi, strtol(s, NULL, 0))
/**
 * @brief Parse a long from @p s using base 0 (strtol).
 * @param s NUL-terminated string; must be non-null.
 * @return Converted value, or 0 if parsing fails.
 */
JSTR_INTERNAL_DEFINE_ATOI(long, atol, strtol(s, NULL, 0))
/**
 * @brief Parse a long long from @p s using base 0 (strtol).
 * @param s NUL-terminated string; must be non-null.
 * @return Converted value, or 0 if parsing fails.
 */
JSTR_INTERNAL_DEFINE_ATOI(long long, atoll, strtol(s, NULL, 0))
/**
 * @brief Parse a double from @p s (strtod).
 * @param s NUL-terminated string; must be non-null.
 * @return Converted value, or 0 if parsing fails.
 */
JSTR_INTERNAL_DEFINE_ATOI(double, atod, strtod(s, NULL))
/**
 * @brief Parse a float from @p s (strtof).
 * @param s NUL-terminated string; must be non-null.
 * @return Converted value, or 0 if parsing fails.
 */
JSTR_INTERNAL_DEFINE_ATOI(float, atof, strtof(s, NULL))

#	undef JSTR_INTERNAL_DEFINE_ATOI

JSTR_INTERNAL_END_DECLS

#	undef R

#endif /* JSTR_STDSTRING_H */
