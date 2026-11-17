/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
{
		const char *save = "a,b,c,d";
		const char *end = save + 7;
		const char *tok = jstr_strtok_ne_len(&save, end, ",", 1);
		assert(tok != NULL && !strncmp(tok, "a", 1));
		tok = jstr_strtok_ne_len(&save, end, ",", 1);
		assert(tok != NULL && !strncmp(tok, "b", 1));
		tok = jstr_strtok_ne_len(&save, end, ",", 1);
		assert(tok != NULL && !strncmp(tok, "c", 1));
		tok = jstr_strtok_ne_len(&save, end, ",", 1);
		assert(tok != NULL && !strncmp(tok, "d", 1));
		tok = jstr_strtok_ne_len(&save, end, ",", 1);
		assert(tok == NULL);
	}
	return 0;
}
