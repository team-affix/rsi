# ==============================================================================
# Toolchain
# ==============================================================================

MAKEFLAGS += -j$(shell nproc)

CXX      = g++
DEPFLAGS = -MMD -MP
CXXFLAGS = -std=c++20 -I. -Icore/hpp -Icli/hpp $(DEPFLAGS)
AR       = ar
ARFLAGS  = rcs

DEBUG_CXXFLAGS      = -DDEBUG -g
DEBUG_FAST_CXXFLAGS = -DDEBUG -g -O3
PROFILE_CXXFLAGS    = -DDEBUG -g -O3 -pg
RELEASE_CXXFLAGS    = -O3

# ==============================================================================
# Paths
# ==============================================================================

GTEST_INC     = googletest/googletest/include
GMOCK_INC     = googletest/googlemock/include
GTEST_ALL_CC  = googletest/googletest/src/gtest-all.cc
GMOCK_ALL_CC  = googletest/googlemock/src/gmock-all.cc
GTEST_CPPFLAGS = \
    -I$(GTEST_INC) -I$(GMOCK_INC) \
    -Igoogletest/googletest -Igoogletest/googlemock

GIT_TAG := $(shell git describe --tags --always --dirty)

# ==============================================================================
# Output names  (all under build/)
# ==============================================================================

CORE_LIB            = build/librsi_core.a
CORE_DEBUG_LIB      = build/librsi_core_debug.a
CORE_DEBUG_FAST_LIB = build/librsi_core_debug_fast.a
CORE_PROFILE_LIB    = build/librsi_core_profile.a

CLI_LIB            = build/librsi_cli.a
CLI_DEBUG_LIB      = build/librsi_cli_debug.a
CLI_DEBUG_FAST_LIB = build/librsi_cli_debug_fast.a
CLI_PROFILE_LIB    = build/librsi_cli_profile.a

CORE_DEBUG_BIN      = build/core_debug
CORE_DEBUG_FAST_BIN = build/core_debug_fast

CLI_DEBUG_BIN      = build/cli_debug
CLI_DEBUG_FAST_BIN = build/cli_debug_fast

RSI_BIN         = build/rsi
RSI_PROFILE_BIN = build/rsi_profile

# ==============================================================================
# Object lists  (object files live in build/obj/<variant>/)
# ==============================================================================

# Core production: discovered at parse time (sources are always present).
CORE_SRC = $(shell find core/cpp -name '*.cpp' | sort)

CORE_OBJ            = $(patsubst core/cpp/%.cpp, build/obj/core/%.o,            $(CORE_SRC))
CORE_DEBUG_OBJ      = $(patsubst core/cpp/%.cpp, build/obj/core_debug/%.o,      $(CORE_SRC))
CORE_DEBUG_FAST_OBJ = $(patsubst core/cpp/%.cpp, build/obj/core_debug_fast/%.o, $(CORE_SRC))
CORE_PROFILE_OBJ    = $(patsubst core/cpp/%.cpp, build/obj/core_profile/%.o,    $(CORE_SRC))

# Core tests: same layout under build/obj/core_{debug,debug_fast}_test/.
CORE_TEST_SRC = $(shell find core/test -name '*.cpp' | sort)

CORE_DEBUG_TEST_OBJ = \
    $(patsubst core/test/%.cpp, build/obj/core_debug_test/%.o, $(CORE_TEST_SRC))
CORE_DEBUG_FAST_TEST_OBJ = \
    $(patsubst core/test/%.cpp, build/obj/core_debug_fast_test/%.o, $(CORE_TEST_SRC))

CORE_DEBUG_GTEST_OBJ = \
    build/obj/core_debug_test/gtest-all.o \
    build/obj/core_debug_test/gmock-all.o
CORE_DEBUG_FAST_GTEST_OBJ = \
    build/obj/core_debug_fast_test/gtest-all.o \
    build/obj/core_debug_fast_test/gmock-all.o

