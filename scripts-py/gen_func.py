#!/usr/bin/env python3
import re
import sys
from pathlib import Path


sys.path.insert(0, str(Path(__file__).resolve().parent))
from jlib import (
    jl_file_get_str, jl_file_to_blocks, jl_fn_get, jl_fn_to_string,
    jl_arg_get_var, jl_arg_index, jl_arg_is_ptr_ptr, jl_arg_is_ptr,
    jl_arg_is_const,
)

ATTR_NOINLINE: str     = 'JSTR_ATTR_NOINLINE'
ATTR_INLINE: str       = 'JSTR_ATTR_INLINE'
ATTR_ACCESS: str       = 'JSTR_ATTR_ACCESS'
ATTR_RESTRICT: str     = 'JSTR_RESTRICT'
ATTR_DEFAULT: str      = 'JSTR_FUNC'
ATTR_DEFAULT_VOID: str = 'JSTR_FUNC_VOID'
ATTR_RET_NONNULL: str  = 'JSTR_FUNC_RET_NONNULL'
JSTRING: str           = 'jstr_ty'
JSTRING_LIST: str      = 'jstrlist_ty'
VAR_JSTRING: str       = 'j'
VAR_JSTRING_LIST: str  = 'l'
DATA: str              = 'data'
SIZE: str              = 'size'
CAP: str               = 'capacity'
VAR_DATA: str          = 's'
VAR_SIZE: str          = 'sz'
VAR_CAP: str           = 'cap'
PREFIX_J: str          = '_j'
PREFIX_LEN: str        = '_len'


def add_inline(attr_str: str) -> str:
    if 'static' not in attr_str and ATTR_INLINE not in attr_str:
        attr_str = re.sub(re.escape(ATTR_NOINLINE), '', attr_str)
        attr_str += '\nstatic ' + ATTR_INLINE
    return attr_str


def grepped(arr: list[str], s: str) -> bool:
    return any(s in elem for elem in arr)


def usage() -> None:
    if len(sys.argv) < 2:
        sys.exit('Usage: ./' + sys.argv[0] + ' <file>')


