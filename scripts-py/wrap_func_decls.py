#!/usr/bin/env python3
"""
wrap-func-decls.py — post-process generated .h files to support
dual-mode inclusion:

  Mode 1 (header-only):
    No special define needed.  Functions are static (current behavior).

  Mode 2 (declarations-only):
    Define JSTR_DECL_ONLY before including.  Function bodies are
    suppressed; only extern prototypes remain.  Link against libjstr.so.

  Mode 3 (implementation / library build):
    Define JSTR_IMPLEMENTATION in exactly one TU.  Functions become
    non-static extern definitions (without 'static').
"""
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from jlib import jl_file_to_blocks

# Regex to match function definitions, capturing 6 groups:
#   1 — attributes before return type
#   2 — return type (possibly with pointer asterisks)
#   3 — function name
#   4 — argument list
#   5 — attributes between argument list and body (JSTR_NOEXCEPT etc.)
#   6 — function body
WRAP_FN_RE: re.Pattern[str] = re.compile(
    r'^'
    r'([^{}]*(?:^|\W))'          # 1 — pre-return attrs
    r'(\w+\s*[* \t\n]*)'         # 2 — return type
    r'\s+'
    r'(\w+)'                     # 3 — name
    r'\s*\(([^{}]*)\)'           # 4 — args
    r'([^{}]*)'                  # 5 — post-args attrs
    r'\{((?:.|\n)*)\}'           # 6 — body
)

# Non-function patterns that can falsely match
NON_FUNCS: set[str] = {'define', 'if', 'else', 'elif', 'for', 'while', 'switch'}

IS_FUNC: re.Pattern[str] = re.compile(r'(?:JSTR_FUNC|JSTR_FUNC_VOID|JSTR_FUNC_PURE)')
IS_DECL_ONLY: re.Pattern[str] = re.compile(r'(?:JSTR_DECL_ONLY|JSTR_AS_LIBRARY)')


def main() -> None:
    file_str = sys.stdin.read()
    out = ''
    for block in jl_file_to_blocks(file_str):
        m = WRAP_FN_RE.match(block)
        if not m:
            out += block + '\n\n'
            continue
        pre_attr = m.group(1)
        rettype = m.group(2).strip()
        name = m.group(3)
        arg_str = m.group(4)
        post_attr = m.group(5)
        body = m.group(6)

        rettype_clean = re.sub(r'\s+', '', rettype)
        if rettype_clean in NON_FUNCS:
            out += block + '\n\n'
            continue
        if not IS_FUNC.search(block):
            out += block + '\n\n'
            continue
        if IS_DECL_ONLY.search(block):
            out += block + '\n\n'
            continue

        # Remove 'static' from pre-attributes
        pre_attr = re.sub(r'^[ \t]*static[ \t]*\n?', '', pre_attr, flags=re.MULTILINE)
        pre_attr = re.sub(r'\n[ \t]*static[ \t]*\n?', '\n', pre_attr)
        pre_attr = re.sub(r'^[ \t]*static[ \t]*', '', pre_attr)
        pre_attr = re.sub(r'[ \t]*static[ \t]*$', '', pre_attr)
        pre_attr = re.sub(r'^\s+', '', pre_attr)
        pre_attr = re.sub(r'\s+$', '', pre_attr)

        # Normalize arg string
        arg_str = re.sub(r',\s*$', '', arg_str)

        # Declaration (extern prototype)
        if pre_attr:
            decl = f"{pre_attr}\nextern {rettype}\n{name}({arg_str});"
        else:
            decl = f"extern {rettype}\n{name}({arg_str});"

        post_attr = re.sub(r'\s+$', '', post_attr)
        # Definition (with JSTR_API in place of static)
        if pre_attr and post_attr:
            defn = f"{pre_attr}\nJSTR_API {rettype}\n{name}({arg_str}){post_attr}\n{{ {body} }}"
        elif pre_attr:
            defn = f"{pre_attr}\nJSTR_API {rettype}\n{name}({arg_str})\n{{ {body} }}"
        elif post_attr:
            defn = f"JSTR_API {rettype}\n{name}({arg_str}){post_attr}\n{{ {body} }}"
        else:
            defn = f"JSTR_API {rettype}\n{name}({arg_str})\n{{ {body} }}"

        out += f"#if defined(JSTR_AS_LIBRARY) || defined(JSTR_DECL_ONLY)\n{decl}\n#else\n{defn}\n#endif\n\n"

    out = re.sub(r'\n\n*$', '\n', out)
    sys.stdout.write(out)


if __name__ == '__main__':
    main()