CORE_DEBUG_BIN_OBJ      = $(CORE_DEBUG_TEST_OBJ) $(CORE_DEBUG_GTEST_OBJ)
CORE_DEBUG_FAST_BIN_OBJ = $(CORE_DEBUG_FAST_TEST_OBJ) $(CORE_DEBUG_FAST_GTEST_OBJ)

CORE_DEBUG_TEST_CXXFLAGS      = $(DEBUG_CXXFLAGS) $(GTEST_CPPFLAGS) -Icore/test
CORE_DEBUG_FAST_TEST_CXXFLAGS = $(DEBUG_FAST_CXXFLAGS) $(GTEST_CPPFLAGS) -Icore/test

# CLI tests
CLI_TEST_SRC = $(shell find cli/test -name '*.cpp' | sort)

CLI_DEBUG_TEST_OBJ = \
    $(patsubst cli/test/%.cpp, build/obj/cli_debug_test/%.o, $(CLI_TEST_SRC))
CLI_DEBUG_FAST_TEST_OBJ = \
    $(patsubst cli/test/%.cpp, build/obj/cli_debug_fast_test/%.o, $(CLI_TEST_SRC))

CLI_DEBUG_GTEST_OBJ = \
    build/obj/cli_debug_test/gtest-all.o \
    build/obj/cli_debug_test/gmock-all.o
CLI_DEBUG_FAST_GTEST_OBJ = \
    build/obj/cli_debug_fast_test/gtest-all.o \
    build/obj/cli_debug_fast_test/gmock-all.o

CLI_DEBUG_BIN_OBJ      = $(CLI_DEBUG_TEST_OBJ) $(CLI_DEBUG_GTEST_OBJ)
CLI_DEBUG_FAST_BIN_OBJ = $(CLI_DEBUG_FAST_TEST_OBJ) $(CLI_DEBUG_FAST_GTEST_OBJ)

CLI_DEBUG_TEST_CXXFLAGS      = $(DEBUG_CXXFLAGS) $(GTEST_CPPFLAGS) -Icli/test
CLI_DEBUG_FAST_TEST_CXXFLAGS = $(DEBUG_FAST_CXXFLAGS) $(GTEST_CPPFLAGS) -Icli/test

# CLI: source files are always present; no codegen needed for compilation.
# cli/cpp/ contains the library sources; cli/entry/ contains the entrypoint.
CLI_SRC = $(shell find cli/cpp -name '*.cpp' | sort)

CLI_OBJ            = $(patsubst cli/cpp/%.cpp, build/obj/cli/%.o,            $(CLI_SRC))
CLI_DEBUG_OBJ      = $(patsubst cli/cpp/%.cpp, build/obj/cli_debug/%.o,      $(CLI_SRC))
CLI_DEBUG_FAST_OBJ = $(patsubst cli/cpp/%.cpp, build/obj/cli_debug_fast/%.o, $(CLI_SRC))
CLI_PROFILE_OBJ    = $(patsubst cli/cpp/%.cpp, build/obj/cli_profile/%.o,    $(CLI_SRC))

