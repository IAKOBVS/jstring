#ifndef JTRAITS_DEF_H__
#define JTRAITS_DEF_H__

#include "macros.h"

typedef struct jstring_t jstring_t;

namespace jstd {

template <typename T, typename Arg>
JSTR_INLINE__
static constexpr int is_same_decay() JSTR_NOEXCEPT__
{
	using namespace std;
	return is_same<T, typename decay<Arg>::type>::value;
}

template <typename Str>
JSTR_INLINE__
static constexpr int are_strings() JSTR_NOEXCEPT__
{
	return (is_same_decay<const char *, Str>()
		|| is_same_decay<char *, Str>()
		|| is_same_decay<jstring_t *, Str>()
		|| std::is_same<jstring_t& , Str>::value
		|| std::is_same<jstring_t&&, Str>::value);
}

template <typename Str, typename... StrArgs,
	 typename = typename std::enable_if<sizeof...(StrArgs) != 0>::type>
JSTR_INLINE__
static constexpr int are_strings() JSTR_NOEXCEPT__
{
	return are_strings<Str>()
	&& are_strings<StrArgs...>();
}

template<typename Str>
JSTR_INLINE__
static constexpr int are_strings(Str) JSTR_NOEXCEPT__
{
	return are_strings<Str>();
}

template<typename Str, typename... StrArgs>
JSTR_INLINE__
static constexpr int are_strings(Str, StrArgs...) JSTR_NOEXCEPT__
{
	return are_strings<Str, StrArgs...>();
}

template <typename Str, typename... StrArgs>
JSTR_INLINE__
static constexpr void assert_are_strings(Str&&, StrArgs&&...) JSTR_NOEXCEPT__
{
	static_assert(are_strings<Str, StrArgs...>(), "Passing non-strings as string arguments!");
}

//

template <typename T, typename Arg>
JSTR_INLINE__
static constexpr int are_same() JSTR_NOEXCEPT__
{
	using namespace std;
	return is_same<T, Arg>::value;
}

template <typename T, typename Arg, typename... Args,
	 typename = typename std::enable_if<sizeof...(Args) != 0>::type>
JSTR_INLINE__
static constexpr int are_same() JSTR_NOEXCEPT__
{
	return are_same<T, Arg>()
	&& are_same<T, Args...>();
}

template<typename T, typename Arg>
JSTR_INLINE__
static constexpr int are_same(Arg) JSTR_NOEXCEPT__
{
	return are_same<T, Arg>();
}

template<typename T, typename Arg, typename... Args>
JSTR_INLINE__
static constexpr int are_same(Arg, Args...) JSTR_NOEXCEPT__
{
	return are_same<T, Arg, Args...>();
}

template <typename T, typename Arg, typename... Args>
JSTR_INLINE__
static constexpr void assert_are_same(Arg&&, Args&&...) JSTR_NOEXCEPT__
{
	static_assert(are_same<T, Arg, Args...>(), "Passing non-T as T arguments!");
}

}

#endif // JTRAITS_DEF_H__
