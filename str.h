#ifndef STR_H_DEF
#define STR_H_DEF
struct sizeOfPtr {
	void *ptr;
	int size;
};

/* int joinStr(char **outStr, ..., "\0") */
int joinStr(char **outStr, ...);
int joinStrLarge(char **outStr, ...);
int joinStrProto(char **outStr, ...);
#endif
