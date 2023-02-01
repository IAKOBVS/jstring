#ifndef JSTR_H_DEF
#define JSTR_H_DEF

#define newJstr(_NAME_OF_STRING, _STRING) jstr _NAME_OF_STRING = {.str = _STRING, .size = 0}

typedef struct jstr {
	char *str;
	int size;
	int len;
} jstr;

/* end with "\0" or "" since the \0 is implicit */
int catJstr(struct jstr *dest, ...);
int addJstr(jstr *dest, jstr *src);
int addStr(jstr *dest, char *src);
int areDigits(char* src);
int isJstr(jstr *structPtr);
#endif
