JSTR_FUNC_VOID
void
jstr_var_len(const char *s, size_t s_len, ...)
{
	(void)s;
	(void)s_len;
}

JSTR_FUNC_VOID
void
jstr_pp(const char **R s, size_t *sz)
{
	(void)s;
	(void)sz;
}

JSTR_FUNC_VOID
void
jstr_cb(void (*cb)(int), char *s, size_t *sz)
{
	(void)cb;
	(void)s;
	(void)sz;
}
