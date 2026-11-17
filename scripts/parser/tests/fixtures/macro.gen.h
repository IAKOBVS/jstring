#define MAKE(T) \
	JSTR_FUNC \
	static T foo_##T(T x) { return x; }

JSTR_FUNC
size_t
jstr_thing_len(const char *s, size_t s_len)
{
	return s_len;
}
