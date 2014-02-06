CXXFLAGS += -std=c++0x -Wall -Wextra -pedantic
CPPFLAGS += -Isrc

ifneq ($(shell $(CXX) --version | grep clang),)
	ifeq ($(shell uname -s),Darwin)
		CXXFLAGS += -stdlib=libc++
	endif
endif

.PHONY: all clean debug dist-clean release test
all: release

release: libamf.a
debug: CXXFLAGS += -g
debug: libamf.a
32bit: CXXFLAGS += -m32
32bit: release
libamf.a: libamf.a(src/serializer.o)

clean:
	rm -f *.a *.o .dep

dist-clean: clean
	$(MAKE) -C tests clean

test: src/serializer.o
	$(MAKE) -C tests
	tests/main

.dep:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM src/serializer.cpp | sed 's,:, $@:,' > $@

ifneq ($(MAKECMDGOALS),clean)
-include .dep
endif
