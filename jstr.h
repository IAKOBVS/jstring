#ifndef STR_H_DEF
#define STR_H_DEF
typedef struct jstr {
	char *str;
	int size;
	int len;
} jstr;

/* end with "\0" or "" since the \0 is implicit */
int catStr(struct jstr *dest, ...);
#endif
