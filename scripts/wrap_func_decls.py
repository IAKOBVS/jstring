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


def build_decl(pre_attr: str, rettype: str, name: str, arg_str: str) -> str:
    """Assemble the extern prototype used in decl-only / library mode."""
    head: str = pre_attr + '\n' if pre_attr else ''
    return f"{head}extern {rettype}\n{name}({arg_str});"


def build_defn(pre_attr: str, post_attr: str, rettype: str, name: str, arg_str: str, body: str) -> str:
    """Assemble the full JSTR_API definition (header-only / impl mode)."""
    head: str = pre_attr + '\n' if pre_attr else ''
    return f"{head}JSTR_API {rettype}\n{name}({arg_str}){post_attr}\n{{ {body} }}"


def main() -> None:
    file_str: str = sys.stdin.read()
    out: str = ''
    for block in jl_file_to_blocks(file_str):  # block: str
        m: re.Match[str] | None = WRAP_FN_RE.match(block)
        if not m:
            out += block + '\n\n'
            continue
        pre_attr: str = m.group(1)
        rettype: str = m.group(2)
        name: str = m.group(3)
        arg_str: str = m.group(4)
        post_attr: str = m.group(5)
        body: str = m.group(6)

        # Preprocessor directives (`#define`, `#if`, ...) match WRAP_FN_RE;
        # detect them by the normalized return type.
        rettype_clean: str = re.sub(r'\s+', '', rettype)
        if rettype_clean in NON_FUNCS:
            out += block + '\n\n'
            continue
        if not IS_FUNC.search(block):
            # Faithful to wrap-func-decls.pl: a block that matches WRAP_FN_RE
            # without a JSTR_FUNC annotation is silently dropped (the Perl
            # `next` emits nothing). Preserved for byte-for-byte parity.
            continue
        if IS_DECL_ONLY.search(block):
            out += block + '\n\n'
            continue

        # Remove 'static' from pre-attributes.
        pre_attr = re.sub(r'^[ \t]*static[ \t]*\n?', '', pre_attr, flags=re.MULTILINE)
        pre_attr = re.sub(r'\n[ \t]*static[ \t]*\n?', '\n', pre_attr)
        pre_attr = re.sub(r'^[ \t]*static[ \t]*', '', pre_attr)
        pre_attr = re.sub(r'[ \t]*static[ \t]*$', '', pre_attr)
        pre_attr = re.sub(r'^\s+', '', pre_attr)
        pre_attr = re.sub(r'\s+$', '', pre_attr)

        # Normalize arg string.
        arg_str = re.sub(r',\s*$', '', arg_str)
        post_attr = re.sub(r'\s+$', '', post_attr)

        # Emit either the extern prototype (library / decl-only mode) or
        # the full JSTR_API definition (header-only / implementation mode).
        out += (
            f"#if defined(JSTR_AS_LIBRARY) || defined(JSTR_DECL_ONLY)\n"
            f"{build_decl(pre_attr, rettype, name, arg_str)}\n"
            f"#else\n"
            f"{build_defn(pre_attr, post_attr, rettype, name, arg_str, body)}\n"
            f"#endif\n\n"
        )

    # Collapse trailing newlines to a single one.
    out = re.sub(r'\n\n*$', '\n', out)
    sys.stdout.write(out)


if __name__ == '__main__':
    main()
