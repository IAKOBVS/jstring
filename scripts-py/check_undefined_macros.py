#!/usr/bin/env python3
"""
For a given file and macro prefix, find all `#define PREFIX_MACRO 1`
and generate `#ifndef PREFIX_MACRO ... #define PREFIX_MACRO 0 ... #endif`
stubs inserted before the last `#endif`.
"""
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from jlib import jl_file_get_str


def main() -> None:
    if len(sys.argv) < 3:
        sys.exit(f"Usage: {sys.argv[0]} <prefix> <filename>")

    prefix: str = sys.argv[1]
    fname: str = sys.argv[2]
    file_str: str = jl_file_get_str(fname)
    lines: list[str] = file_str.split('\n')

    # Collect every `#define PREFIX_MACRO 1` that is not already given a
    # fallback definition (`PREFIX_MACRO 0`) and is not a header guard
    # (names ending in `_H`).
    undef: str = ''
    for line in lines:  # line: str
        m: re.Match[str] | None = re.match(r'^[ \t]*#[ \t]*define[ \t]+(' + re.escape(prefix) + r'\w*)[ \t]+1', line)
        if m:
            macro: str = m.group(1)
            if not re.search(re.escape(macro) + r'[ \t]+0', file_str) and not macro.endswith('_H'):
                undef += f"#ifndef {macro}\n#\tdefine {macro} 0\n#endif\n"

    # Insert the fallback definitions just before the closing #endif.
    end: int = file_str.rfind('#endif')
    file_str = file_str[:end] + undef + file_str[end:]
    sys.stdout.write(file_str)


if __name__ == '__main__':
    main()
