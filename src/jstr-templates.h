#ifndef JSTR_TEMPLATES_H
#define JSTR_TEMPLATES_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#ifdef __cplusplus
#	include <utility>
#	include "jtraits.h"
#endif /* __cpluslus */

#include "jstr-config.h"
#include "jstr-struct.h"

#define PJSTR_MALLOC_ERR(p, malloc_fail)                         \
	do {                                                     \
		if (jstr_unlikely((p) == NULL)) {                \
			pjstr_err(__FILE__, __LINE__, __func__); \
			malloc_fail;                             \
		}                                                \
	} while (0)
#define PJSTR_GROW(old_cap, new_cap)                                         \
	do {                                                                 \
		JSTR_ASSERT_IS_SIZE(old_cap);                                \
		JSTR_ASSERT_IS_SIZE(new_cap);                                \
		while ((old_cap *= JSTR_GROWTH) < (new_cap))                 \
			;                                                    \
		(old_cap) = PJSTR_ALIGN_UP(old_cap, PJSTR_MALLOC_ALIGNMENT); \
	} while (0)
#define PJSTR_REALLOC(p, old_cap, new_cap, malloc_fail) \
	do {                                            \
		JSTR_ASSERT_IS_STR(p);                  \
		JSTR_ASSERT_IS_SIZE(old_cap);           \
		JSTR_ASSERT_IS_SIZE(new_cap);           \
		PJSTR_GROW(old_cap, new_cap);           \
		(p) = (char *)realloc(p, old_cap);      \
		PJSTR_MALLOC_ERR(p, malloc_fail);       \
	} while (0)
#define PJSTR_REALLOCEXACT(p, old_cap, new_cap, malloc_fail) \
	do {                                                 \
		JSTR_ASSERT_IS_STR(p);                       \
		JSTR_ASSERT_IS_SIZE(old_cap);                \
		JSTR_ASSERT_IS_SIZE(new_cap);                \
		(old_cap) = PJSTR_ALIGN_UP_STR(new_cap);     \
		(p) = (char *)realloc(p, old_cap);           \
		PJSTR_MALLOC_ERR(p, malloc_fail);            \
	} while (0)
#define PJSTR_MIN_ALLOC(new_cap)                           \
	((new_cap < PJSTR_MIN_CAP / JSTR_ALLOC_MULTIPLIER) \
	 ? (PJSTR_MIN_CAP)                                 \
	 : (new_cap * JSTR_ALLOC_MULTIPLIER))
#define PJSTR_MIN_ALLOCEXACT(new_cap) \
	((new_cap < PJSTR_MIN_CAP)    \
	 ? (PJSTR_MIN_CAP)            \
	 : (new_cap))
#define PJSTR_ALLOC_ONLY(p, cap, new_cap, do_fail) \
	do {                                       \
		(cap) = PJSTR_MIN_ALLOC(new_cap);  \
		(p) = (char *)malloc((cap));       \
		PJSTR_MALLOC_ERR((p), do_fail);    \
	} while (0)

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

JSTR_MAYBE_UNUSED
JSTR_NOINLINE
JSTR_NOTHROW
static void
pjstr_err(const char *R const FILE_,
	  const int LINE_,
	  const char *R const func_) JSTR_NOEXCEPT
{
#if JSTR_ERR_MSG_ON_MALLOC_ERROR
	fprintf(stderr, "%s:%d:%s\n:Can't malloc:", FILE_, LINE_, func_);
	perror("");
#endif
#if JSTR_EXIT_ON_MALLOC_ERROR
	exit(1);
#endif
}

JSTR_MAYBE_UNUSED
JSTR_NOINLINE
JSTR_NOTHROW
static void
pjstr_err_exit(void) JSTR_NOEXCEPT
{
#if JSTR_ERR_MSG_ON_MALLOC_ERROR
	fprintf(stderr, "%s:%d:%s\n:Can't malloc:", __FILE__, __LINE__, __func__);
	perror("");
#endif
	exit(1);
}

PJSTR_END_DECLS

#ifdef __cplusplus

