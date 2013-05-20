CXXFLAGS += -g -Wall -Wextra -pthread -std=c++0x

GTEST_DIR = googletest
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include
# and include AMF libs
CPPFLAGS += -I..

# Tests produced by this Makefile
TESTS = main
# Tests used by the main test
SUBTESTS = bool double integer null string undefined

all : $(TESTS)

clean :
	rm -f $(TESTS) gtest.a gtest_main.a *.o .dep

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# our own build rules
main.o : main.cpp $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

$(addsuffix .o,$(SUBTESTS)) : %.o : %.cpp $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

main : main.o $(addsuffix .o,$(SUBTESTS)) ../serializer.o gtest.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@

# gtest build rules
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

-include .dep
.dep:
	$(CXX) $(CPPFLAGS) -MM $(addsuffix .cpp,$(TESTS)) \
		$(addsuffix .cpp,$(SUBTESTS)) | sed 's,:, $@:,' > $@