# Compiler-written header deps (one .d per .o); empty until the first compile.
CORE_DEP            = $(CORE_OBJ:.o=.d)
CORE_DEBUG_DEP      = $(CORE_DEBUG_OBJ:.o=.d)
CORE_DEBUG_FAST_DEP = $(CORE_DEBUG_FAST_OBJ:.o=.d)
CORE_PROFILE_DEP    = $(CORE_PROFILE_OBJ:.o=.d)
CORE_DEBUG_TEST_DEP      = $(CORE_DEBUG_TEST_OBJ:.o=.d)
CORE_DEBUG_FAST_TEST_DEP = $(CORE_DEBUG_FAST_TEST_OBJ:.o=.d)
CORE_DEBUG_GTEST_DEP      = $(CORE_DEBUG_GTEST_OBJ:.o=.d)
CORE_DEBUG_FAST_GTEST_DEP = $(CORE_DEBUG_FAST_GTEST_OBJ:.o=.d)
CLI_DEBUG_TEST_DEP      = $(CLI_DEBUG_TEST_OBJ:.o=.d)
CLI_DEBUG_FAST_TEST_DEP = $(CLI_DEBUG_FAST_TEST_OBJ:.o=.d)
CLI_DEBUG_GTEST_DEP      = $(CLI_DEBUG_GTEST_OBJ:.o=.d)
CLI_DEBUG_FAST_GTEST_DEP = $(CLI_DEBUG_FAST_GTEST_OBJ:.o=.d)
CLI_DEP            = $(CLI_OBJ:.o=.d)
CLI_DEBUG_DEP      = $(CLI_DEBUG_OBJ:.o=.d)
CLI_DEBUG_FAST_DEP = $(CLI_DEBUG_FAST_OBJ:.o=.d)
CLI_PROFILE_DEP    = $(CLI_PROFILE_OBJ:.o=.d)

# ==============================================================================
# User-facing targets
# ==============================================================================

.PHONY: all core core_debug core_debug_fast cli cli_debug cli_debug_fast \
        rsi rsi_profile clean

all: core core_debug core_debug_fast cli cli_debug cli_debug_fast rsi

core: $(CORE_LIB)

core_debug: $(CORE_DEBUG_BIN)

core_debug_fast: $(CORE_DEBUG_FAST_BIN)

cli: $(CLI_LIB)

cli_debug: $(CORE_DEBUG_LIB) $(CLI_DEBUG_LIB) $(CLI_DEBUG_BIN)

cli_debug_fast: $(CORE_DEBUG_FAST_LIB) $(CLI_DEBUG_FAST_LIB) $(CLI_DEBUG_FAST_BIN)

rsi: $(CORE_LIB) $(CLI_LIB) $(RSI_BIN)

rsi_profile: $(CORE_PROFILE_LIB) $(CLI_PROFILE_LIB) $(RSI_PROFILE_BIN)

clean:
	rm -rf build

# ==============================================================================
# Library archive rules
# ==============================================================================

$(CORE_LIB): $(CORE_OBJ) | build
	$(AR) $(ARFLAGS) $@ $(CORE_OBJ)

$(CORE_DEBUG_LIB): $(CORE_DEBUG_OBJ) | build
	$(AR) $(ARFLAGS) $@ $(CORE_DEBUG_OBJ)

$(CORE_DEBUG_FAST_LIB): $(CORE_DEBUG_FAST_OBJ) | build
	$(AR) $(ARFLAGS) $@ $(CORE_DEBUG_FAST_OBJ)

$(CORE_PROFILE_LIB): $(CORE_PROFILE_OBJ) | build
	$(AR) $(ARFLAGS) $@ $(CORE_PROFILE_OBJ)

$(CLI_LIB): $(CLI_OBJ) | build
	$(AR) $(ARFLAGS) $@ $(CLI_OBJ)

$(CLI_DEBUG_LIB): $(CLI_DEBUG_OBJ) | build
	$(AR) $(ARFLAGS) $@ $(CLI_DEBUG_OBJ)

$(CLI_DEBUG_FAST_LIB): $(CLI_DEBUG_FAST_OBJ) | build
	$(AR) $(ARFLAGS) $@ $(CLI_DEBUG_FAST_OBJ)

$(CLI_PROFILE_LIB): $(CLI_PROFILE_OBJ) | build
	$(AR) $(ARFLAGS) $@ $(CLI_PROFILE_OBJ)

# ==============================================================================
# Binary link rules
# ==============================================================================

$(CORE_DEBUG_BIN): $(CORE_DEBUG_LIB) $(CORE_DEBUG_BIN_OBJ) | build
	$(CXX) $(CXXFLAGS) -o $@ \
	    $(CORE_DEBUG_BIN_OBJ) \
	    -Lbuild -lrsi_core_debug -lpthread

