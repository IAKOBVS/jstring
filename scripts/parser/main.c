#include "jstrgen.h"

#include <stdio.h>
#include <string.h>

static void
jp_usage(const char *prog)
{
	fprintf(stderr, "usage: %s <gen-func|namespace> <file>\n", prog);
}

int
main(int argc, char **argv)
{
	jp_buf_ty in;
	jp_buf_ty out;
	int rc = 0;

	if (argc == 2 && strcmp(argv[1], "namespace") == 0) {
		char tmp[8192];

		jp_buf_init(&in);
		for (;;) {
			size_t n = fread(tmp, 1, sizeof(tmp), stdin);
			if (n && jp_buf_append(&in, tmp, n) != 0) {
				jp_buf_free(&in);
				return 2;
			}
			if (n < sizeof(tmp))
				break;
		}
	} else if (argc == 3) {
		if (jp_read_file(argv[2], &in) != 0) {
			fprintf(stderr, "%s: cannot read %s\n", argv[0], argv[2]);
			return 2;
		}
	} else {
		jp_usage(argv[0]);
		return 2;
	}
	if (strcmp(argv[1], "gen-func") == 0)
		rc = jp_generate(in.data ? in.data : "", in.len, &out);
	else if (strcmp(argv[1], "namespace") == 0)
		rc = jp_namespace(in.data ? in.data : "", in.len, &out);
	else {
		jp_usage(argv[0]);
		jp_buf_free(&in);
		return 2;
	}
	if (rc != 0) {
		fprintf(stderr, "%s: generation failed for %s\n", argv[0],
		        argc == 3 ? argv[2] : "<stdin>");
		jp_buf_free(&in);
		return 1;
	}
	if (out.len)
		fwrite(out.data, 1, out.len, stdout);
	jp_buf_free(&out);
	jp_buf_free(&in);
	return 0;
}
