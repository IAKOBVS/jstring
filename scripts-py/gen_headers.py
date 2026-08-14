#!/usr/bin/env python3
"""Generate standalone single-header versions of the source headers.

Port of `scripts/gen-headers.pl`. For each function block, emit the
declaration and strip `static`/`JSTR_ATTR_INLINE`; for non-function
blocks, drop `#include "_..."` lines that pull in internal headers.
"""
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from jlib import jl_file_get_str, jl_file_to_blocks, jl_fn_get, jl_fn_to_string, FnParts

ATTR_INLINE: str = 'JSTR_ATTR_INLINE'


def main() -> None:
    if len(sys.argv) < 2:
        sys.exit(f"Usage: {sys.argv[0]} <filename>")

    fname: str = sys.argv[1]
    file_str1: str = jl_file_get_str(fname)
    out: str = ''
    for block in jl_file_to_blocks(file_str1):  # block: str
        result: FnParts | None = jl_fn_get(block)
        if result and '/*' not in block:
            attr: str
            rettype: str
            name: str
            arg_arr: list[str]
            attr, rettype, name, arg_arr, _ = result
            # Skip names starting with P/p (private/internal helpers).
            if re.match(r'^[Pp]', name):
                continue
            out += jl_fn_to_string(attr, rettype, name, arg_arr) + ';'
            # The static-strip result is discarded here — kept only for
            # parity with gen-headers.pl.
            attr = re.sub(r'(\W|^)(?:static|' + re.escape(ATTR_INLINE) + r')(\W|$)', r'\1\2', attr)
        else:
            # Non-function block: strip internal-header includes.
            block = re.sub(r'^#[\t ]*include[ \t]*"_.*$', '', block, flags=re.MULTILINE)
            out += block
        out += '\n\n'
    sys.stdout.write(out)


if __name__ == '__main__':
    main()
