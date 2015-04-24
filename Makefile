CXXFLAGS += -std=c++0x -Wall -Wextra -pedantic
CPPFLAGS += -Isrc

ifneq ($(shell $(CXX) --version | grep clang),)
	ifeq ($(shell uname -s),Darwin)
		CXXFLAGS += -stdlib=libc++
	endif
endif

SRC = $(wildcard src/*.cpp) $(wildcard src/types/*.cpp)
OBJ = $(SRC:.cpp=.o)

.PHONY: all clean debug dist-clean release test
all: release

release: libamf.a
debug: CXXFLAGS += -g
debug: libamf.a
32bit: CXXFLAGS += -m32
32bit: release
libamf.a: libamf.a($(OBJ))

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

ifneq ($(MAKECMDGOALS),clean)
-include .dep
endif
