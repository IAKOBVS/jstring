#include <stdio.h>
#include <string.h>
#include "../include/regex.h"

int main(void) {
    jstr_ty s = JSTR_INIT;
    jstr_re_ty preg;
    if (jstr_re_comp(&preg, "colou?r", JSTR_RE_CF_EXTENDED) != JSTR_RE_RET_NOERROR) {
        fprintf(stderr, "regcomp failed\n");
        return 2;
    }
    /* initialize s to "color colour" */
    if (jstr_assign_len(jstr_struct(&s), "color colour", strlen("color colour"))) {
        fprintf(stderr, "assign failed\n");
        return 2;
    }
    jstr_re_off_ty n = jstr_re_rplcall_len_exec(&preg, jstr_struct(&s), "X", 1, 0);
    printf("ret=%jd size=%zu cap=%zu data='%s'\n", (intmax_t)n, s.size, s.capacity, s.data);
    jstr_free_j(&s);
    jstr_re_free(&preg);
    return 0;
}
