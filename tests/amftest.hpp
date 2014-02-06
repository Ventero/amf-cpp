#include <vector>

#include "gtest/gtest.h"
#include "amf.hpp"

using namespace amf;

template<typename T>
static void isEqual(const std::vector<u8>& expected, const T& value) {
	v8 serialized = value.serialize();
	isEqual(expected, serialized);
}

template<>
void isEqual(const std::vector<u8>& expected, const std::vector<u8>& serialized) {
	ASSERT_EQ(expected, serialized) << "Expected length " << expected.size()
	                                << ", got " << serialized.size();
}
