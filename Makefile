# jstring — incremental Makefile build
# Usage: make            # build libjstr.so
#        make -j         # parallel build
#        make test       # build + run all test variants
#        make clean      # remove build/
#        make install    # install to /usr/local (needs sudo)
#        make headers    # regenerate generated headers only
#        make setup      # run one-time setup checks

REPO     := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
BUILD    := $(REPO)/build
INCLUDE  := $(REPO)/include
SCRIPTS  := $(REPO)/scripts

OBJDIR   := $(BUILD)/obj
LIBDIR   := $(BUILD)/lib
SRCGEN   := $(BUILD)/src
HDRGEN   := $(BUILD)/include/jstr
BINDIR   := $(BUILD)/bin

# C lexer/parser header generator (no Python, no regex).
PARSER_DIR := $(SCRIPTS)/parser
PARSER_BIN := $(BINDIR)/jstrgen
PARSER_SRC := $(PARSER_DIR)/util.c $(PARSER_DIR)/parser.c $(PARSER_DIR)/emit.c \
              $(PARSER_DIR)/main.c $(PARSER_DIR)/jstrgen.h

CC       ?= cc
CFLAGS   ?= -O2

# Use ccache when available (override with CCACHE= to disable).
CCACHE     ?= $(shell command -v ccache 2>/dev/null)
CCACHE_BIN := $(shell command -v $(CCACHE) 2>/dev/null)
ifneq ($(CCACHE_BIN),)
CC         := $(CCACHE_BIN) $(CC)
endif
# -D_XOPEN_SOURCE=700 exposes POSIX-gated jstr_io_* functions (readsystem,
# readdir, etc.) so they are exported from libjstr.so. The old ./scripts/test
# passed this through its CFLAGS when building the library; without it the
# library lacks those symbols and DECL_ONLY tests fail to link.
override CFLAGS += -std=c99 -Wall -Wextra -Wpedantic -Wsign-conversion -fPIC -MMD -MP -D_XOPEN_SOURCE=700

# ─── Discover compilation units ───
# Top-level headers that aren't structural/config produce .c wrappers.
SKIP     := $(INCLUDE)/struct.h $(INCLUDE)/macros.h $(INCLUDE)/macros-arch.h \
            $(INCLUDE)/macros-os.h $(INCLUDE)/pointer-arith.h \
            $(INCLUDE)/config.h $(INCLUDE)/jstr.h
