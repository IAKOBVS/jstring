#ifndef STR_H_DEF
#define STR_H_DEF
struct ptrSize {
	int size;
	int len;
};

int addStrings(struct ptrSize *ptrInfo, char **dest, ...);
int addStr(char **dest, int destLen, char *src, int srcLen, struct ptrSize *ptrInfo);
#endif
