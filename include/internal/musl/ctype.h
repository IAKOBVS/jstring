
#ifndef JSTR_INTERNAL_MUSL_CTYPE_H
#	define JSTR_INTERNAL_MUSL_CTYPE_H 1

#	include "../../macros.h"

JSTR_INTERNAL_BEGIN_DECLS

#	define ISALPHA(c)  (((unsigned)c | 32) - 'a' < 26)
#	define ISDIGIT(c)  ((unsigned)c - '0' < 10)
#	define ISGRAPH(c)  ((unsigned)c - 0x21 < 0x5e)
#	define ISASCII(c)  (!(c & ~0x7f))
#	define ISBLANK(c)  ((c == ' ') | (c == '\t'))
#	define ISCNTRL(c)  ((unsigned)c < 0x20 || c == 0x7f)
#	define ISLOWER(c)  ((unsigned)c - 'a' < 26)
#	define ISUPPER(c)  ((unsigned)c - 'A' < 26)
#	define ISPRINT(c)  ((unsigned)c - 0x20 < 0x5f)
#	define ISALNUM(c)  (ISALPHA(c) | ISDIGIT(c))
#	define ISPUNCT(c)  (ISGRAPH(c) ^ ISALNUM(c))
#	define ISSPACE(c)  ((c == ' ') | ((unsigned)c - '\t' < 5))
#	define TOLOWER(c)  (c | (ISUPPER(c) ? 32 : 0))
#	define TOUPPER(c)  (c & (ISLOWER(c) ? 0x5f : (int)-1))
#	define ISXDIGIT(c) (ISDIGIT(c) | (((unsigned)c | 32) - 'a' < 6))

/**
 * @brief Test whether @p c is an alphabetic character.
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param c Input parameter.
 * @return Non-zero if @p c is an alphabetic character, 0 otherwise.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_ATTR_INLINE
JSTR_ATTR_CONST
static int
jstr_isalpha(int c)
JSTR_NOEXCEPT
{
	return ISALPHA((unsigned char)c);
}

/**
 * @brief Test whether @p c is a decimal digit.
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param c Input parameter.
 * @return Non-zero if @p c is a decimal digit, 0 otherwise.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_ATTR_INLINE
JSTR_ATTR_CONST
static int
jstr_isdigit(int c)
JSTR_NOEXCEPT
{
	return ISDIGIT((unsigned char)c);
}

/**
 * @brief Test whether @p c is alphanumeric.
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param c Input parameter.
 * @return Non-zero if @p c is an alphanumeric character, 0 otherwise.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_ATTR_INLINE
JSTR_ATTR_CONST
static int
jstr_isalnum(int c)
JSTR_NOEXCEPT
{
	return ISALNUM((unsigned char)c);
}

/**
 * @brief Test whether @p c is an ASCII character.
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param c Input parameter.
 * @return Non-zero if @p c is an ASCII character, 0 otherwise.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_ATTR_INLINE
JSTR_ATTR_CONST
static int
jstr_isascii(int c)
JSTR_NOEXCEPT
{
	return ISASCII((unsigned char)c);
}

/**
 * @brief Test whether @p c is a blank (space or tab).
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param c Input parameter.
 * @return Non-zero if @p c is a blank (space or tab), 0 otherwise.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_ATTR_INLINE
JSTR_ATTR_CONST
static int
jstr_isblank(int c)
JSTR_NOEXCEPT
{
	return ISBLANK((unsigned char)c);
}

/**
 * @brief Test whether @p c is a control character.
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param c Input parameter.
 * @return Non-zero if @p c is a control character, 0 otherwise.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_ATTR_INLINE
JSTR_ATTR_CONST
static int
jstr_iscntrl(int c)
JSTR_NOEXCEPT
{
	return ISCNTRL((unsigned char)c);
}

/**
 * @brief Test whether @p c is a graphic character.
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param c Input parameter.
 * @return Non-zero if @p c is a graphic character, 0 otherwise.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_ATTR_INLINE
JSTR_ATTR_CONST
static int
jstr_isgraph(int c)
JSTR_NOEXCEPT
{
	return ISGRAPH((unsigned char)c);
}

/**
 * @brief Test whether @p c is a lowercase letter.
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param c Input parameter.
 * @return Non-zero if @p c is a lowercase letter, 0 otherwise.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_ATTR_INLINE
JSTR_ATTR_CONST
static int
jstr_islower(int c)
JSTR_NOEXCEPT
{
	return ISLOWER((unsigned char)c);
}

/**
 * @brief Test whether @p c is an uppercase letter.
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param c Input parameter.
 * @return Non-zero if @p c is an uppercase letter, 0 otherwise.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_ATTR_INLINE
JSTR_ATTR_CONST
static int
jstr_isupper(int c)
JSTR_NOEXCEPT
{
	return ISUPPER((unsigned char)c);
}

/**
 * @brief Test whether @p c is printable.
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param c Input parameter.
 * @return Non-zero if @p c is a printable character, 0 otherwise.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_ATTR_INLINE
JSTR_ATTR_CONST
static int
jstr_isprint(int c)
JSTR_NOEXCEPT
{
	return ISPRINT((unsigned char)c);
}

/**
 * @brief Test whether @p c is punctuation.
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param c Input parameter.
 * @return Non-zero if @p c is a punctuation character, 0 otherwise.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_ATTR_INLINE
JSTR_ATTR_CONST
static int
jstr_ispunct(int c)
JSTR_NOEXCEPT
{
	return ISPUNCT((unsigned char)c);
}

/**
 * @brief Test whether @p c is whitespace.
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param c Input parameter.
 * @return Non-zero if @p c is a whitespace character, 0 otherwise.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_ATTR_INLINE
JSTR_ATTR_CONST
static int
jstr_isspace(int c)
JSTR_NOEXCEPT
{
	return ISSPACE((unsigned char)c);
}

/**
 * @brief Test whether @p c is a hexadecimal digit.
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param c Input parameter.
 * @return Non-zero if @p c is a hexadecimal digit, 0 otherwise.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_ATTR_INLINE
JSTR_ATTR_CONST
static int
jstr_isxdigit(int c)
JSTR_NOEXCEPT
{
	return ISXDIGIT((unsigned char)c);
}

/**
 * @brief Convert @p c to lowercase (ASCII).
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param c Input parameter.
 * @return Lowercase form of @p c.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_ATTR_INLINE
JSTR_ATTR_CONST
static int
jstr_tolower(int c)
JSTR_NOEXCEPT
{
	return TOLOWER((unsigned char)c);
}

/**
 * @brief Convert @p c to uppercase (ASCII).
 *
 * @internal Internal helper; not part of the public jstr API.
 *
 * @param c Input parameter.
 * @return Uppercase form of @p c.
 *
 * @note Pointer parameters must be non-NULL unless the length is 0; empty inputs and boundary lengths are handled by the implementation. This helper is internal and may change without notice.
 */
JSTR_ATTR_INLINE
JSTR_ATTR_CONST
static int
jstr_toupper(int c)
JSTR_NOEXCEPT
{
	return TOUPPER((unsigned char)c);
}

#	undef ISALPHA
#	undef ISDIGIT
#	undef ISGRAPH
#	undef ISASCII
#	undef ISBLANK
#	undef ISCNTRL
#	undef ISLOWER
#	undef ISUPPER
#	undef ISPRINT
#	undef ISALNUM
#	undef ISPUNCT
#	undef ISSPACE
#	undef TOLOWER
#	undef TOUPPER
#	undef ISXDIGIT

JSTR_INTERNAL_END_DECLS

#endif /* JSTR_INTERNAL_MUSL_CTYPE_H */
