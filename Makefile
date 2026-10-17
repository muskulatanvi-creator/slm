CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -O2
SRCDIR   := src
BUILDDIR := build

LIB_SRCS := $(filter-out $(SRCDIR)/main.cpp,$(wildcard $(SRCDIR)/*.cpp))
LIB_OBJS := $(LIB_SRCS:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
MAIN_OBJ := $(BUILDDIR)/main.o

.PHONY: all clean test

all: slm

slm: $(MAIN_OBJ) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# -MMD -MP: track header dependencies so only what changed is recompiled
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

test_model: tests/test_model.cpp $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) -o $@ $^

test: test_model
	./test_model

clean:
	rm -rf $(BUILDDIR) slm test_model

-include $(wildcard $(BUILDDIR)/*.d)
