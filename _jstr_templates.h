#ifndef JSTR_TEMPLATES_DEF_H
#define JSTR_TEMPLATES_DEF_H

#ifdef _cplusplus
extern "C" {
#endif /* _cplusplus */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _cplusplus
}
#endif /* _cplusplus */

#ifdef _cplusplus
#	include <utility>
#endif /* _cpluslus */

#include "_jstr_config.h"
#include "_jstr_macros.h"
#include "_jstr_traits.h"

#define JSTR_ERR                                                                               \
	do {                                                                                     \
		if (JSTR_PRINT_ERR_MSG_ON_MALLOC_ERROR) {                                        \
			fprintf(stderr, "%s:%d:%s:Can't malloc:", __FILE__, __LINE__, __func__); \
			perror("");                                                              \
		}                                                                                \
		if (JSTR_EXIT_ON_MALLOC_ERROR)                                                   \
			exit(1);                                                                 \
	} while (0)

#define JSTR_MALLOC_ERR(p, malloc_fail) \
	do {                            \
		if (jstr_unlikely(!(p))) {   \
			JSTR_ERR;       \
			malloc_fail;    \
		}                       \
	} while (0)
#define JSTR_GROW(old_cap, new_cap)                                       \
	do {                                                              \
		JSTR_ASSERT_IS_SIZE(old_cap);                             \
		JSTR_ASSERT_IS_SIZE(new_cap);                             \
		while (((old_cap) *= JSTR_GROWTH_MULTIPLIER) < (new_cap)) \
			;                                                 \
	} while (0)
#define JSTR_REALLOC(p, old_cap, new_cap, malloc_fail) \
	do {                                           \
		JSTR_ASSERT_IS_STR(p);                 \
		JSTR_ASSERT_IS_SIZE(old_cap);          \
		JSTR_ASSERT_IS_SIZE(new_cap);          \
		JSTR_GROW(old_cap, new_cap);           \
		(p) = (char *)realloc(p, old_cap);     \
		JSTR_MALLOC_ERR(p, malloc_fail);       \
	} while (0)

#if JSTR_HAVE_REALLOC_MREMAP
#	define JSTR_IS_MMAP(cap) (((cap) > JSTR_MIN_MMAP) ? 1 : 0)
#endif /* JSTR_HAVE_REALLOC_MREMAP */

#ifdef __cplusplus

