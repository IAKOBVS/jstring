#!/usr/bin/env python3
"""
Check that every `#define MACRO` has a corresponding `#undef MACRO`.

Prints a warning for each macro missing an undef. The `scripts/
check-scoped-macros` wrapper turns those warnings into a non-zero exit.
"""
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from jlib import jl_file_get_str


def main() -> None:
    if len(sys.argv) < 2:
        sys.exit(f"Usage: {sys.argv[0]} <filename>")

    fname: str = sys.argv[1]
    file_str: str = jl_file_get_str(fname)
    lines: list[str] = file_str.split('\n')
    for line in lines:  # line: str
        m: re.Match[str] | None = re.match(r'^[ \t]*#[ \t]*define[ \t]+([A-Z][A-Z0-9_]*)', line)
        if m:
            macro: str = m.group(1)
            if not re.search(r'(?:^|\n)[ \t]*#[ \t]*undef[ \t]+' + re.escape(macro), file_str):
                print(f"{macro} is not defined in {fname}.")


if __name__ == '__main__':
    main()