namespace jstr {

namespace priv {

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_PURE
JSTR_WARN_UNUSED
JSTR_NOTHROW
static constexpr size_t
strlen_args() JSTR_NOEXCEPT
{
	return 0;
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_WARN_UNUSED
JSTR_PURE
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static size_t
strlen_args(Str &&s,
	    StrArgs &&...args) JSTR_NOEXCEPT
{
	return strlen(std::forward<Str>(s))
	       + strlen_args(std::forward<StrArgs>(args)...);
}

template <size_t N>
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static void
appendmore_assign(size_t *R sz,
		  char **dst,
		  const char (&src)[N]) JSTR_NOEXCEPT
{
	memcpy(*dst, src, N - 1);
	*dst += N - 1;
	*sz += N - 1;
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
appendmore_assign(size_t *sz,
		  char **dst,
		  const char *R src) JSTR_NOEXCEPT
{
#	if JSTR_HAVE_STPCPY
	char *const tmp = stpcpy(*dst, src);
	*sz += tmp - *dst;
	*dst = tmp;
#	else
	const size_t len = strlen(*dst);
	memcpy(*dst, src, len);
	*dst += len;
#	endif
}

JSTR_INLINE
JSTR_NOTHROW
static constexpr void
appendmore_loop_assign(size_t *, char **) JSTR_NOEXCEPT
{
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static void
appendmore_loop_assign(size_t *sz,
		       char **dst,
		       Str &&_arg,
		       StrArgs &&...args) JSTR_NOEXCEPT
{
	appendmore_assign(sz, dst, std::forward<Str>(_arg));
	appendmore_loop_assign(sz, dst, std::forward<StrArgs>(args)...);
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static void
appendmore_loop_assign(char **dst,
		       Str &&_arg,
		       StrArgs &&...args) JSTR_NOEXCEPT
{
	appendmore_assign(dst, std::forward<Str>(_arg));
	appendmore_loop_assign(dst, std::forward<StrArgs>(args)...);
}

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_PURE
JSTR_WARN_UNUSED
JSTR_NOTHROW
static constexpr size_t
strlen_args(size_t *) JSTR_NOEXCEPT
{
	return 0;
}

template <typename Str,
	  typename = typename std::enable_if<jtraits_are_strings<Str>(), int>::type>
JSTR_WARN_UNUSED
JSTR_PURE
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static size_t
strlen(size_t *R *R strlen_arr,
       Str &&_arg) JSTR_NOEXCEPT
{
	return ((*(*strlen_arr)++) = ::strlen(std::forward<Str>(_arg)));
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_WARN_UNUSED
JSTR_PURE
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static size_t
strlen_args(size_t *strlen_arr,
	    Str &&s,
	    StrArgs &&...args) JSTR_NOEXCEPT
{
	return strlen(&strlen_arr, std::forward<Str>(s))
	       + strlen_args(strlen_arr, std::forward<StrArgs>(args)...);
}

template <size_t N>
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static void
appendmore_assign(char *R *R const dst,
		  size_t *R *R strlen_arr,
		  const char (&src)[N]) JSTR_NOEXCEPT
{
	memcpy(*dst, src, N - 1);
	*dst += N - 1;
	++*strlen_arr;
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
appendmore_assign(char *R *R dst,
		  size_t *R *R strlen_arr,
		  const char *R src) JSTR_NOEXCEPT
{
	memcpy(*dst, src, *(*strlen_arr));
	*dst += *(*strlen_arr)++;
}

JSTR_INLINE
JSTR_NOTHROW
static constexpr void
appendmore_loop_assign(char **, size_t *) JSTR_NOEXCEPT
{
}

template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static void
appendmore_loop_assign(char **dst,
		       size_t *strlen_arr,
		       Str &&_arg,
		       StrArgs &&...args) JSTR_NOEXCEPT
{
	appendmore_assign(dst, &strlen_arr, std::forward<Str>(_arg));
	appendmore_loop_assign(dst, strlen_arr, std::forward<StrArgs>(args)...);
}

} // namespace priv

} // namespace jstr

/*
   Insert multiple strings to S.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static void
jstr_alloc_appendmore(char *R *R const s,
		      size_t *const sz,
		      size_t *const cap,
		      Str &&_arg,
		      StrArgs &&...args) JSTR_NOEXCEPT
{
	size_t strlen_arr[1 + sizeof...(args)];
	*sz = jstr::priv::strlen_args(strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(args)...);
	*cap = *sz * 2;
	*s = (char *)malloc(*cap);
	PJSTR_MALLOC_ERR(*s, return);
	char *p = *s;
	jstr::priv::appendmore_loop_assign(&p, strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(args)...);
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
JSTR_NONNULL_ALL JSTR_NOTHROW static void
jstr_alloc_appendmore_f(char *R const s,
			size_t *const sz,
			Str &&_arg,
			StrArgs &&...args) JSTR_NOEXCEPT
{
	size_t strlen_arr[1 + sizeof...(args)];
	*sz = jstr::priv::strlen_args(strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(args)...);
	jstr::priv::appendmore_loop_assign(&s, strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(args)...);
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
JSTR_NONNULL_ALL JSTR_NOTHROW static void
jstr_appendmore(char *R *R const s,
		size_t *const sz,
		size_t *const cap,
		Str &&_arg,
		StrArgs &&...args) JSTR_NOEXCEPT
{
	size_t strlen_arr[1 + sizeof...(args)];
	const size_t newsz = *sz + jstr::priv::strlen_args(strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(args)...);
	if (*cap < *sz)
		PJSTR_REALLOC(*s, *cap, newsz + 1, return);
	char *p = *s + *sz;
	jstr::priv::appendmore_loop_assign(&p, strlen_arr, std::forward<Str>(_arg), std::forward<StrArgs>(args)...);
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
JSTR_NONNULL_ALL JSTR_NOTHROW static void
jstr_appendmore_f(char *s,
		  size_t *R sz,
		  Str &&_arg,
		  StrArgs &&...args) JSTR_NOEXCEPT
{
	s += *sz;
	jstr::priv::appendmore_loop_assign(sz, &s, std::forward<Str>(_arg), std::forward<StrArgs>(args)...);
	*s = '\0';
}

/*
  Insert multiple strings to S.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static void
jstr_alloc_appendmore_j(jstr_ty *R const j,
			Str &&arg,
			StrArgs &&...args) JSTR_NOEXCEPT
{
	jstr_alloc_appendmore(&j->data, &j->size, &j->capacity, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
}

/*
  Append multiple strings to end of S.
*/
template <typename Str,
	  typename... StrArgs,
	  typename = typename std::enable_if<jtraits_are_strings<Str, StrArgs...>(), int>::type>
JSTR_INLINE
JSTR_NONNULL_ALL JSTR_NOTHROW static void
jstr_appendmore_j(jstr_ty *R const j,
		  Str &&arg,
		  StrArgs &&...args) JSTR_NOEXCEPT
{
	jstr_appendmore(&j->data, &j->size, &j->capacity, std::forward<Str>(arg), std::forward<StrArgs>(args)...);
}

#endif /* __cplusplus */

#undef R

#endif /* JSTR_TMPLATES_H */
