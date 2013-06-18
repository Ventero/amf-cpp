CXXFLAGS += -std=c++0x -Wall -Wextra -pedantic
CPPFLAGS += -I.

.PHONY: all clean debug dist-clean release test
all: release

release: libamf.a
debug: CXXFLAGS += -g
debug: libamf.a
32bit: CXXFLAGS += -m32
32bit: release

clean:
	rm -f *.a *.o .dep

dist-clean: clean
	$(MAKE) -C tests clean

test: serializer.o
	$(MAKE) -C tests
	tests/main

libamf.a: serializer.o
	$(AR) $(ARFLAGS) $@ $<

.dep:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM serializer.cpp | sed 's,:, $@:,' > $@

ifneq ($(MAKECMDGOALS),clean)
	-include .dep
endif