$(CORE_DEBUG_FAST_BIN): $(CORE_DEBUG_FAST_LIB) $(CORE_DEBUG_FAST_BIN_OBJ) | build
	$(CXX) $(CXXFLAGS) -o $@ \
	    $(CORE_DEBUG_FAST_BIN_OBJ) \
	    -Lbuild -lrsi_core_debug_fast -lpthread

$(CLI_DEBUG_BIN): $(CLI_DEBUG_LIB) $(CORE_DEBUG_LIB) $(CLI_DEBUG_BIN_OBJ) | build
	$(CXX) $(CXXFLAGS) -o $@ \
	    $(CLI_DEBUG_BIN_OBJ) \
	    -Lbuild -lrsi_cli_debug -lrsi_core_debug -lpthread

$(CLI_DEBUG_FAST_BIN): $(CLI_DEBUG_FAST_LIB) $(CORE_DEBUG_FAST_LIB) $(CLI_DEBUG_FAST_BIN_OBJ) | build
	$(CXX) $(CXXFLAGS) -o $@ \
	    $(CLI_DEBUG_FAST_BIN_OBJ) \
	    -Lbuild -lrsi_cli_debug_fast -lrsi_core_debug_fast -lpthread

$(RSI_BIN): $(CORE_LIB) $(CLI_LIB) cli/entry/main.cpp | build
	$(CXX) $(CXXFLAGS) $(RELEASE_CXXFLAGS) \
	    -DRSI_GIT_TAG=\"$(GIT_TAG)\" \
	    cli/entry/main.cpp \
	    -Lbuild -lrsi_cli -lrsi_core \
	    -o $@

$(RSI_PROFILE_BIN): $(CORE_PROFILE_LIB) $(CLI_PROFILE_LIB) cli/entry/main.cpp | build
	$(CXX) $(CXXFLAGS) $(PROFILE_CXXFLAGS) \
	    -DRSI_GIT_TAG=\"$(GIT_TAG)\" \
	    cli/entry/main.cpp \
	    -Lbuild -lrsi_cli_profile -lrsi_core_profile \
	    -o $@

# ==============================================================================
# Compilation pattern rules
# ==============================================================================

# --- core (release | debug | debug_fast | profile) ---

build/obj/core/%.o: core/cpp/%.cpp | build/obj/core
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(RELEASE_CXXFLAGS) -c $< -o $@

build/obj/core_debug/%.o: core/cpp/%.cpp | build/obj/core_debug
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEBUG_CXXFLAGS) -c $< -o $@

build/obj/core_debug_fast/%.o: core/cpp/%.cpp | build/obj/core_debug_fast
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEBUG_FAST_CXXFLAGS) -c $< -o $@

build/obj/core_profile/%.o: core/cpp/%.cpp | build/obj/core_profile
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(PROFILE_CXXFLAGS) -c $< -o $@

# --- core tests (debug | debug_fast) ---

build/obj/core_debug_test/%.o: core/test/%.cpp | build/obj/core_debug_test
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CORE_DEBUG_TEST_CXXFLAGS) -c $< -o $@

build/obj/core_debug_test/gtest-all.o: $(GTEST_ALL_CC) | build/obj/core_debug_test
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CORE_DEBUG_TEST_CXXFLAGS) -c $< -o $@

build/obj/core_debug_test/gmock-all.o: $(GMOCK_ALL_CC) | build/obj/core_debug_test
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CORE_DEBUG_TEST_CXXFLAGS) -c $< -o $@

build/obj/core_debug_fast_test/%.o: core/test/%.cpp | build/obj/core_debug_fast_test
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CORE_DEBUG_FAST_TEST_CXXFLAGS) -c $< -o $@

