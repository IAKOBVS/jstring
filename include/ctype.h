
#include "internal/musl/ctype.h"

#ifndef JSTR_CTYPE_H
#	define JSTR_CTYPE_H 1

#	include "macros.h"

#ifdef JSTR_IMPLEMENTATION
#	if defined __AVX512BW__ || defined __AVX2__ || defined __SSE2__
#		ifndef JSTR_HAVE_SIMD
#			define JSTR_HAVE_SIMD 1
#		endif
#		undef JSTR_IMPLEMENTATION
#		include "internal/simd.h"
#		define JSTR_IMPLEMENTATION 1
#	endif
#endif

#	define R JSTR_RESTRICT

JSTR_INTERNAL_BEGIN_DECLS

/**
 * @brief Convert every ASCII lowercase letter in the NUL-terminated string S to
 *        uppercase, in place.
 *
 * Scanning starts at S and stops at the first NUL byte, so embedded NUL
 * bytes terminate conversion. Only the 26 ASCII letters 'a'..'z' are
 * affected; bytes outside that range (including bytes with value >= 0x80
 * and all control characters) are stored unchanged. Conversion is ASCII-only
 * and does not depend on the current locale.
 *
 * @param[in,out] s NUL-terminated string to convert in place. Must not be
 *                  NULL.
 *
 * @return Pointer to the terminating NUL byte of S (one past the last
 *         converted character). For an empty string, returns S.
 *
 * @note S must refer to writable storage covering its full length plus the
 *       terminating NUL.
 * @note S and the result alias the same buffer; no separate destination is
 *       used.
 * @note Passing NULL is undefined behavior.
 */
JSTR_FUNC_VOID
char *
jstr_toupperstr_p(char *s) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_SIMD && defined CMPGT8 && defined CMPLT8
	return jstr_internal_simd_toupperstr_p(s);
#	else
	unsigned char *p = (unsigned char *)s;
	for (; (*p = jstr_toupper(*p)); ++p) {}
	return (char *)p;
#	endif
}
#else
;
#endif

/**
 * @brief Convert every ASCII uppercase letter in the NUL-terminated string S to
 *        lowercase, in place.
 *
 * Scanning starts at S and stops at the first NUL byte, so embedded NUL
 * bytes terminate conversion. Only the 26 ASCII letters 'A'..'Z' are
 * affected; bytes outside that range (including bytes with value >= 0x80
 * and all control characters) are stored unchanged. Conversion is ASCII-only
 * and does not depend on the current locale.
 *
 * @param[in,out] s NUL-terminated string to convert in place. Must not be
 *                  NULL.
 *
 * @return Pointer to the terminating NUL byte of S (one past the last
 *         converted character). For an empty string, returns S.
 *
 * @note S must refer to writable storage covering its full length plus the
 *       terminating NUL.
 * @note S and the result alias the same buffer; no separate destination is
 *       used.
 * @note Passing NULL is undefined behavior.
 */
JSTR_FUNC_VOID
char *
jstr_tolowerstr_p(char *s) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_SIMD && defined CMPGT8 && defined CMPLT8
	return jstr_internal_simd_tolowerstr_p(s);
#	else
	unsigned char *p = (unsigned char *)s;
	for (; (*p = jstr_tolower(*p)); ++p) {}
	return (char *)p;
#	endif
}
#else
;
#endif

/**
 * @brief Convert exactly N bytes of S to lowercase, in place.
 *
 * Unlike jstr_tolowerstr_p, this variant is length-driven rather than
 * NUL-driven: all N bytes are processed even if they contain embedded NUL
 * bytes, so the buffer need not be NUL-terminated. Only ASCII 'A'..'Z' are
 * changed. S must point to at least N writable bytes.
 *
 * @param[in,out] s Buffer to convert. Must not be NULL when N > 0; N == 0 is
 *                  a no-op that does not dereference S.
 * @param[in] n     Number of bytes to convert.
 *
 *
 * @note No NUL terminator is written; the caller is responsible for
 *       termination if required.
 * @note Passing S == NULL with N > 0 is undefined behavior.
 * @note Auto-vectorized at -O2 by GCC >= 15.1 and Clang >= 3.6.
 */
