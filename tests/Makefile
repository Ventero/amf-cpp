CXXFLAGS += -g -Wall -Wextra -pedantic -pthread -std=c++0x
LDFLAGS += -lpthread

ifneq ($(shell $(CXX) --version | grep clang),)
	ifeq ($(shell uname -s),Darwin)
		CXXFLAGS += -stdlib=libc++
	endif
endif

GTEST_DIR = googletest
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include
# and add AMF and test base directories to the include paths
CPPFLAGS += -I../src -I.

# Source files
SRC = $(wildcard *.cpp) $(wildcard types/*.cpp)
OBJ = $(SRC:.cpp=.o)

# Precompiled header name
PCH = amftest.hpp

# gtest files
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

all: main

clean:
	rm -f main $(OBJ) .dep $(PCH).gch

dist-clean: clean
	rm -f gtest-all.o gtest_main.o

$(OBJ): %.o : %.cpp $(GTEST_HEADERS) $(PCH).gch
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

main: $(OBJ) ../libamf.a gtest-all.o gtest_main.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

$(PCH).gch: $(PCH)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

# gtest build rules
gtest-all.o: $(GTEST_SRCS)
	$(CXX) $(CPPFLAGS) -isystem $(GTEST_DIR) $(CXXFLAGS) \
		-c $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o: $(GTEST_SRCS)
	$(CXX) $(CPPFLAGS) -isystem $(GTEST_DIR) $(CXXFLAGS) \
		-c $(GTEST_DIR)/src/gtest_main.cc

.dep:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM $(SRC) | \
		sed '/^[^[:space:]]/s,^[^:]*: \([^[:space:]]*\)/,\1/&,;s,:, $@:,' > $@
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM -MT $(PCH).gch $(PCH) | \
		sed 's,:, $@:,' >> $@

ifeq ($(filter $(MAKECMDGOALS),clean dist-clean),)
-include .dep
endif
