#!/usr/bin/env python3
"""Namespace internal macros as `JSTR_INTERNAL_*`.

Port of `scripts-perl/namespace-macros.pl`. Reads a header from stdin (or a
filename) and rewrites every macro that is `#undef`'d at the end of the
file, except those already prefixed with `JSTR`/`jstr`.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from jlib import jl_file_get_str, jl_file_namespace_macros


def main() -> None:
    if len(sys.argv) == 1:
        file_str: str = sys.stdin.read()
    else:
        file_str = jl_file_get_str(sys.argv[1])
    file_str = jl_file_namespace_macros(file_str, 'JSTR_INTERNAL_', ['JSTR', 'jstr'])
    sys.stdout.write(file_str)


if __name__ == '__main__':
    main()
