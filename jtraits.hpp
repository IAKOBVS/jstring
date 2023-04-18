#include "macros.h"

typedef struct jstring_t jstring_t;

namespace jtraits {

template <typename Str>
JSTR_INLINE__
static constexpr int are_strings() JSTR_NOEXCEPT__
{
	return (std::is_same<const char *, std::decay_t<Str>>::value
		|| std::is_same<const char *&, std::decay_t<Str>>::value
		|| std::is_same<const char *&&, std::decay_t<Str>>::value
		|| std::is_same<char *, std::decay_t<Str>>::value
		|| std::is_same<char *&, std::decay_t<Str>>::value
		|| std::is_same<char *&&, std::decay_t<Str>>::value
		|| std::is_same<jstring_t *, std::decay_t<Str>>::value
		|| std::is_same<jstring_t *&, std::decay_t<Str>>::value
		|| std::is_same<jstring_t *&&, std::decay_t<Str>>::value
		|| std::is_same<jstring_t& , std::decay_t<Str>>::value
		|| std::is_same<jstring_t&&, std::decay_t<Str>>::value);
}

template <typename Str, typename... StrArgs, typename = typename std::enable_if<sizeof...(StrArgs) != 0>::type>
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
	static_assert(assert_are_strings<Str, StrArgs...>(), "Passing non-strings as string arguments!");
}

//

template <typename T, typename Arg>
JSTR_INLINE__
static constexpr int are_type() JSTR_NOEXCEPT__
{
	return std::is_same<T, std::decay_t<Arg>>::value;
}

template <typename T, typename Arg, typename... Args, typename = typename std::enable_if<sizeof...(Args) != 0>::type>
JSTR_INLINE__
static constexpr int are_type() JSTR_NOEXCEPT__
{
	return are_type<T, Arg>()
	&& are_type<T, Args...>();
}

template<typename T, typename Arg>
JSTR_INLINE__
static constexpr int are_type(Arg) JSTR_NOEXCEPT__
{
	return are_type<T, Arg>();
}

template<typename T, typename Arg, typename... Args>
JSTR_INLINE__
static constexpr int are_type(Arg, Args...) JSTR_NOEXCEPT__
{
	return are_type<T, Arg, Args...>();
}

}