COMPILES := $(filter-out $(SKIP),$(wildcard $(INCLUDE)/*.h))
GEN_CSRC := $(patsubst $(INCLUDE)/%.h,$(SRCGEN)/%.c,$(COMPILES))
OBJS     := $(patsubst $(SRCGEN)/%.c,$(OBJDIR)/%.o,$(GEN_CSRC))
DEPS     := $(OBJS:.o=.d)

LIB      := $(LIBDIR)/libjstr.so

# ─── Classify source headers for code generation ───
MACRO_HDRS := $(wildcard $(INCLUDE)/macros*.h)
INT_HDRS   := $(wildcard $(INCLUDE)/internal/*.h)
MUSL_HDRS  := $(wildcard $(INCLUDE)/internal/musl/*.h)
REG_HDRS   := $(filter-out $(MACRO_HDRS) $(INT_HDRS) $(MUSL_HDRS),$(wildcard $(INCLUDE)/*.h))

MACRO_GENS := $(patsubst $(INCLUDE)/%.h,$(HDRGEN)/%.h,$(MACRO_HDRS))
INT_GENS   := $(patsubst $(INCLUDE)/%.h,$(HDRGEN)/%.h,$(INT_HDRS))
MUSL_GENS  := $(patsubst $(INCLUDE)/%.h,$(HDRGEN)/%.h,$(MUSL_HDRS))
REG_GENS   := $(patsubst $(INCLUDE)/%.h,$(HDRGEN)/%.h,$(REG_HDRS))
ALL_GENS   := $(MACRO_GENS) $(INT_GENS) $(MUSL_GENS) $(REG_GENS)

# ═══════════════════════════════════════════════════════════════
# Targets
# ═══════════════════════════════════════════════════════════════

.PHONY: all clean test testbuild test1 test-parser test-runner bench-parser install uninstall headers setup

# Keep generated source wrappers (not intermediate)
.PRECIOUS: $(SRCGEN)/%.c

all: headers $(LIB)
	@echo "Library: $(LIB)"

# ─── Shared library ───
$(LIB): $(OBJS) | $(LIBDIR)
	@echo "  LD    $@"
	@$(CC) -shared -Wl,-soname,libjstr.so -flto -o $@ $^

# ─── Source wrappers ───
$(SRCGEN)/%.c: $(INCLUDE)/%.h | $(SRCGEN)
	@printf '#define JSTR_IMPLEMENTATION 1\n#include "../../include/%s"\n' '$(notdir $<)' > $@

# ─── Object compilation ───
# Depends on setup stamp (macros.h modified in-place by scripts/setup).
# -MMD -MP generate .d dependency files as a side effect.
$(OBJDIR)/%.o: $(SRCGEN)/%.c $(BUILD)/.setup-stamp | $(OBJDIR)
	@echo "  CC    $<"
	@$(CC) $(CFLAGS) -DJSTR_IMPLEMENTATION=1 -c $< -o $@

# ─── Header generator binary ───
$(PARSER_BIN): $(PARSER_SRC) | $(BINDIR)
	@echo "  CC    jstrgen"
	@$(CC) -std=c99 -O2 -Wall -Wextra -Wpedantic -Wsign-conversion -o $@ \
		$(PARSER_DIR)/util.c $(PARSER_DIR)/parser.c $(PARSER_DIR)/emit.c \
		$(PARSER_DIR)/main.c

# ─── Generated headers: macro files -> namespace only ───
$(MACRO_GENS): $(HDRGEN)/%.h: $(INCLUDE)/%.h $(PARSER_BIN) | $(HDRGEN)
	@mkdir -p $(dir $@)
	@$(PARSER_BIN) namespace "$<" > "$@"

# ─── Generated headers: internal -> namespace only ───
$(INT_GENS): $(HDRGEN)/%.h: $(INCLUDE)/%.h $(PARSER_BIN) | $(HDRGEN)
	@mkdir -p $(dir $@)
	@$(PARSER_BIN) namespace "$<" > "$@"

# ─── Generated headers: musl -> namespace only ───
$(MUSL_GENS): $(HDRGEN)/%.h: $(INCLUDE)/%.h $(PARSER_BIN) | $(HDRGEN)
	@mkdir -p $(dir $@)
	@$(PARSER_BIN) namespace "$<" > "$@"

# ─── Generated headers: regular -> gen-func | namespace ───
$(REG_GENS): $(HDRGEN)/%.h: $(INCLUDE)/%.h $(PARSER_BIN) | $(HDRGEN)
	@mkdir -p $(dir $@)
	@$(PARSER_BIN) gen-func "$<" | $(PARSER_BIN) namespace > "$@"

headers: $(ALL_GENS)

# ─── Setup (one-time, modifies include/macros*.h in place) ───
$(BUILD)/.setup-stamp: $(INCLUDE)/macros.h $(INCLUDE)/macros-arch.h $(INCLUDE)/macros-os.h | $(BUILD)
	@echo "  SETUP"
	@$(SCRIPTS)/setup
	@touch $@

setup: $(BUILD)/.setup-stamp

# ─── Auto-dependency tracking (included from prior builds) ───
-include $(DEPS)

# ─── Directory creation ───
$(BUILD) $(OBJDIR) $(LIBDIR) $(SRCGEN) $(HDRGEN) $(BINDIR):
	@mkdir -p $@

# ═══════════════════════════════════════════════════════════════
# Tests — incremental, parallel compilation
# ═══════════════════════════════════════════════════════════════
# Each tests/*.c is compiled into per-variant objects under build/test-obj/
# with -MMD -MP dependency tracking (so header changes trigger rebuilds),
# and linked into runnable binaries under build/test-bin/. scripts/test then
# only links already-built objects (never recompiles unchanged sources).
# The four variants are: {normal, -march=native} × {fast, -DJSTR_TEST_SLOW=1}.

TEST_OBJDIR := $(BUILD)/test-obj
TEST_BINDIR := $(BUILD)/test-bin
TESTSRC_ALL := $(shell find $(REPO)/tests -type f -name '*.c')
# fuzz-regex.c provides the shared fuzz_regex_patterns() implementation; it
# is not a standalone test (no main), only linked into the fuzz-regex tests.
FUZZ_SRC    := $(REPO)/tests/fuzz-regex.c
# Scratch bug-repro files that don't follow the test harness #include
# convention (they include headers in a way that conflicts with the
# -include jstr.h + DECL_ONLY build) — excluded from the suite.
TEST_SKIP   := $(REPO)/tests/repro_crash.c $(REPO)/tests/repro-replace-color.c
TESTSRC     := $(filter-out $(FUZZ_SRC) $(TEST_SKIP),$(TESTSRC_ALL))

TEST_BASE  := -std=c99 -Wall -Wextra -Wno-unused-result -O2 -g -fsanitize=address -D_XOPEN_SOURCE=700
TEST_DECL  := -DJSTR_DECL_ONLY -include $(HDRGEN)/jstr.h -iquote $(HDRGEN)

TVARS       := v0 v1 v2 v3
TV_FLAGS_v0 :=
TV_FLAGS_v1 := -march=native
TV_FLAGS_v2 := -DJSTR_TEST_SLOW=1
TV_FLAGS_v3 := -DJSTR_TEST_SLOW=1 -march=native
TV_LABEL_v0 := fast
TV_LABEL_v1 := fast -march=native
TV_LABEL_v2 := slow
TV_LABEL_v3 := slow -march=native

# Tests that must link the shared fuzz-regex implementation. The glob is
# absolute so grep -l already returns absolute paths (matching TESTSRC).
# fuzz-regex.c itself matches but is already filtered out of TESTSRC.
FUZZ_TESTS := $(shell grep -rl 'fuzz-regex\.h' $(REPO)/tests 2>/dev/null)

# Absolute tests/... source path -> target-name-safe unique key. The path
# relative to tests/ is preserved (slashes become underscores) so files with
# the same basename in different subdirectories do not shadow each other.
testkey = $(subst /,_,$(patsubst $(REPO)/tests/%.c,%,$(1)))

test-obj = $(TEST_OBJDIR)/$(1)_$(2).o
test-bin = $(TEST_BINDIR)/$(1)_$(2)

# Shared fuzz-regex implementation objects (one per variant).
FUZZ_OBJ_v0 := $(TEST_OBJDIR)/fuzz-regex_v0.o
FUZZ_OBJ_v1 := $(TEST_OBJDIR)/fuzz-regex_v1.o
FUZZ_OBJ_v2 := $(TEST_OBJDIR)/fuzz-regex_v2.o
FUZZ_OBJ_v3 := $(TEST_OBJDIR)/fuzz-regex_v3.o

# $(1)=test key, $(2)=src path, $(3)=variant.
# Returns the shared fuzz-regex object for fuzz tests, else empty.
test-sup = $(if $(filter $(2),$(FUZZ_TESTS)),$(FUZZ_OBJ_$(3)))

TESTBINS :=

# $(1)=test key, $(2)=src path, $(3)=variant
# NOTE: every per-iteration value must be referenced via $(call ...) with
# call params — these resolve during eval's single expansion to the correct
# literal for THIS rule. Never reference a $$(global) deferred in recipes
# (globals get overwritten each foreach iteration) and never a bare
# $(global) in the same eval block (eval expands the whole string before it
# parses, so it'd see the PREVIOUS iteration's value). Automatic vars ($@)
# are deferred with $$.
define TEST_rules
$(call test-obj,$(1),$(3)): $(2) $(HDRGEN)/jstr.h | $(TEST_OBJDIR) $(ALL_GENS)
	@echo "  CC  [$(3)] $(notdir $(2))"
	@$$(CC) $(TEST_BASE) $(TV_FLAGS_$(3)) $(TEST_DECL) -MMD -MP -c $(2) -o $$@
$(call test-bin,$(1),$(3)): $(call test-obj,$(1),$(3)) $(call test-sup,$(1),$(2),$(3)) | $(LIB) $(TEST_BINDIR)
	@echo "  LD  [$(3)] $(1)"
	@$$(CC) $(TEST_BASE) $(TV_FLAGS_$(3)) $(call test-obj,$(1),$(3)) $(call test-sup,$(1),$(2),$(3)) -L$(LIBDIR) -Wl,-rpath,$(LIBDIR) -ljstr -o $$@
TESTBINS += $(call test-bin,$(1),$(3))
endef

# All fuzz-regex.c objects (shared impl, per variant).
define FUZZ_rules
$(FUZZ_OBJ_$(1)): $(FUZZ_SRC) $(HDRGEN)/jstr.h | $(TEST_OBJDIR) $(ALL_GENS)
	@echo "  CC  [$(1)] fuzz-regex.c"
	@$(CC) $(TEST_BASE) $(TV_FLAGS_$(1)) $(TEST_DECL) -MMD -MP -c $(FUZZ_SRC) -o $$@
endef
$(foreach v,$(TVARS),$(eval $(call FUZZ_rules,$(v))))

# Test rules and the thousands of test dependency files are only needed when
# building tests. The recursive run-only make (goals run-*) skips both, so it
# doesn't reparse 3,500+ .d files just to launch already-built binaries.
TEST_BUILD_GOALS := $(filter test testbuild,$(MAKECMDGOALS))
TEST_RUN_GOALS   := $(filter test run-%,$(MAKECMDGOALS))

ifneq ($(TEST_BUILD_GOALS),)
# Per test × variant compile+l link rules.
$(foreach src,$(TESTSRC),$(foreach v,$(TVARS),$(eval $(call TEST_rules,$(call testkey,$(src)),$(src),$(v)))))

# Auto-dependency tracking for test objects.
-include $(wildcard $(TEST_OBJDIR)/*.d)

$(BUILD)/test-obj $(BUILD)/test-bin: | $(BUILD)
	@mkdir -p $@
endif

# Guard against future basename/path collisions silently shadowing a test.
TESTKEYS := $(foreach src,$(TESTSRC),$(call testkey,$(src)))
ifneq ($(words $(TESTKEYS)),$(words $(sort $(TESTKEYS))))
$(error duplicate generated test target key; rename the test or extend testkey)
endif

# ─── Running tests ───
# TESTS may narrow the run to specific test names/paths, e.g.
#   make test TESTS="test-builder test-string"
# Every test is still built by testbuild, but only the selected variants run.
RUN_SRCS := $(if $(strip $(TESTS)),$(foreach t,$(TESTS),$(filter %/$(basename $(notdir $(t))).c,$(TESTSRC))),$(TESTSRC))

RUN_TARGETS :=
# $(1)=test key, $(2)=src path, $(3)=variant
define RUN_rules
.PHONY: run-$(1)-$(3)
run-$(1)-$(3): $(call test-bin,$(1),$(3))
	@mkdir -p $(BUILD)/test-logs; \
	log=$(BUILD)/test-logs/$(1)_$(3).log; \
	if LD_LIBRARY_PATH=$(LIBDIR) $$< >$$log 2>&1; then \
		rm -f $$log; \
		echo 'PASS: $(1) [$(TV_LABEL_$(3))]'; \
	else \
		echo '--- output: $(1)_$(3) [$(TV_LABEL_$(3))] ---'; \
		tail -n 40 $$log; \
		echo 'FAIL: $(1) [$(TV_LABEL_$(3))]'; \
		exit 1; \
	fi
RUN_TARGETS += run-$(1)-$(3)
endef
ifneq ($(TEST_RUN_GOALS),)
$(foreach src,$(RUN_SRCS),$(foreach v,$(TVARS),$(eval $(call RUN_rules,$(call testkey,$(src)),$(src),$(v)))))

ifneq ($(strip $(TESTS)),)
ifeq ($(strip $(RUN_TARGETS)),)
$(error no matching tests for TESTS='$(TESTS)')
endif
endif
endif

NPROC := $(shell nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null || echo 1)
# Only force -j when the parent make isn't already parallel (avoids the
# "jobserver" warning when MAKEFLAGS already carries -j).
TEST_JOBFLAG := $(if $(filter -j% --jobs%,$(MAKEFLAGS)),,-j$(NPROC))

# ─── Convenience ───
clean:
	rm -rf $(BUILD)

test: testbuild test-parser test-runner
	@$(MAKE) --no-print-directory -k $(TEST_JOBFLAG) $(RUN_TARGETS)

# Golden tests for the C lexer/parser header generator.
test-parser: $(PARSER_BIN)
	@$(PARSER_DIR)/tests/run $(PARSER_BIN)

# Regression tests for the runner itself (scripts/test).
test-runner:
	@$(SCRIPTS)/test-selftest

# Compare the C generator against the Python reference pipeline.
bench-parser: $(PARSER_BIN)
	@$(PARSER_DIR)/bench

# Build all test variant binaries incrementally (does not run them).
testbuild: headers $(LIB) $(TESTBINS)

# Compile and run a single test (used by scripts/test1).
#   make test1 SRC=tests/foo.c [TMPDIR=/tmp] [FLAGS="..."]
test1: headers $(LIB)
	@set -eu; \
	src='$(SRC)'; \
	[ -n "$$src" ] || { echo 'usage: make test1 SRC=<test.c> [TMPDIR=<dir>] [FLAGS="..."]' >&2; exit 2; }; \
	tmpdir='$(TMPDIR)'; \
	[ -n "$$tmpdir" ] || tmpdir=$$(mktemp -d "$${TMPDIR:-/tmp}/jstr-test1.XXXXXX"); \
	mkdir -p "$$tmpdir"; \
	base=$$(basename "$$src" .c); \
	obj="$$tmpdir/$$base.o"; \
	decl='-DJSTR_DECL_ONLY -include $(HDRGEN)/jstr.h -iquote $(HDRGEN)'; \
	echo "  CC  $$src"; \
	$(CC) -c "$$src" -o "$$obj" $(FLAGS) $$decl; \
	extra=''; \
	if grep -q 'fuzz-regex\.h' "$$src" 2>/dev/null; then \
		support='$(REPO)/tests/fuzz-regex.c'; \
		sobj="$$tmpdir/fuzz-regex.o"; \
		$(CC) -c "$$support" -o "$$sobj" $(FLAGS) $$decl; \
		extra="$$sobj"; \
	fi; \
	echo "  LD  $$base"; \
	$(CC) "$$obj" $$extra -o "$$tmpdir/$$base" $(FLAGS) -L$(LIBDIR) -Wl,-rpath,$(LIBDIR) -ljstr; \
	if "$$tmpdir/$$base"; then \
		echo "PASS: $$base $(FLAGS)"; \
	else \
		echo "FAIL: $$base $(FLAGS)"; exit 1; \
	fi

install: $(LIB)
	$(SCRIPTS)/install

uninstall:
	$(SCRIPTS)/uninstall