namespace jstr {

namespace _private {

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
static constexpr size_t
strlen_args() JSTR_NOEXCEPT
{
	return 0;
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_WARN_UNUSED
JSTR_CONST
JSTR_INLINE
JSTR_NONNULL_ALL static size_t
strlen_args(Str &&s,
	    StrArgs &&...args) JSTR_NOEXCEPT
{
	return strlen(std::forward<Str>(s))
	       + strlen_args(std::forward<StrArgs>(args)...);
}

template <size_t N>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_assign(size_t *JSTR_RST sz,
	   char **dst,
	   const char (&src)[N]) JSTR_NOEXCEPT
{
	memcpy(*dst, src, N - 1);
	*dst += (N - 1);
	*sz += (N - 1);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void
cat_assign(size_t *sz,
	   char **dst,
	   const char *JSTR_RST src) JSTR_NOEXCEPT
{
#	if JSTR_HAVE_STPCPY
	char *const _new = stpcpy(*dst, src);
	*sz += _new - *dst;
	*dst = _new;
#	else
	const size_t len = strlen(*dst);
	memcpy(*dst, src, len);
	*dst += len;
#	endif
}

JSTR_INLINE
static constexpr void
cat_loop_assign(size_t *, char **) JSTR_NOEXCEPT
{
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_loop_assign(size_t *sz,
		char **dst,
		Str &&arg,
		StrArgs &&...args) JSTR_NOEXCEPT
{
	cat_assign(sz, dst, std::forward<Str>(arg));
	cat_loop_assign(sz, dst, std::forward<StrArgs>(args)...);
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_loop_assign(char **dst,
		Str &&arg,
		StrArgs &&...args) JSTR_NOEXCEPT
{
	cat_assign(dst, std::forward<Str>(arg));
	cat_loop_assign(dst, std::forward<StrArgs>(args)...);
}

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
static constexpr size_t
strlen_args(size_t *) JSTR_NOEXCEPT
{
	return 0;
}

template <typename Str,
	  typename = typename std::enable_if<jtraits_are_strings<Str>(), int>::type>
JSTR_WARN_UNUSED
JSTR_CONST
JSTR_INLINE
JSTR_NONNULL_ALL static size_t
strlen(size_t **JSTR_RST strlen_arr,
       Str &&arg) JSTR_NOEXCEPT
{
	return ((*(*strlen_arr)++) = ::strlen(std::forward<Str>(arg)));
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_WARN_UNUSED
JSTR_CONST
JSTR_INLINE
JSTR_NONNULL_ALL static size_t
strlen_args(size_t *strlen_arr,
	    Str &&s,
	    StrArgs &&...args) JSTR_NOEXCEPT
{
	return strlen(&strlen_arr, std::forward<Str>(s))
	       + strlen_args(strlen_arr, std::forward<StrArgs>(args)...);
}

template <size_t N>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_assign(char **JSTR_RST const dst,
	   size_t **JSTR_RST strlen_arr,
	   const char (&src)[N]) JSTR_NOEXCEPT
{
	memcpy(*dst, src, N - 1);
	*dst += (N - 1);
	++*strlen_arr;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void
cat_assign(char **JSTR_RST dst,
	   size_t **JSTR_RST strlen_arr,
	   const char *JSTR_RST src) JSTR_NOEXCEPT
{
	memcpy(*dst, src, *(*strlen_arr));
	*dst += *(*strlen_arr)++;
}

JSTR_INLINE
static constexpr void
cat_loop_assign(char **, size_t *) JSTR_NOEXCEPT
{
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
cat_loop_assign(char **dst,
		size_t *strlen_arr,
		Str &&arg,
		StrArgs &&...args) JSTR_NOEXCEPT
{
	cat_assign(dst, &strlen_arr, std::forward<Str>(arg));
	cat_loop_assign(dst, strlen_arr, std::forward<StrArgs>(args)...);
}

} // namespace _private

} // namespace jstr

/*
   Insert multiple strings to S.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
jstr_alloc_cat(char **JSTR_RST const s,
	       size_t *const sz,
	       size_t *const cap,
	       Str &&arg,
	       StrArgs &&...args) JSTR_NOEXCEPT
{
	size_t strlen_arr[1 + sizeof...(args)];
	*sz = jstr::_private::strlen_args(strlen_arr, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	*cap = *sz * 2;
	*s = (char *)malloc(*cap);
	JSTR_MALLOC_ERR(*s, return);
	char *p = *s;
	jstr::_private::cat_loop_assign(&p, strlen_arr, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	*p = '\0';
}

/*
   Insert multiple strings to S.
   Assumes that S have enough space.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
jstr_alloc_cat_f(char *JSTR_RST const s,
		 size_t *const sz,
		 Str &&arg,
		 StrArgs &&...args) JSTR_NOEXCEPT
{
	size_t strlen_arr[1 + sizeof...(args)];
#	if 0
	*sz = jstr::_private::strlen_args(std::forward<Str>(arg), std::forward<StrArgs>(args)...);
#	else
	*sz = jstr::_private::strlen_args(strlen_arr, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
#	endif
	jstr::_private::cat_loop_assign(&s, strlen_arr, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	*s = '\0';
}

/*
   Append multiple strings to end of S.
   Assumes that S have enough space.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
jstr_cat(char **JSTR_RST const s,
	 size_t *const sz,
	 size_t *const cap,
	 Str &&arg,
	 StrArgs &&...args) JSTR_NOEXCEPT
{
	size_t strlen_arr[1 + sizeof...(args)];
#	if 0
	const size_t newsz = *sz + jstr::_private::strlen_args(std::forward<Str>(arg), std::forward<StrArgs>(args)...);
#	else
	const size_t newsz = *sz + jstr::_private::strlen_args(strlen_arr, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
#	endif
	if (*cap < *sz)
		JSTR_REALLOC(*s, *cap, newsz + 1, return);
	char *p = *s + *sz;
#	if 0
	jstr::_private::cat_loop_assign(&p, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
#	else
	jstr::_private::cat_loop_assign(&p, strlen_arr, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
#	endif
	*p = '\0';
	*sz = newsz;
}

/*
   Append multiple strings to end of S.
   Assumes that S have enough space.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL static void
jstr_cat_f(char *s,
	   size_t *JSTR_RST sz,
	   Str &&arg,
	   StrArgs &&...args) JSTR_NOEXCEPT
{
	s += *sz;
	jstr::_private::cat_loop_assign(sz, &s, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
	*s = '\0';
}

#endif /* _cplusplus */

#endif /* JSTR_TMPLATES_DEF_H */
