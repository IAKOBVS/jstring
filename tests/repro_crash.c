#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "jstr.h"
#include "regex.h"

int main() {
    jstr_re_ty preg;
    jstr_re_comp(&preg, "a", JSTR_RE_CF_EXTENDED);

    char *buf = malloc(10);
    assert(buf);
    strcpy(buf, "abc");
    size_t sz = 3;
    size_t cap = 10;

    jstr_re_rm_exec(&preg, &buf, &sz, &cap, 0);
    assert(buf[sz] == '\0');

    free(buf);
    jstr_re_free(&preg);
    return 0;
}
