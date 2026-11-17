JSTR_FUNC_VOID
void
jstr_braces(char *s, size_t *sz)
{
	const char *x = "}";
	/* } */
	if (x[0] == '{') { *sz = 0; }
}