build/obj/core_debug_fast_test/gtest-all.o: $(GTEST_ALL_CC) | build/obj/core_debug_fast_test
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CORE_DEBUG_FAST_TEST_CXXFLAGS) -c $< -o $@

build/obj/core_debug_fast_test/gmock-all.o: $(GMOCK_ALL_CC) | build/obj/core_debug_fast_test
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CORE_DEBUG_FAST_TEST_CXXFLAGS) -c $< -o $@

# --- cli tests (debug | debug_fast) ---

build/obj/cli_debug_test/%.o: cli/test/%.cpp | build/obj/cli_debug_test
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CLI_DEBUG_TEST_CXXFLAGS) -c $< -o $@

build/obj/cli_debug_test/gtest-all.o: $(GTEST_ALL_CC) | build/obj/cli_debug_test
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CLI_DEBUG_TEST_CXXFLAGS) -c $< -o $@

build/obj/cli_debug_test/gmock-all.o: $(GMOCK_ALL_CC) | build/obj/cli_debug_test
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CLI_DEBUG_TEST_CXXFLAGS) -c $< -o $@

build/obj/cli_debug_fast_test/%.o: cli/test/%.cpp | build/obj/cli_debug_fast_test
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CLI_DEBUG_FAST_TEST_CXXFLAGS) -c $< -o $@

build/obj/cli_debug_fast_test/gtest-all.o: $(GTEST_ALL_CC) | build/obj/cli_debug_fast_test
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CLI_DEBUG_FAST_TEST_CXXFLAGS) -c $< -o $@

build/obj/cli_debug_fast_test/gmock-all.o: $(GMOCK_ALL_CC) | build/obj/cli_debug_fast_test
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CLI_DEBUG_FAST_TEST_CXXFLAGS) -c $< -o $@

# --- cli (release | debug | debug_fast | profile) ---

build/obj/cli/%.o: cli/cpp/%.cpp | build/obj/cli
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(RELEASE_CXXFLAGS) -c $< -o $@

build/obj/cli_debug/%.o: cli/cpp/%.cpp | build/obj/cli_debug
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEBUG_CXXFLAGS) -c $< -o $@

build/obj/cli_debug_fast/%.o: cli/cpp/%.cpp | build/obj/cli_debug_fast
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEBUG_FAST_CXXFLAGS) -c $< -o $@

build/obj/cli_profile/%.o: cli/cpp/%.cpp | build/obj/cli_profile
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(PROFILE_CXXFLAGS) -c $< -o $@

# ==============================================================================
# Header dependencies (compiler-generated .d files)
# ==============================================================================

-include $(CORE_DEP)
-include $(CORE_DEBUG_DEP)
-include $(CORE_DEBUG_FAST_DEP)
-include $(CORE_PROFILE_DEP)
-include $(CORE_DEBUG_TEST_DEP)
-include $(CORE_DEBUG_FAST_TEST_DEP)
-include $(CORE_DEBUG_GTEST_DEP)
-include $(CORE_DEBUG_FAST_GTEST_DEP)
-include $(CLI_DEBUG_TEST_DEP)
-include $(CLI_DEBUG_FAST_TEST_DEP)
-include $(CLI_DEBUG_GTEST_DEP)
-include $(CLI_DEBUG_FAST_GTEST_DEP)
-include $(CLI_DEP)
-include $(CLI_DEBUG_DEP)
-include $(CLI_DEBUG_FAST_DEP)
-include $(CLI_PROFILE_DEP)

# ==============================================================================
# Build directory creation
# ==============================================================================

build \
build/obj/core build/obj/core_debug build/obj/core_debug_fast build/obj/core_profile \
build/obj/core_debug_test build/obj/core_debug_fast_test \
build/obj/cli_debug_test build/obj/cli_debug_fast_test \
build/obj/cli build/obj/cli_debug build/obj/cli_debug_fast build/obj/cli_profile:
	mkdir -p $@
