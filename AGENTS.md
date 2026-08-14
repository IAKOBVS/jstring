# jstring — agent guidance

## Build system

Custom shell scripts + Python generators (no Makefile, no CMake). Run from repo root:

```sh
./compile         # setup checks + generate headers + build shared library into build/
./test                  # run all tests (4 variants each, via scripts/test)
./clean                 # rm -rf build/
./scripts/fmt [files]   # clang-format (skips _jstr* and *macros* files)
./install               # sudo cp build/include/jstr/ → /usr/local/include/jstr
./install-to <dir>      # install to custom prefix
./uninstall             # remove installed files
```

`./compile` runs `scripts/setup` first (checks page size, undefined macros, scoped macros), then generates headers via Python (`scripts-py/gen_func.py` + `scripts-py/namespace_macros.py`), then compiles a shared library into `build/lib/`. Requires `python3` and `perl` is no longer needed.

## Code generation

- `include/*.h` are the source-of-truth headers; `build/include/jstr/*.h` are generated.
- `scripts-py/gen_func.py` converts `JSTR_FUNC`/`JSTR_FUNC_VOID` annotations into `static inline` functions and generates `jstr_*` wrappers from annotated function blocks.
- **Do not put blank lines inside function bodies.** The Python codegen splits blocks by blank lines.
- `scripts-py/namespace_macros.py` renames `NAMESPACE_INTERNAL_*` → `jstr_internal_*` etc.
- `scripts/check-py-parity` regenerates every header through the Python engine and the original Perl engine (re-extracted from git history) and asserts byte-identical output. Run it after any change to `scripts-py/`.

## Testing

```sh
./test                  # all tests, 4 combos: {normal, -march=native} × {fast, -DJSTR_TEST_SLOW=1}
./test-check-fail       # runs all tests, shows only failures (filters PASS)
scripts/test1 <tmpdir> <test.c> [cflags]  # compile & run a single test
./scripts/compile-nosimd  # build no-SIMD coverage library in build-nosimd/
```

- Every test in `tests/*.c` is compiled and run from `/tmp`.
- Tests using `mkdtemp` must `#define _POSIX_C_SOURCE 200809L` before any includes.
- New tests ARE automatically picked up — `./test` globs `tests/*.c`.
- For manual compilation of test-only files: use `-DJSTR_DECL_ONLY -include build/include/jstr/jstr.h -iquote build/include/jstr`.
- **No-SIMD test compilation** (not compatible with `scripts/test1`): use `-DJSTR_DECL_ONLY -include build-nosimd/include/jstr/jstr.h -iquote build-nosimd/include/jstr` and link with `-Lbuild-nosimd/lib -ljstr`, setting `LD_LIBRARY_PATH=build-nosimd/lib`.
- Default CFLAGS: `-std=c99 -Wall -Wextra -Wpedantic -O2 -g -fsanitize=address`.
- All 4 variants run in parallel; tests can take a while.
- Order: `./scripts/compile && ./test`.
- Use `./test-check-fail` (not `./test`) when checking for errors — it filters out passing tests so failures are immediately visible.

### Debugging test failures

When a test assertion fails:
1. Extract the failing sub-test and compile it standalone with `-D_XOPEN_SOURCE=700` (not `--coverage`)
2. Add `printf` to the callback and check `ftw->dirpath` and `ftw->st` info
3. The exact callback count depends on flags: `REG|DIR` counts root dir, `REG` skip root, no flags counts everything
4. `stat` follows symlinks, so symlinks appear as `S_IFREG` unless `lstat` is used
5. Use `cat > /tmp/test-debug.c << 'EOF' ... EOF` approach for rapid iteration

## Test-Driven Development

All new features, bug fixes, and changes **must** come with a test. Write the test first, verify it fails, then implement the fix/feature, then verify it passes.

Workflow:

1. Write a failing test in `tests/`.
2. Run `./compile && ./test-check-fail` to confirm the new test fails.
3. Implement the feature/fix.
4. Run `./compile && ./test-check-fail` to confirm the new test passes and no existing tests break.

Do not submit or consider a change complete without a corresponding test.

## Parallel Coverage Work

When working toward 100% line coverage, spin up to **5 subagents in parallel** to explore different coverage gaps simultaneously. Assign each agent a focused area (e.g., one module per agent) and have them return a detailed plan with specific file paths and line numbers for uncovered code, ordered by ease of coverage. Then implement tests serially.

Coverage priority order (highest ROI first):
1. **FTW edge cases** in `include/io.h` — NONFATAL_ERR, STATREG, symlink/socket types, non-dir func_match paths
2. **`include/builder.h`** — `jstr_reserve`/`jstr_reserveexactalways` failure paths, `jstr_shrink`, `jstr_grow`
3. **`include/regex.h`** — remaining 12 uncovered lines (error paths in compile/exec/replace)
4. **`include/replace.h`** — remaining 12 uncovered lines (allocation failure paths)
5. **`include/internal/musl/`** — vendored musl code (memrchr, twoway, strstr-lt8). Verify correctness by comparing against known musl sources and add edge-case tests for alignment branches, short needles, and long patterns.

Use `scripts/coverage` to measure results: build with `--coverage -fno-inline -D_XOPEN_SOURCE=700`, run all tests, check `lib_include_*.h.gcov` in the temp output directory.

## Language & toolchain

- **C99** (`-std=c99`). Flags: `-Wall -Wextra -Wpedantic -Wsign-conversion`.
- Format: `.clang-format` (WebKit brace style, 8-wide tab indentation, Never sort includes).
- If `tcc` is available, `./scripts/compile` uses it for a syntax check instead of `cc`.
- libc requirement: musl's twoway `memmem` is vendored. POSIX headers (`io.h`, `regex.h`) must be included explicitly by the user.
- **Library compilation caveat**: `./scripts/compile` does not define `_ATFILE_SOURCE` or `_XOPEN_SOURCE >= 700`, so the shared library is built without `USE_ATFILE=1` even on systems that support `*at` functions. Test-only code (DECL_ONLY) uses the generated headers which may define `USE_ATFILE=1`. This mismatch can cause different behavior in edge cases (e.g., `open` failure vs `stat` failure on nonexistent paths).

## Key conventions

| Pattern | Meaning |
| :--- | :--- |
| `_len` | Takes explicit length parameter. |
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

**Error handling**: functions return negative on error. Check return values. Use `jstr_err()` to print or `jstr_errdie()` to print and exit. Set `JSTR_PANIC=1` to auto-abort on errors.

**Namespacing**: everything uses `[Jj][Ss][Tt][Rr]_` prefix.

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
