#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "jstr.h"

#define newJstr(_NAME_OF_STRING, _STRING) jstr _NAME_OF_STRING = {.str = _STRING, .size = 0}

/* end with "\0" or "" since the \0 is implicit */
/* addStrings(&struct, &dest, ..., "") */
int catJstr(jstr *dest, ...)
{
	/* *dest->size must be initialized with 0 if empty */
	va_list ap;
	va_start(ap, dest);
	int argLen=0;
	for (;;) {
		char *argvStr = va_arg(ap, char*);
		if (!argvStr[0])
			break;
		argLen += strlen(argvStr);
	}
	va_end(ap);
	if (!argLen)
		goto ERR;
	if (!dest->len)
		dest->len = strlen(dest->str);
	int i = dest->len;
	dest->len += argLen;
	if (!dest->size) {
		char *tmp = dest->str;
		if (!(dest->str = malloc(dest->size)))
			goto ERR;
		memcpy(dest->str, tmp, dest->len);
	} else if (dest->size < 2 * dest->len) {
		dest->size = 2 * dest->len;
		if (!(dest->str = realloc(dest->str, dest->size)))
			goto ERR;
	}
	va_start(ap, dest);
	for (;;) {
		char *argvStr = va_arg(ap, char*);
		if (!argvStr[0])
			break;
		do {
			(dest->str)[i++] = *argvStr++;
		} while (*argvStr);
	}
	va_end(ap);
	(dest->str)[dest->len + 1] = '\0';
	return dest->size;

ERR:
	perror("catJstr(char *dest->string, ...): ");
	return 0;
}

int addJstr(jstr *dest, jstr *src)
{
	/* *dest->size must be initialized with 0 if empty */
	if ((!dest->len && !(dest->len = strlen(dest->str))) || (!src->len && !(src->len = strlen(src->str))))
		goto ERR;
	if (!dest->size) {
		char *tmp = dest->str;
		dest->size = 2 * (dest->len + src->len);
		if (!(dest->str = malloc(dest->size)))
			goto ERR;
		memcpy(dest->str, tmp, dest->size);
	} else if (dest->size < 2 * dest->len) {
		if (!(dest->str = realloc(dest->str, dest->size)))
			goto ERR;
	}
	int i = dest->len;
	int j = 0;
	do {
		(dest->str)[i++] = (src->str)[j++];
	} while (j < src->len);
	dest->len += src->len;
	(dest->str)[dest->len + 1] = '\0';
	return dest->size;

ERR:
	perror("int addjtr(jstr *dest, jstr *src): ");
	return 0;
}

int addStr(jstr *dest, char *src)
{
	/* *dest->size must be initialized with 0 if empty */
	int srcLen;
	if ((!dest->len && !(dest->len = strlen(dest->str))) || (!(srcLen = strlen(src))))
		goto ERR;
	if (!dest->size) {
		char *tmp = dest->str;
		dest->size = 2 * (dest->len + srcLen);
		if (!(dest->str = malloc(dest->size)))
			goto ERR;
		memcpy(dest->str, tmp, dest->size);
	} else if (dest->size < 2 * dest->len) {
		if (!(dest->str = realloc(dest->str, dest->size)))
			goto ERR;
	}
	int i = dest->len;
	int j = 0;
	do {
		(dest->str)[i++] = (src)[j++];
	} while (j < srcLen);
	dest->len += srcLen;
	(dest->str)[dest->len + 1] = '\0';
	return dest->size;

ERR:
	perror("int addStr(jstr *dest, char *src): ");
	return 0;
}

int areDigits(char* src)
{
	while (*src)
		switch (*src++){
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			continue;
		default:
			return 0;
		}
	return 1;
}

int isJstr(jstr *structPtr)
{
	if (!*((unsigned char *)&*structPtr))
		return 0;
	return 1;
}
