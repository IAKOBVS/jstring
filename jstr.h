#ifndef STR_H_DEF
#define STR_H_DEF
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
int emptyStruct(jstr *structPtr);
#endif
