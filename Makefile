CXXFLAGS += -std=c++0x -Wall -Wextra -pedantic
CPPFLAGS += -Isrc

ifneq ($(shell $(CXX) --version | grep clang),)
	ifeq ($(shell uname -s),Darwin)
		CXXFLAGS += -stdlib=libc++
	endif
endif

SRC = $(wildcard src/*.cpp) $(wildcard src/types/*.cpp) $(wildcard src/utils/*.cpp)
OBJ = $(SRC:.cpp=.o)

.PHONY: all release debug 32bit clean dist-clean build-test test
all: release

release: libamf.a
debug: CXXFLAGS += -g
debug: libamf.a
32bit: CXXFLAGS += -m32
32bit: release

libamf.a: $(OBJ)
	ar rv $@ $^

clean:
	rm -f libamf.a $(OBJ) .dep

dist-clean: clean
	$(MAKE) -C tests clean

build-test: libamf.a
	$(MAKE) -C tests

test: build-test
	tests/main

.dep:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM $(SRC) | \
		sed '/^[^[:space:]]/s,^[^:]*: \([^[:space:]]*\)/,\1/&,;s,:, $@:,' > $@

ifeq ($(filter $(MAKECMDGOALS),clean dist-clean),)
-include .dep
endif