def main() -> None:
    usage()
    fname = sys.argv[1]
    file_str1 = jl_file_get_str(fname)
    file_str2 = ''
    func_arr: list[str] = []

    # --- first pass: generate non-_len() wrappers for _len() functions ---
    for block in jl_file_to_blocks(file_str1):
        file_str2 += block + '\n\n'
        result = jl_fn_get(block)
        if not result:
            continue
        attr, rettype, name, arg_list, _ = result
        if not re.match(r'^jstr\w*_', name):
            continue
        if grepped(arg_list, '..'):   # variadic
            continue
        if len(arg_list) == 0:
            continue
        func_arr.append(name)
        if PREFIX_LEN not in name:
            continue
        if name.endswith(PREFIX_J):
            continue
        base_name = re.sub(re.escape(PREFIX_LEN) + r'(_|$)', r'\1', name)
        if base_name + '(' in file_str1:
            continue
        # Build the non-_len() function body
        body = '' if rettype == 'void' else 'return '
        body += name + '('
        name = base_name
        dont_make_func = True

        # Walk args, replacing _len size params with strlen()
        i = 0
        while i < len(arg_list):
            arg = arg_list[i]
            var = jl_arg_get_var(arg)
            if 'size_t' in arg:
                if re.search(re.escape(PREFIX_LEN) + '$', var):
                    dont_make_func = False
                    base_var = re.sub(re.escape(PREFIX_LEN) + r'(_|$)', r'\1', var)
                    i_str = jl_arg_index(arg_list, base_var)
                    if i_str != -1:
                        var_str = jl_arg_get_var(arg_list[i_str])
                        body += "strlen((const char *)" + var_str + ")"
                        arg_list.pop(i)
                        body += ', '
                        continue
                elif re.match(r'^[^*]' + re.escape(VAR_SIZE) + '$', var):
                    dont_make_func = False
                    var_str = jl_arg_get_var(arg_list[0])
                    body += "strlen((const char *)" + var_str + ")"
                    arg_list.pop(i)
                    body += ', '
                    continue
            body += var
            body += ', '
            i += 1

        if dont_make_func:
            continue
        body = re.sub(r', $', '', body)
        body += ');'
        attr = re.sub(r'^[ \t]*#.*$', '', attr, flags=re.MULTILINE)
        attr = re.sub(r'\s*' + re.escape(ATTR_ACCESS) + r'\(\(.*?\)\)', '', attr)
        attr = re.sub(r'\n\n', '', attr)
        attr = re.sub(r'\n$', '', attr)
        attr = add_inline(attr)
        body = re.sub(r', $', '', body)

        file_str2 += jl_fn_to_string(attr, rettype, name, arg_list, body) + '\n\n'
        func_arr.append(name)

    file_str3 = ''

    # --- second pass: generate _j() wrappers ---
    for block in jl_file_to_blocks(file_str2):
        file_str3 += block + '\n\n'
        result = jl_fn_get(block)
        if not result:
            continue
        attr, rettype, name, arg_list, _ = result
        if not re.match(r'^jstr\w*_', name):
            continue
        if name.endswith(PREFIX_J):
            continue
        if len(arg_list) == 0:
            continue
        if 'unsafe' in name:
            continue
        has_size_or_cap = False
        has_variadic = False
        for a in arg_list:
            if a.endswith(VAR_SIZE) or a.endswith(VAR_CAP):
                has_size_or_cap = True
            elif '...' in a:
                has_variadic = True
                break
        if not has_size_or_cap or has_variadic:
            continue
        j_name = name + PREFIX_J
        if grepped(func_arr, j_name):
            continue
        returns_end_ptr = False
        if rettype == 'void':
            body = ''
        else:
            if re.search(r'_p(?:_|$)', name) and rettype != 'void':
                returns_end_ptr = True
                body = VAR_JSTRING + '->' + SIZE + ' = JSTR_DIFF('
                rettype = 'void'
                attr = re.sub(re.escape(ATTR_DEFAULT) + r'(\W|$)', ATTR_DEFAULT_VOID + r'\1', attr)
            elif grepped(func_arr, name + '_p'):
                continue
            else:
                body = 'return '
        body += name + '('
        name = j_name
        name = re.sub(r'_p(_|$)', r'\1', name)
        has_data = False
        j = 0
        while j < len(arg_list):
            arg = arg_list[j]
            var = jl_arg_get_var(arg)
            if var == VAR_DATA:
                has_data = True
                deref = '&' if jl_arg_is_ptr_ptr(arg) else ''
                body += deref + VAR_JSTRING + '->' + DATA
                arg_list[j] = ('const ' if jl_arg_is_const(arg) else '') + JSTRING + ' *' + ATTR_RESTRICT + ' j'
            elif var == VAR_SIZE:
                deref = '&' if jl_arg_is_ptr(arg) else ''
                body += deref + VAR_JSTRING + '->' + SIZE
                arg_list.pop(j)
                body += ', '
                continue
            elif var == VAR_CAP:
                deref = '&' if jl_arg_is_ptr(arg) else ''
                body += deref + VAR_JSTRING + '->' + CAP
                arg_list.pop(j)
                body += ', '
                continue
            else:
                body += var
            body += ', '
            j += 1
        if not has_data:
            continue
        body = re.sub(r', $', '', body)
        body += ')'
        if returns_end_ptr:
            body += ', ' + VAR_JSTRING + '->' + DATA + ')'
        body += ';'
        attr = re.sub(re.escape(ATTR_RET_NONNULL), ATTR_DEFAULT_VOID, attr)
        attr = re.sub(r'^[ \t]*#.*$', '', attr, flags=re.MULTILINE)
        attr = re.sub(r'\s*' + re.escape(ATTR_ACCESS) + r'\(\(.*?\)\)', '', attr)
        attr = re.sub(r'\n\n', '', attr)
        attr = re.sub(r'\n$', '', attr)
        attr = add_inline(attr)
        file_str3 += jl_fn_to_string(attr, rettype, name, arg_list, body) + '\n\n'
        func_arr.append(name)

    file_str3 = re.sub(r'\n\n*$', '\n', file_str3)
    sys.stdout.write(file_str3)


if __name__ == '__main__':
    main()
