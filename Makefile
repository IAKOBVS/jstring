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

CC       ?= cc
CFLAGS   ?= -O2
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

.PHONY: all clean test testbuild install uninstall headers setup

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

# ─── Generated headers: macro files -> namespace_macros only ───
$(MACRO_GENS): $(HDRGEN)/%.h: $(INCLUDE)/%.h | $(HDRGEN)
	@mkdir -p $(dir $@)
	@cd $(SCRIPTS) && python3 namespace_macros.py "$<" > "$@"

# ─── Generated headers: internal -> namespace_macros only ───
$(INT_GENS): $(HDRGEN)/%.h: $(INCLUDE)/%.h | $(HDRGEN)
	@mkdir -p $(dir $@)
	@cd $(SCRIPTS) && python3 namespace_macros.py "$<" > "$@"

# ─── Generated headers: musl -> namespace_macros only ───
$(MUSL_GENS): $(HDRGEN)/%.h: $(INCLUDE)/%.h | $(HDRGEN)
	@mkdir -p $(dir $@)
	@cd $(SCRIPTS) && python3 namespace_macros.py "$<" > "$@"

# ─── Generated headers: regular -> gen_func | namespace_macros ───
$(REG_GENS): $(HDRGEN)/%.h: $(INCLUDE)/%.h | $(HDRGEN)
	@mkdir -p $(dir $@)
	@cd $(SCRIPTS) && python3 gen_func.py "$<" | python3 namespace_macros.py > "$@"

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
$(BUILD) $(OBJDIR) $(LIBDIR) $(SRCGEN) $(HDRGEN):
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
TESTSRC_ALL := $(wildcard $(REPO)/tests/*.c)
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

# Tests that must link the shared fuzz-regex implementation. The glob is
# absolute so grep -l already returns absolute paths (matching TESTSRC).
# fuzz-regex.c itself matches but is already filtered out of TESTSRC.
FUZZ_TESTS := $(shell grep -l 'fuzz-regex\.h' $(REPO)/tests/*.c 2>/dev/null)

test-obj = $(TEST_OBJDIR)/$(1)_$(2).o
test-bin = $(TEST_BINDIR)/$(1)_$(2)

# Shared fuzz-regex implementation objects (one per variant).
FUZZ_OBJ_v0 := $(TEST_OBJDIR)/fuzz-regex_v0.o
FUZZ_OBJ_v1 := $(TEST_OBJDIR)/fuzz-regex_v1.o
FUZZ_OBJ_v2 := $(TEST_OBJDIR)/fuzz-regex_v2.o
FUZZ_OBJ_v3 := $(TEST_OBJDIR)/fuzz-regex_v3.o

# $(1)=src basename, $(2)=src path, $(3)=variant.
# Returns the shared fuzz-regex object for fuzz tests, else empty.
test-sup = $(if $(filter $(2),$(FUZZ_TESTS)),$(FUZZ_OBJ_$(3)))

TESTBINS :=

# $(1)=src basename, $(2)=src path, $(3)=variant
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

# Per test × variant compile+l link rules.
$(foreach src,$(TESTSRC),$(foreach v,$(TVARS),$(eval $(call TEST_rules,$(basename $(notdir $(src))),$(src),$(v)))))

# Auto-dependency tracking for test objects.
-include $(wildcard $(TEST_OBJDIR)/*.d)

$(BUILD)/test-obj $(BUILD)/test-bin: | $(BUILD)
	@mkdir -p $@

# ─── Convenience ───
clean:
	rm -rf $(BUILD)

test: testbuild
	$(SCRIPTS)/test

# Build all test variant binaries incrementally (does not run them).
testbuild: headers $(LIB) $(TESTBINS)

install: $(LIB)
	$(SCRIPTS)/install

uninstall:
	$(SCRIPTS)/uninstall
