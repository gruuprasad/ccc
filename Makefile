BUILDDIR ?= build
CFG      ?= release
NAME     ?= c4
SRCDIR   ?= src

# Be verbose about the build.
Q ?= @

all:

-include config/$(CFG).cfg

HASH   := $(shell git log -1 --pretty=format:%H)
BINDIR := $(BUILDDIR)/$(CFG)
LEGACYDIR := $(BUILDDIR)/legacy/$(HASH)/$(CFG)
BIN    := $(BINDIR)/$(NAME)
LEGACYBIN := $(LEGACYDIR)/$(NAME)
SRC    := $(sort $(SRCDIR)/main.cpp $(wildcard $(SRCDIR)/**/*.cpp))
OBJ    := $(SRC:$(SRCDIR)/%.cpp=$(BINDIR)/%.o)
DEP    := $(OBJ:%.o=%.d)

# Try to locate llvm-config, a tool that produces command line flags for the
# build process.
ifneq ("$(wildcard llvm/install/bin/llvm-config)","")
	LLVM_CONFIG  ?= llvm/install/bin/llvm-config
else
	LLVM_CONFIG  ?= llvm-config
endif

# Obtain the LLVM build flags if we found llvm-config.
ifeq (,$(wildcard $(LLVM_CONFIG)))
	LLVM_CFLAGS  :=
	LLVM_LDFLAGS :=
else
	LLVM_CFLAGS  := $(shell $(LLVM_CONFIG) --cppflags)
	LLVM_LDFLAGS := $(shell $(LLVM_CONFIG) --ldflags --libs --system-libs)
endif

CFLAGS   := $(LLVM_CFLAGS) $(CFLAGS)
CXXFLAGS += $(CFLAGS) -std=c++11 -MMD
LDFLAGS  += $(LLVM_LDFLAGS)

DUMMY := $(shell mkdir -p $(sort $(dir $(OBJ))))

#cosmetic
COLOR ?= \033[33m
_COLOR := \033[0m

# build rules
.PHONY: all clean

all:
	@$(MAKE) clean --no-print-directory
	@$(MAKE) -j$(nproc) $(BIN) --no-print-directory

-include $(DEP)

legacy:
	@$(MAKE) -j$(nproc) $(BIN) --no-print-directory
	@echo "$(COLOR)===> LEGACY$(_COLOR) | $(LEGACYBIN)"
	$(Q)mkdir -p $(LEGACYDIR)
	@echo "$(shell echo ${CXXFLAGS} | sed -e 's/^[ \t]*//')" > $(LEGACYBIN).flags
	$(Q)mv $(BIN) $(LEGACYBIN)

clean:
	@echo "$(COLOR)===> CLEAN$(_COLOR) | $(BINDIR)"
	$(Q)rm -fr $(BINDIR)

$(BIN): $(OBJ)
	@echo "$(COLOR)===> LD  [$(shell echo $(CXX) ${LDFLAGS} | sed -e 's/^[ \t]*//')]$(_COLOR) | $@"
	$(Q)$(CXX) -o $(BIN) $(OBJ) $(LDFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "$(COLOR)===> CXX [$(shell echo $(CXX) ${CXXFLAGS} | sed -e 's/^[ \t]*//')]$(_COLOR) | $*"
	$(Q)$(CXX) $(CXXFLAGS) -c -o $@ $<
