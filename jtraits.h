#ifndef JTRAITS_H_DEF
#define JTRAITS_H_DEF 1

#ifdef __cplusplus

#	include "macros.h"
#	include <utility>

template <typename T, typename Arg>
JSTR_INLINE JSTR_NOTHROW static constexpr int
jtraits_is_same_decay() JSTR_NOEXCEPT
{
	using namespace std;
	return is_same<T, typename decay<Arg>::type>::value;
}

template <typename T, typename Arg>
JSTR_INLINE JSTR_NOTHROW static constexpr int
jtraits_is_same_decay(Arg) JSTR_NOEXCEPT
{
	return jtraits_is_same_decay<T, Arg>();
}

template <typename Str>
JSTR_INLINE JSTR_NOTHROW static constexpr int
jtraits_are_strings() JSTR_NOEXCEPT
{
	using namespace std;
	return (jtraits_is_same_decay<const char *, Str>()
		|| jtraits_is_same_decay<char *, Str>());
}

template <typename Str, typename... StrArgs, typename = typename std::enable_if<sizeof...(StrArgs) != 0>::type>
JSTR_INLINE JSTR_NOTHROW static constexpr int
jtraits_are_strings() JSTR_NOEXCEPT
{
	return jtraits_are_strings<Str>()
	       && jtraits_are_strings<StrArgs...>();
}

template <typename Str>
JSTR_INLINE JSTR_NOTHROW static constexpr int
jtraits_are_strings(Str) JSTR_NOEXCEPT
{
	return jtraits_are_strings<Str>();
}

template <typename Str, typename... StrArgs>
JSTR_INLINE JSTR_NOTHROW static constexpr int
jtraits_are_strings(Str, StrArgs...) JSTR_NOEXCEPT
{
	return jtraits_are_strings<Str, StrArgs...>();
}

template <typename Str, typename... StrArgs>
JSTR_INLINE JSTR_NOTHROW static constexpr void
jtraits_assert_jtraits_are_strings(Str &&, StrArgs &&...) JSTR_NOEXCEPT
{
	static_assert(jtraits_are_strings<Str, StrArgs...>(), "Passing non-strings as string arguments!");
}

template <typename T, typename Arg>
JSTR_INLINE JSTR_NOTHROW static constexpr int
jtraits_are_same() JSTR_NOEXCEPT
{
	return std::is_same<T, Arg>::value;
}

template <typename T, typename Arg, typename... Args, typename = typename std::enable_if<sizeof...(Args) != 0>::type>
JSTR_INLINE JSTR_NOTHROW static constexpr int
jtraits_are_same() JSTR_NOEXCEPT
{
	return jtraits_are_same<T, Arg>()
	       && jtraits_are_same<T, Args...>();
}

template <typename T, typename Arg>
JSTR_INLINE JSTR_NOTHROW static constexpr int
jtraits_are_same(Arg) JSTR_NOEXCEPT
{
	return jtraits_are_same<T, Arg>();
}

template <typename T, typename Arg, typename... Args>
JSTR_INLINE JSTR_NOTHROW static constexpr int
jtraits_are_same(Arg, Args...) JSTR_NOEXCEPT
{
	return jtraits_are_same<T, Arg, Args...>();
}

template <typename T, typename Arg, typename... Args>
JSTR_INLINE JSTR_NOTHROW static constexpr void
jtraits_assert_jtraits_are_same(Arg &&, Args &&...) JSTR_NOEXCEPT
{
	static_assert(jtraits_are_same<T, Arg, Args...>(), "Passing non-T as T arguments!");
}

#endif /* __cplusplus */

#endif /* JTRAITS_H_DEF */
