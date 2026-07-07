#!/usr/bin/env python3
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from jlib import jl_file_get_str, jl_file_to_blocks, jl_fn_get, jl_fn_to_string

ATTR_INLINE: str = 'JSTR_ATTR_INLINE'


def main() -> None:
    if len(sys.argv) < 2:
        sys.exit(f"Usage: {sys.argv[0]} <filename>")

    fname = sys.argv[1]
    file_str1 = jl_file_get_str(fname)
    out = ''
    for block in jl_file_to_blocks(file_str1):
        result = jl_fn_get(block)
        if result and '/*' not in block:
            attr, rettype, name, arg_arr, _ = result
            if re.match(r'^[Pp]', name):
                continue
            out += jl_fn_to_string(attr, rettype, name, arg_arr) + ';'
            attr = re.sub(r'(\W|^)(?:static|' + re.escape(ATTR_INLINE) + r')(\W|$)', r'\1\2', attr)
        else:
            block = re.sub(r'^#[\t ]*include[ \t]*"_.*$', '', block, flags=re.MULTILINE)
            out += block
        out += '\n\n'
    sys.stdout.write(out)


if __name__ == '__main__':
    main()
