CXXFLAGS += -std=c++0x -Wall -Wextra -pedantic
CPPFLAGS += -I.

.PHONY: all clean debug dist-clean release test
all: release

release: libamf.a
debug: CXXFLAGS += -g
debug: libamf.a

clean:
	rm -f *.a *.o

dist-clean: clean
	$(MAKE) -C tests clean

test:
	$(MAKE) -C tests
	tests/main

libamf.a: serializer.o
	$(AR) $(ARFLAGS) $@ $<
