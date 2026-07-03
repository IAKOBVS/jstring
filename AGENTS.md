# jstring — agent guidance

## Build system

Custom shell+Perl scripts (no Makefile, no CMake). Run from repo root:

```sh
./scripts/build         # generate headers into build/include/jstr/
./compile         # generate headers + build shared library into build/
./test                  # run all tests (4 variants each, via scripts/test)
./clean                 # rm -rf build/
./scripts/fmt [files]   # clang-format (skips _jstr* and *macros* files)
./install               # sudo cp build/include/jstr/ → /usr/local/include/jstr
./install-to <dir>      # install to custom prefix
./uninstall             # remove installed files
```

`./compile` and `./scripts/build` both run `scripts/setup` first (checks page size, undefined macros, scoped macros), then generate headers via Perl (`gen-func.pl` + `namespace-macros.pl`). `./compile` additionally compiles a shared library into `build/lib/`.

## Code generation

- `include/*.h` are the source-of-truth headers; `build/include/jstr/*.h` are generated.
- `gen-func.pl` converts `JSTR_FUNC`/`JSTR_FUNC_VOID` annotations into `static inline` functions and generates `jstr_*` wrappers from annotated function blocks.
- **Do not put blank lines inside function bodies.** The Perl codegen splits blocks by blank lines.
- `namespace-macros.pl` renames `NAMESPACE_INTERNAL_*` → `jstr_internal_*` etc.

## Testing

```sh
./test                  # all tests, 4 combos: {normal, -march=native} × {fast, -DJSTR_TEST_SLOW=1}
./test <test.c> ...     # run specific test file(s) by name
./test-check-fail       # runs all tests, shows only failures (filters PASS)
scripts/test1 <tmpdir> <test.c> [cflags]  # compile & run a single test (one-off)
```

- **Tests are auto-discovered**: any `.c` file placed in `tests/` is picked up by the runner.
- Every test in `tests/*.c` is compiled and run from `/tmp`.
- Default CFLAGS: `-std=c99 -Wall -Wextra -Wpedantic -O2 -g -fsanitize=address`.
- All 4 variants run in parallel via `xargs -P "$NPROC"`; tests can take a while.
- Order: `./scripts/build && ./test`.
- Use `./test-check-fail` (not `./test`) when checking for errors — it filters out passing tests so failures are immediately visible.
- To run a single variant of a single test: `scripts/test1 /tmp tests/test-regex.c`.

## Language & toolchain

- **C99** (`-std=c99`). Flags: `-Wall -Wextra -Wpedantic -Wsign-conversion`.
- Format: `.clang-format` (WebKit brace style, 8-wide tab indentation, Never sort includes).
- If `tcc` is available, `./scripts/build` uses it for a syntax check instead of `cc`.
- libc requirement: musl's twoway `memmem` is vendored. POSIX headers (`io.h`, `regex.h`) must be included explicitly by the user.

## Key conventions

| Pattern | Meaning |
| :--- | :--- |
| `_len` | Takes explicit length parameter. |
| `_mut` | String is mutable (writable); may be modified in place (e.g., temporary NUL-termination). |
| `_nonnull*` | Assumes pointer arguments are non-null. |
| `_p` / `*stp*` | Returns pointer to end of processed string (e.g., `stpcpy`). |
| `_unsafe` | Assumes buffer has sufficient space (no reallocation). |
| `_from` | Operates starting from a given index (in-bounds assumed). |
| `_mem*` | String need not be NUL-terminated (takes `void *`). |
| `_backref` | Replacement string supports `\0` backreferences. |
| `_at` | Indexed access (bounds-checked when `JSTR_DEBUG=1`). |
| `_chk*` | Evaluates to true if value is an error. |
| No `_len` on `str` arg | String must be NUL-terminated; size parameter only optimizes `strlen`. |

**`char *` convention**: parameters of type `char *` imply no embedded NUL bytes
(`strlen(str) ≤ *sz`). Functions that take `char *` + `*sz` may use `strlen`,
`strcspn`, `strspn`, `memchr`, etc. internally and can stop at any NUL they
encounter. For data that may contain embedded NULs, use `_mem*` variants or cast
to `void *` and use the corresponding `_mem*` / `void *` overload when available.

**Always zero-initialize**: `jstr_ty j = JSTR_INIT;`

**Error handling**:
- Functions return negative values on error (typically `-1` or `JSTR_RET_ERR`). Check return values.
- `jstr_chk(x)` evaluates to true if `x` is an error value (`x < 0`).
- `jstr_err()` prints the last error message to stderr.
- `jstr_errdie()` prints the last error and calls `exit(EXIT_FAILURE)`.
- `JSTR_RETURN_ERR(msg)` is the canonical macro for setting an error and returning -1.
- Set `JSTR_PANIC=1` environment variable to auto-abort on any error (calls `abort()` in a signal handler).
- Test files include `<test.h>` which provides `t_assert`, `t_print`, and a shared test harness.

**Namespacing**: everything uses `[Jj][Ss][Tt][Rr]_` prefix.

## REG_STARTEND portability

The `regex.h` module uses the BSD extension `REG_STARTEND` (also available on Linux
since glibc 2.0) to match substrings without NUL-termination. On platforms without
`REG_STARTEND` (musl, older BSDs), two strategies are used:

| Function | Strategy | When to use |
| :--- | :--- | :--- |
| `jstr_re_exec_len` | `malloc` + copy + NUL-terminate + free | String may be read-only (`const char *`). |
| `jstr_re_exec_mut` | Temporarily NUL-terminates in place, restores original byte | String is known writable (`char *`). Avoids allocation. |

**Always prefer `jstr_re_exec_mut` when the buffer is writable** — it avoids the
`malloc`/`free` overhead of `jstr_re_exec_len`'s fallback. Internal functions
with `char * *s` parameters (e.g. `jstr_internal_re_rplcn_backref_len_from_exec`)
should use `jstr_re_exec_mut` rather than `jstr_re_exec_len`.

## Caveats

- `jstr_tolower()` / `jstr_toupper()` misbehave on `EOF`.
- Do not pass strings with embedded NULs to functions taking `char *` (use `void *` ones instead).
- Functions annotated `_len` that take `char *` + `*sz` may still stop at
  embedded NULs via `strlen`, `strcspn`, `strspn`, or `memchr` (see `char *`
  convention above). Use `_mem*` variants for binary-safe operations.
- `internal/*.h` are internal headers.
- Config macros go in `config.h` before including `jstr.h` (e.g., `JSTR_USE_UNLOCKED_IO`, `JSTR_DEBUG`, `JSTR_PANIC`).
- In regex tests, eflags must not be REG_NOSUB.

## License

MIT © 2023–2026 James Tirta Halim.
