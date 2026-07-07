#!/usr/bin/env python3
"""
Check that every `#define MACRO` has a corresponding `#undef MACRO`.
Exits with code 1 if any macro is missing an undef.
"""
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from jlib import jl_file_get_str


def main() -> None:
    if len(sys.argv) < 2:
        sys.exit(f"Usage: {sys.argv[0]} <filename>")

    fname = sys.argv[1]
    file_str = jl_file_get_str(fname)
    lines = file_str.split('\n')
    for line in lines:
        m = re.match(r'^[ \t]*#[ \t]*define[ \t]+([A-Z][A-Z0-9_]*)', line)
        if m:
            macro = m.group(1)
            if not re.search(r'(?:^|\n)[ \t]*#[ \t]*undef[ \t]+' + re.escape(macro), file_str):
                print(f"{macro} is not defined in {fname}.")


if __name__ == '__main__':
    main()