JSTR_FUNC_VOID
void
jstr_tolowerstr_len(char *s, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_SIMD && defined CMPGT8 && defined CMPLT8
	jstr_internal_simd_tolowerstr_len(s, n);
	return;
#	else
	unsigned char *p = (unsigned char *)s;
	for (; n--; ++p)
		*p = jstr_tolower(*p);
	return;
#	endif
}
#else
;
#endif

#	if (defined __GLIBC__ && JSTR_GLIBC_PREREQ(15, 1))
|| defined __clang__
#		define JSTR_HAVE_TOLOWER_AUTOVECTORIZATION 1
#		define JSTR_HAVE_TOUPPER_AUTOVECTORIZATION 1
#	else
#		define JSTR_HAVE_TOLOWER_AUTOVECTORIZATION 0
#		define JSTR_HAVE_TOUPPER_AUTOVECTORIZATION 0
#	endif

/**
 * @brief Copy N bytes from SRC to DST, converting ASCII uppercase letters to
 *        lowercase, then append a NUL terminator.
 *
 * Length-driven and binary-safe for the N copied bytes: embedded NULs in SRC
 * are copied and converted like any other byte. Only 'A'..'Z' are changed.
 * DST and SRC must not overlap (both are restrict-qualified). DST must have
 * room for at least N + 1 bytes.
 *
 * @param[out] dst Destination buffer. Must not be NULL when N > 0. Receives
 *                 the converted bytes followed by '\0' when N > 0.
 * @param[in] src  Source buffer. Must not be NULL when N > 0. Must provide
 *                 at least N readable bytes.
 * @param[in] n    Number of bytes to copy and convert.
 *
 *
 * @note When N == 0 the function performs no work and writes no terminator;
 *       DST is left unchanged. Callers needing termination must handle the
 *       empty case themselves.
 * @note ASCII-only; locale-independent.
 * @note Auto-vectorized at -O2 by GCC >= 15.1 and Clang >= 3.6.
 *
 */
JSTR_FUNC_VOID void
jstr_tolowerstrcpy_len(char *R dst, char *R src, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (n) {
		unsigned char *d = (unsigned char *)dst;
		const unsigned char *s = (unsigned char *)src;
		while (n--)
			*d++ = jstr_tolower(*s++);
		*d = '\0';
	}
}
#else
;
#endif

/**
 * @brief Convert exactly N bytes of S to uppercase, in place.
 *
 * Unlike jstr_toupperstr_p, this variant is length-driven rather than
 * NUL-driven: all N bytes are processed even if they contain embedded NUL
 * bytes, so the buffer need not be NUL-terminated. Only ASCII 'a'..'z' are
 * changed. S must point to at least N writable bytes.
 *
 * @param[in,out] s Buffer to convert. Must not be NULL when N > 0; N == 0 is
 *                  a no-op that does not dereference S.
 * @param[in] n     Number of bytes to convert.
 *
 *
 * @note No NUL terminator is written; the caller is responsible for
 *       termination if required.
 * @note Passing S == NULL with N > 0 is undefined behavior.
 * @note Auto-vectorized at -O2 by GCC >= 15.1 and Clang >= 3.6.
 */
JSTR_FUNC_VOID
void
jstr_toupperstr_len(char *s, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
#	if JSTR_HAVE_SIMD && defined CMPGT8 && defined CMPLT8
	jstr_internal_simd_toupperstr_len(s, n);
	return;
#	else
	unsigned char *p = (unsigned char *)s;
	for (; n--; ++p)
		*p = jstr_toupper(*p);
	return;
#	endif
}
#else
;
#endif

/**
 * @brief Copy N bytes from SRC to DST, converting ASCII lowercase letters to
 *        uppercase, then append a NUL terminator.
 *
 * Length-driven and binary-safe for the N copied bytes: embedded NULs in SRC
 * are copied and converted like any other byte. Only 'a'..'z' are changed.
 * DST and SRC must not overlap (both are restrict-qualified). DST must have
 * room for at least N + 1 bytes.
 *
 * @param[out] dst Destination buffer. Must not be NULL when N > 0. Receives
 *                 the converted bytes followed by '\0' when N > 0.
 * @param[in] src  Source buffer. Must not be NULL when N > 0. Must provide
 *                 at least N readable bytes.
 * @param[in] n    Number of bytes to copy and convert.
 *
 *
 * @note When N == 0 the function performs no work and writes no terminator;
 *       DST is left unchanged. Callers needing termination must handle the
 *       empty case themselves.
 * @note ASCII-only; locale-independent.
 * @note Auto-vectorized at -O2 by GCC >= 15.1 and Clang >= 3.6.
 */
JSTR_FUNC_VOID
void
jstr_toupperstrcpy_len(char *R dst, char *R src, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (n) {
		unsigned char *d = (unsigned char *)dst;
		const unsigned char *s = (unsigned char *)src;
		while (n--)
			*d++ = jstr_toupper(*s++);
		*d = '\0';
	}
}
#else
;
#endif

/**
 * @brief Copy SRC to DST, converting ASCII lowercase letters to uppercase, and
 *        NUL-terminate DST.
 *
 * SRC must be a NUL-terminated string; copying stops at its terminating NUL,
 * which is converted (toupper('\\0') == '\\0') and stored, terminating DST.
 * Only 'a'..'z' are changed. DST and SRC must not overlap.
 *
 * @param[out] dst Destination buffer. Must not be NULL and must have room for
 *                 at least strlen(SRC) + 1 bytes.
 * @param[in] src  NUL-terminated source string. Must not be NULL.
 *
 * @return Pointer to the terminating NUL byte written to DST. For an empty
 *         SRC, returns DST.
 *
 * @note ASCII-only; locale-independent.
 * @note If DST and SRC overlap, behavior is undefined.
 */
JSTR_FUNC
char *
jstr_toupperstrcpy_p(char *R dst, const char *R src) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	unsigned char *d = (unsigned char *)dst;
	const unsigned char *s = (const unsigned char *)src;
	while ((*d++ = jstr_toupper(*s++))) {}
	return (char *)d - 1;
}
#else
;
#endif

/**
 * @brief Copy SRC to DST, converting ASCII uppercase letters to lowercase, and
 *        NUL-terminate DST.
 *
 * SRC must be a NUL-terminated string; copying stops at its terminating NUL,
 * which is converted (tolower('\\0') == '\\0') and stored, terminating DST.
 * Only 'A'..'Z' are changed. DST and SRC must not overlap.
 *
 * @param[out] dst Destination buffer. Must not be NULL and must have room for
 *                 at least strlen(SRC) + 1 bytes.
 * @param[in] src  NUL-terminated source string. Must not be NULL.
 *
 * @return Pointer to the terminating NUL byte written to DST. For an empty
 *         SRC, returns DST.
 *
 * @note ASCII-only; locale-independent.
 * @note If DST and SRC overlap, behavior is undefined.
 */
JSTR_FUNC
char *
jstr_tolowerstrcpy_p(char *R dst, const char *R src) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	unsigned char *d = (unsigned char *)dst;
	const unsigned char *s = (const unsigned char *)src;
	while ((*d++ = jstr_tolower(*s++))) {}
	return (char *)d - 1;
}
#else
;
#endif

JSTR_INTERNAL_END_DECLS

#	undef JSTR_INTERNAL_DEFINE_REPEAT_CTYPE
#	undef R

#endif /* JSTR_CTYPE_H */
